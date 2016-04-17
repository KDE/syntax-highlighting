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

#include "rule.h"
#include "syntaxdefinition.h"

#include <QDebug>
#include <QString>
#include <QXmlStreamReader>

using namespace KateSyntax;

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


Rule::Rule() :
    m_def(nullptr),
    m_column(-1),
    m_firstNonSpace(false),
    m_lookAhead(false),
    m_dynamic(false)
{
}

Rule::~Rule()
{
}

SyntaxDefinition* Rule::syntaxDefinition() const
{
    return m_def;
}

void Rule::setSyntaxDefinition(SyntaxDefinition* def)
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

bool Rule::load(QXmlStreamReader &reader)
{
    Q_ASSERT(reader.tokenType() == QXmlStreamReader::StartElement);

    m_attribute = reader.attributes().value(QStringLiteral("attribute")).toString();
    if (reader.name() != QLatin1String("IncludeRules")) // IncludeRules uses this with a different semantic
        m_context.parse(reader.attributes().value(QStringLiteral("context")));
    m_firstNonSpace = reader.attributes().value(QStringLiteral("firstNonSpace")) == QLatin1String("true");
    m_lookAhead = reader.attributes().value(QStringLiteral("lookAhead")) == QLatin1String("true");
    bool colOk = false;
    m_column = reader.attributes().value(QStringLiteral("column")).toInt(&colOk);
    if (!colOk)
        m_column = -1;
    m_dynamic = reader.attributes().value(QStringLiteral("dynamic")) == QLatin1String("true");

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
                    rule->setSyntaxDefinition(m_def);
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
    m_context.resolve(m_def);
    foreach (auto rule, m_subRules)
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
    if (m_firstNonSpace && (offset > 0 || text.at(0).isSpace()))
        return offset;

    if (m_column >= 0 && offset != m_column)
        return offset;

    const auto result = doMatch(text, offset, captures);
    if (result.offset() == offset || result.offset() == text.size())
        return result;

    foreach (auto subRule, m_subRules) {
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
        qWarning() << "Unknown rule type:" << name;

    return Ptr(rule);
}

bool Rule::isDelimiter(QChar c) const
{
    // TODO: this is definition-global and configurable!
    static const QString delimiters = QStringLiteral(".():!+,-<=>%&*/;?[]^{|}~\\ \t");
    return delimiters.contains(c);
}


bool AnyChar::doLoad(QXmlStreamReader& reader)
{
    m_chars = reader.attributes().value(QStringLiteral("String")).toString();
    if (m_chars.size() == 1)
        qDebug() << "AnyChar rule with just one char: use DetectChar instead.";
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
    return true;
}

MatchResult DetectChar::doMatch(const QString& text, int offset, const QStringList &captures)
{
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

    return offset + 1;
}


MatchResult DetectSpaces::doMatch(const QString& text, int offset, const QStringList&)
{
    while(offset < text.size() && text.at(offset).isSpace())
        ++offset;
    return offset;
}


MatchResult Float::doMatch(const QString& text, int offset, const QStringList&)
{
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

bool IncludeRules::doLoad(QXmlStreamReader& reader)
{
    const auto s = reader.attributes().value(QLatin1String("context"));
    auto splitted = s.split(QLatin1String("##"), QString::KeepEmptyParts);
    if (splitted.isEmpty())
        return false;
    m_contextName = splitted.at(0).toString();
    if (splitted.size() > 1)
        m_defName = splitted.at(1).toString();

    return !m_contextName.isEmpty() || !m_defName.isEmpty();
}

MatchResult IncludeRules::doMatch(const QString& text, int offset, const QStringList&)
{
    Q_UNUSED(text);
    qWarning() << "Unresolved include rule for" << m_contextName << "##" << m_defName;
    return offset;
}


MatchResult Int::doMatch(const QString& text, int offset, const QStringList &captures)
{
    while(offset < text.size() && text.at(offset).isDigit())
        ++offset;
    return offset;
}


bool KeywordListRule::doLoad(QXmlStreamReader& reader)
{
    m_listName = reader.attributes().value(QStringLiteral("String")).toString();
    return !m_listName.isEmpty();
}

MatchResult KeywordListRule::doMatch(const QString& text, int offset, const QStringList&)
{
    if (offset > 0 && !isDelimiter(text.at(offset - 1)))
        return offset;

    if (m_keywordList.isEmpty()) {
        Q_ASSERT(syntaxDefinition());
        m_keywordList = syntaxDefinition()->keywordList(m_listName);
    }

    auto newOffset = offset;
    while (text.size() > newOffset && !isDelimiter(text.at(newOffset)))
        ++newOffset;
    if (newOffset == offset)
        return offset;

    // TODO support case-insensitive keywords
    // TODO avoid the copy in mid()
    if (m_keywordList.keywords().contains(text.mid(offset, newOffset - offset)))
        return newOffset;
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
    m_regexp.setPattern(reader.attributes().value(QStringLiteral("String")).toString());
    m_regexp.setMinimal(reader.attributes().value(QStringLiteral("minimal")) == QLatin1String("true"));
    m_regexp.setCaseSensitivity(reader.attributes().value(QStringLiteral("insensitive")) == QLatin1String("true") ? Qt::CaseInsensitive : Qt::CaseSensitive);
    return !m_regexp.isEmpty(); // m_regexp.isValid() would be better, but parses the regexp and thus is way too expensive
}

MatchResult RegExpr::doMatch(const QString& text, int offset, const QStringList &captures)
{
    Q_ASSERT(m_regexp.isValid());

    auto idx = m_regexp.indexIn(text, offset, QRegExp::CaretAtOffset);
    if (idx == offset)
        return MatchResult(offset + m_regexp.matchedLength(), m_regexp.capturedTexts());
    return offset;
}


bool StringDetect::doLoad(QXmlStreamReader& reader)
{
    m_string = reader.attributes().value(QStringLiteral("String")).toString();
    m_caseSensitivity = reader.attributes().value(QStringLiteral("insensitive")) == QLatin1String("true") ? Qt::CaseInsensitive : Qt::CaseSensitive;
    return !m_string.isEmpty();
}

MatchResult StringDetect::doMatch(const QString& text, int offset, const QStringList &captures)
{
    if (text.midRef(offset, m_string.size()).compare(m_string, m_caseSensitivity) == 0)
        return offset + m_string.size();
    return offset;
}


bool WordDetect::doLoad(QXmlStreamReader& reader)
{
    m_word = reader.attributes().value(QStringLiteral("String")).toString();
    return !m_word.isEmpty();
}

MatchResult WordDetect::doMatch(const QString& text, int offset, const QStringList &captures)
{
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
