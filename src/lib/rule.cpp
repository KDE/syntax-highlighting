/*
    Copyright (C) 2016 Volker Krause <vkrause@kde.org>

    This program is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "rule_p.h"
#include "definition_p.h"
#include "ksyntaxhighlighting_logging.h"
#include "xml_p.h"

#include <QDebug>
#include <QString>
#include <QXmlStreamReader>

using namespace KSyntaxHighlighting;

static bool isOctalChar(QChar c)
{
    return c.isNumber() && c != QLatin1Char('9') && c != QLatin1Char('8');
}

static bool isHexChar(QChar c)
{
    return c.isNumber()
        || c == QLatin1Char('a') || c == QLatin1Char('A')
        || c == QLatin1Char('b') || c == QLatin1Char('B')
        || c == QLatin1Char('c') || c == QLatin1Char('C')
        || c == QLatin1Char('d') || c == QLatin1Char('D')
        || c == QLatin1Char('e') || c == QLatin1Char('E')
        || c == QLatin1Char('f') || c == QLatin1Char('F');
}

static int matchEscapedChar(const QString &text, int offset)
{
    if (text.at(offset) != QLatin1Char('\\') || text.size() < offset + 2)
        return offset;

    const auto c = text.at(offset + 1);
    static const auto controlChars = QStringLiteral("abefnrtv\"'?\\");
    if (controlChars.contains(c))
        return offset + 2;

    if (c == QLatin1Char('x')) { // hex encoded character
        auto newOffset = offset + 2;
        for (int i = 0; i < 2 && newOffset + i < text.size(); ++i, ++newOffset) {
            if (!isHexChar(text.at(newOffset)))
                break;
        }
        if (newOffset == offset + 2)
            return offset;
        return newOffset;
    }

    if (isOctalChar(c)) { // octal encoding
        auto newOffset = offset + 2;
        for (int i = 0; i < 2 && newOffset + i < text.size(); ++i, ++newOffset) {
            if (!isOctalChar(text.at(newOffset)))
                break;
        }
        if (newOffset == offset + 2)
            return offset;
        return newOffset;
    }

    return offset;
}

static QString quoteCapture(const QString &capture)
{
    auto quoted = capture;
    return quoted.replace(QRegularExpression(QStringLiteral("(\\W)")), QStringLiteral("\\\\1"));
}

static QString replaceCaptures(const QString &pattern, const QStringList &captures, bool quote)
{
    auto result = pattern;
    for (int i = 1; i < captures.size(); ++i) {
        result.replace(QLatin1Char('%') + QString::number(i), quote ? quoteCapture(captures.at(i)) : captures.at(i));
    }
    return result;
}


Rule::Rule() :
    m_column(-1),
    m_firstNonSpace(false),
    m_lookAhead(false),
    m_dynamic(false)
{
}

Rule::~Rule()
{
}

Definition Rule::definition() const
{
    return m_def.definition();
}

void Rule::setDefinition(const Definition &def)
{
    m_def = def;
}

QString Rule::attribute() const
{
    return m_attribute;
}

ContextSwitch Rule::context() const
{
    return m_context;
}

bool Rule::isLookAhead() const
{
    return m_lookAhead;
}

bool Rule::isDynamic() const
{
    return m_dynamic;
}

bool Rule::firstNonSpace() const
{
    return m_firstNonSpace;
}

int Rule::requiredColumn() const
{
    return m_column;
}

FoldingRegion Rule::beginRegion() const
{
    return m_beginRegion;
}

FoldingRegion Rule::endRegion() const
{
    return m_endRegion;
}

bool Rule::load(QXmlStreamReader &reader)
{
    Q_ASSERT(reader.tokenType() == QXmlStreamReader::StartElement);

    m_attribute = reader.attributes().value(QStringLiteral("attribute")).toString();
    if (reader.name() != QLatin1String("IncludeRules")) // IncludeRules uses this with a different semantic
        m_context.parse(reader.attributes().value(QStringLiteral("context")));
    m_firstNonSpace = Xml::attrToBool(reader.attributes().value(QStringLiteral("firstNonSpace")));
    m_lookAhead = Xml::attrToBool(reader.attributes().value(QStringLiteral("lookAhead")));
    bool colOk = false;
    m_column = reader.attributes().value(QStringLiteral("column")).toInt(&colOk);
    if (!colOk)
        m_column = -1;
    m_dynamic = Xml::attrToBool(reader.attributes().value(QStringLiteral("dynamic")));

    auto regionName = reader.attributes().value(QLatin1String("beginRegion"));
    if (!regionName.isEmpty())
        m_beginRegion = FoldingRegion(FoldingRegion::Begin, DefinitionData::get(m_def.definition())->foldingRegionId(regionName.toString()));
    regionName = reader.attributes().value(QLatin1String("endRegion"));
    if (!regionName.isEmpty())
        m_endRegion = FoldingRegion(FoldingRegion::End, DefinitionData::get(m_def.definition())->foldingRegionId(regionName.toString()));

    auto result = doLoad(reader);

    if (m_lookAhead && m_context.isStay())
        result = false;

    reader.readNext();
    while (!reader.atEnd()) {
        switch (reader.tokenType()) {
            case QXmlStreamReader::StartElement:
            {
                auto rule = Rule::create(reader.name());
                if (rule) {
                    rule->setDefinition(m_def.definition());
                    if (rule->load(reader)) {
                        m_subRules.push_back(rule);
                        reader.readNext();
                    }
                } else {
                    reader.skipCurrentElement();
                }
                break;
            }
            case QXmlStreamReader::EndElement:
                return result;
            default:
                reader.readNext();
                break;
        }
    }

    return result;
}

void Rule::resolveContext()
{
    m_context.resolve(m_def.definition());
    foreach (const auto &rule, m_subRules)
        rule->resolveContext();
}

bool Rule::doLoad(QXmlStreamReader& reader)
{
    Q_UNUSED(reader);
    return true;
}

MatchResult Rule::match(const QString &text, int offset, const QStringList &captures)
{
    Q_ASSERT(!text.isEmpty());

    const auto result = doMatch(text, offset, captures);
    if (result.offset() == offset || result.offset() == text.size())
        return result;

    foreach (const auto &subRule, m_subRules) {
        const auto subResult = subRule->match(text, result.offset(), QStringList());
        if (subResult.offset() > result.offset())
            return MatchResult(subResult.offset(), result.captures());
    }

    return result;
}

Rule::Ptr Rule::create(const QStringRef& name)
{
    Rule *rule = nullptr;
    if (name == QLatin1String("AnyChar"))
        rule = new AnyChar;
    else if (name == QLatin1String("DetectChar"))
        rule = new DetectChar;
    else if (name == QLatin1String("Detect2Chars"))
        rule = new Detect2Char;
    else if (name == QLatin1String("DetectIdentifier"))
        rule = new DetectIdentifier;
    else if (name == QLatin1String("DetectSpaces"))
        rule = new DetectSpaces;
    else if (name == QLatin1String("Float"))
        rule = new Float;
    else if (name == QLatin1String("Int"))
        rule = new Int;
    else if (name == QLatin1String("HlCChar"))
        rule = new HlCChar;
    else if (name == QLatin1String("HlCHex"))
        rule = new HlCHex;
    else if (name == QLatin1String("HlCOct"))
        rule = new HlCOct;
    else if (name == QLatin1String("HlCStringChar"))
        rule = new HlCStringChar;
    else if (name == QLatin1String("IncludeRules"))
        rule = new IncludeRules;
    else if (name == QLatin1String("keyword"))
        rule = new KeywordListRule;
    else if (name == QLatin1String("LineContinue"))
        rule = new LineContinue;
    else if (name == QLatin1String("RangeDetect"))
        rule = new RangeDetect;
    else if (name == QLatin1String("RegExpr"))
        rule = new RegExpr;
    else if (name == QLatin1String("StringDetect"))
        rule = new StringDetect;
    else if (name == QLatin1String("WordDetect"))
        rule = new WordDetect;
    else
        qCWarning(Log) << "Unknown rule type:" << name;

    return Ptr(rule);
}

bool Rule::isDelimiter(QChar c) const
{
    auto defData = DefinitionData::get(m_def.definition());
    return defData->isDelimiter(c);
}


bool AnyChar::doLoad(QXmlStreamReader& reader)
{
    m_chars = reader.attributes().value(QStringLiteral("String")).toString();
    if (m_chars.size() == 1)
        qCDebug(Log) << "AnyChar rule with just one char: use DetectChar instead.";
    return !m_chars.isEmpty();
}

MatchResult AnyChar::doMatch(const QString& text, int offset, const QStringList&)
{
    if (m_chars.contains(text.at(offset)))
        return offset + 1;
    return offset;
}


bool DetectChar::doLoad(QXmlStreamReader& reader)
{
    const auto s = reader.attributes().value(QStringLiteral("char"));
    if (s.isEmpty())
        return false;
    m_char = s.at(0);
    if (isDynamic()) {
        m_captureIndex = m_char.digitValue();
    }
    return true;
}

MatchResult DetectChar::doMatch(const QString& text, int offset, const QStringList &captures)
{
    if (isDynamic()) {
        if (captures.size() <= m_captureIndex || captures.at(m_captureIndex).isEmpty())
            return offset;
        if (text.at(offset) == captures.at(m_captureIndex).at(0))
            return offset + 1;
        return offset;
    }

    if (text.at(offset) == m_char)
        return offset + 1;
    return offset;
}


bool Detect2Char::doLoad(QXmlStreamReader& reader)
{
    const auto s1 = reader.attributes().value(QStringLiteral("char"));
    const auto s2 = reader.attributes().value(QStringLiteral("char1"));
    if (s1.isEmpty() || s2.isEmpty())
        return false;
    m_char1 = s1.at(0);
    m_char2 = s2.at(0);
    return true;
}

MatchResult Detect2Char::doMatch(const QString& text, int offset, const QStringList &captures)
{
    Q_UNUSED(captures); // TODO
    if (text.size() - offset < 2)
        return offset;
    if (text.at(offset) == m_char1 && text.at(offset + 1) == m_char2)
        return offset + 2;
    return offset;
}


MatchResult DetectIdentifier::doMatch(const QString& text, int offset, const QStringList&)
{
    if (!text.at(offset).isLetter() && text.at(offset) != QLatin1Char('_'))
        return offset;

    for (int i = offset + 1; i < text.size(); ++i) {
        const auto c = text.at(i);
        if (!c.isLetterOrNumber() && c != QLatin1Char('_'))
            return i;
    }

    return text.size();
}


MatchResult DetectSpaces::doMatch(const QString& text, int offset, const QStringList&)
{
    while(offset < text.size() && text.at(offset).isSpace())
        ++offset;
    return offset;
}


MatchResult Float::doMatch(const QString& text, int offset, const QStringList&)
{
    if (offset > 0 && !isDelimiter(text.at(offset - 1)))
        return offset;

    auto newOffset = offset;
    while (newOffset < text.size() && text.at(newOffset).isDigit())
        ++newOffset;

    if (newOffset >= text.size() || text.at(newOffset) != QLatin1Char('.'))
        return offset;
    ++newOffset;

    while (newOffset < text.size() && text.at(newOffset).isDigit())
        ++newOffset;

    if (newOffset == offset + 1) // we only found a decimal point
        return offset;

    auto expOffset = newOffset;
    if (expOffset >= text.size() || (text.at(expOffset) != QLatin1Char('e') && text.at(expOffset) != QLatin1Char('E')))
        return newOffset;
    ++expOffset;

    if (expOffset < text.size() && (text.at(expOffset) == QLatin1Char('+') || text.at(expOffset) == QLatin1Char('-')))
        ++expOffset;
    bool foundExpDigit = false;
    while (expOffset < text.size() && text.at(expOffset).isDigit()) {
        ++expOffset;
        foundExpDigit = true;
    }

    if (!foundExpDigit)
        return newOffset;
    return expOffset;
}


MatchResult HlCChar::doMatch(const QString& text, int offset, const QStringList&)
{
    if (text.size() < offset + 3)
        return offset;

    if (text.at(offset) != QLatin1Char('\'') || text.at(offset + 1) == QLatin1Char('\''))
        return offset;

    auto newOffset = matchEscapedChar(text, offset + 1);
    if (newOffset == offset + 1) {
        if (text.at(newOffset) == QLatin1Char('\\'))
            return offset;
        else
            ++newOffset;
    }
    if (newOffset >= text.size())
        return offset;

    if (text.at(newOffset) == QLatin1Char('\''))
        return newOffset + 1;

    return offset;
}


MatchResult HlCHex::doMatch(const QString& text, int offset, const QStringList&)
{
    if (offset > 0 && !isDelimiter(text.at(offset - 1)))
        return offset;

    if (text.size() < offset + 3)
        return offset;

    if (text.at(offset) != QLatin1Char('0') || (text.at(offset + 1) != QLatin1Char('x') && text.at(offset + 1) != QLatin1Char('X')))
        return offset;

    if (!isHexChar(text.at(offset + 2)))
        return offset;

    offset += 3;
    while (offset < text.size() && isHexChar(text.at(offset)))
        ++offset;

    // TODO Kate matches U/L suffix, QtC does not?

    return offset;
}


MatchResult HlCOct::doMatch(const QString& text, int offset, const QStringList&)
{
    if (offset > 0 && !isDelimiter(text.at(offset - 1)))
        return offset;

    if (text.size() < offset + 2)
        return offset;

    if (text.at(offset) != QLatin1Char('0'))
        return offset;

    if (!isOctalChar(text.at(offset + 1)))
        return offset;

    offset += 2;
    while (offset < text.size() && isOctalChar(text.at(offset)))
        ++offset;

    return offset;
}


MatchResult HlCStringChar::doMatch(const QString& text, int offset, const QStringList&)
{
    return matchEscapedChar(text, offset);
}


QString IncludeRules::contextName() const
{
    return m_contextName;
}

QString IncludeRules::definitionName() const
{
    return m_defName;
}

bool IncludeRules::includeAttribute() const
{
    return m_includeAttribute;
}

bool IncludeRules::doLoad(QXmlStreamReader& reader)
{
    const auto s = reader.attributes().value(QLatin1String("context"));
    auto splitted = s.split(QLatin1String("##"), QString::KeepEmptyParts);
    if (splitted.isEmpty())
        return false;
    m_contextName = splitted.at(0).toString();
    if (splitted.size() > 1)
        m_defName = splitted.at(1).toString();
    m_includeAttribute = Xml::attrToBool(reader.attributes().value(QLatin1String("includeAttrib")));

    return !m_contextName.isEmpty() || !m_defName.isEmpty();
}

MatchResult IncludeRules::doMatch(const QString& text, int offset, const QStringList&)
{
    Q_UNUSED(text);
    qCWarning(Log) << "Unresolved include rule for" << m_contextName << "##" << m_defName;
    return offset;
}


MatchResult Int::doMatch(const QString& text, int offset, const QStringList &captures)
{
    if (offset > 0 && !isDelimiter(text.at(offset - 1)))
        return offset;

    Q_UNUSED(captures); // ### the doc says this can be dynamic, but how??
    while(offset < text.size() && text.at(offset).isDigit())
        ++offset;
    return offset;
}


bool KeywordListRule::doLoad(QXmlStreamReader& reader)
{
    m_listName = reader.attributes().value(QLatin1String("String")).toString();
    if (reader.attributes().hasAttribute(QLatin1String("insensitive"))) {
        m_hasCaseSensitivityOverride = true;
        m_caseSensitivityOverride = Xml::attrToBool(reader.attributes().value(QLatin1String("insensitive"))) ?
            Qt::CaseInsensitive : Qt::CaseSensitive;
    } else {
        m_hasCaseSensitivityOverride = false;
    }
    return !m_listName.isEmpty();
}

MatchResult KeywordListRule::doMatch(const QString& text, int offset, const QStringList&)
{
    if (offset > 0 && !isDelimiter(text.at(offset - 1)))
        return offset;

    if (m_keywordList.isEmpty()) {
        const auto def = definition();
        Q_ASSERT(def.isValid());
        auto defData = DefinitionData::get(def);
        m_keywordList = defData->keywordList(m_listName);
    }

    auto newOffset = offset;
    while (text.size() > newOffset && !isDelimiter(text.at(newOffset)))
        ++newOffset;
    if (newOffset == offset)
        return offset;

    if (m_hasCaseSensitivityOverride) {
        if (m_keywordList.contains(text.midRef(offset, newOffset - offset), m_caseSensitivityOverride))
            return newOffset;
    } else {
        if (m_keywordList.contains(text.midRef(offset, newOffset - offset)))
            return newOffset;
    }
    return offset;
}


bool LineContinue::doLoad(QXmlStreamReader& reader)
{
    const auto s = reader.attributes().value(QStringLiteral("char"));
    if (s.isEmpty())
        m_char = QLatin1Char('\\');
    else
        m_char = s.at(0);
    return true;
}

MatchResult LineContinue::doMatch(const QString& text, int offset, const QStringList&)
{
    if (offset == text.size() - 1 && text.at(offset) == m_char)
        return offset + 1;
    return offset;
}


bool RangeDetect::doLoad(QXmlStreamReader& reader)
{
    const auto s1 = reader.attributes().value(QStringLiteral("char"));
    const auto s2 = reader.attributes().value(QStringLiteral("char1"));
    if (s1.isEmpty() || s2.isEmpty())
        return false;
    m_begin = s1.at(0);
    m_end = s2.at(0);
    return true;
}

MatchResult RangeDetect::doMatch(const QString& text, int offset, const QStringList&)
{
    if (text.size() - offset < 2)
        return offset;
    if (text.at(offset) != m_begin)
        return offset;

    auto newOffset = offset + 1;
    while (newOffset < text.size()) {
        if (text.at(newOffset) == m_end)
            return newOffset + 1;
        ++newOffset;
    }
    return offset;
}

bool RegExpr::doLoad(QXmlStreamReader& reader)
{
    m_pattern = reader.attributes().value(QStringLiteral("String")).toString();
    m_regexp.setPattern(m_pattern);
    const auto isMinimal = Xml::attrToBool(reader.attributes().value(QStringLiteral("minimal")));
    const auto isCaseInsensitive = Xml::attrToBool(reader.attributes().value(QStringLiteral("insensitive")));
    m_regexp.setPatternOptions(
        (isMinimal ? QRegularExpression::InvertedGreedinessOption : QRegularExpression::NoPatternOption) |
        (isCaseInsensitive ? QRegularExpression::CaseInsensitiveOption : QRegularExpression::NoPatternOption));
    return !m_pattern.isEmpty(); // m_regexp.isValid() would be better, but parses the regexp and thus is way too expensive
}

MatchResult RegExpr::doMatch(const QString& text, int offset, const QStringList &captures)
{
    Q_ASSERT(m_regexp.isValid());

    if (isDynamic())
        m_regexp.setPattern(replaceCaptures(m_pattern, captures, true));

    auto result = m_regexp.match(text, offset, QRegularExpression::NormalMatch, QRegularExpression::DontCheckSubjectStringMatchOption);
    if (result.capturedStart() == offset)
        return MatchResult(offset + result.capturedLength(), result.capturedTexts());
    return MatchResult(offset, result.capturedStart());
}


bool StringDetect::doLoad(QXmlStreamReader& reader)
{
    m_string = reader.attributes().value(QStringLiteral("String")).toString();
    m_caseSensitivity = Xml::attrToBool(reader.attributes().value(QStringLiteral("insensitive"))) ? Qt::CaseInsensitive : Qt::CaseSensitive;
    return !m_string.isEmpty();
}

MatchResult StringDetect::doMatch(const QString& text, int offset, const QStringList &captures)
{
    auto pattern = m_string;
    if (isDynamic())
        pattern = replaceCaptures(m_string, captures, false);
    if (text.midRef(offset, pattern.size()).compare(pattern, m_caseSensitivity) == 0)
        return offset + pattern.size();
    return offset;
}


bool WordDetect::doLoad(QXmlStreamReader& reader)
{
    m_word = reader.attributes().value(QStringLiteral("String")).toString();
    return !m_word.isEmpty();
}

MatchResult WordDetect::doMatch(const QString& text, int offset, const QStringList &captures)
{
    Q_UNUSED(captures); // TODO
    if (text.size() - offset < m_word.size())
        return offset;

    if (offset > 0 && !isDelimiter(text.at(offset - 1)))
        return offset;

    if (text.midRef(offset, m_word.size()) != m_word)
        return offset;

    if (text.size() == offset + m_word.size() || isDelimiter(text.at(offset + m_word.size())))
        return offset + m_word.size();

    return offset;
}
