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

#include <QDebug>
#include <QString>
#include <QXmlStreamReader>

using namespace KateSyntax;

Rule::Rule() :
    m_firstNonSpace(false)
{
}

Rule::~Rule()
{
    qDeleteAll(m_subRules);
}

QString Rule::attribute() const
{
    return m_attribute;
}

QString Rule::context() const
{
    return m_context;
}

bool Rule::load(QXmlStreamReader &reader)
{
    m_attribute = reader.attributes().value(QStringLiteral("attribute")).toString();
    m_context = reader.attributes().value(QStringLiteral("context")).toString();
    if (m_context.isEmpty())
        m_context = QStringLiteral("#stay");
    m_firstNonSpace = reader.attributes().value(QStringLiteral("firstNonSpace")) == QLatin1String("true");

    auto result = doLoad(reader);

    // TODO load sub-rules

    reader.readNextStartElement();
    return result;
}

bool Rule::doLoad(QXmlStreamReader& reader)
{
    Q_UNUSED(reader);
    return true;
}

int Rule::match(const QString &text, int offset)
{
    Q_ASSERT(!text.isEmpty());
    if (m_firstNonSpace && (offset > 0 || text.at(0).isSpace()))
        return false;

    auto result = doMatch(text, offset);

    // TODO match sub-rules

    return result;
}

Rule* Rule::create(const QStringRef& name)
{
    qDebug() << name;
    Rule *rule = nullptr;
    if (name == QLatin1String("AnyChar"))
        rule = new AnyChar;
    else if (name == QLatin1String("DetectChar"))
        rule = new DetectChar;
    else if (name == QLatin1String("Detect2Chars"))
        rule = new Detect2Char;
    else if (name == QLatin1String("DetectSpaces"))
        rule = new DetectSpaces;
    else if (name == QLatin1String("keyword"))
        rule = new KeywordListRule;
    else if (name == QLatin1String("RegExpr"))
        rule = new RegExpr;
    else if (name == QLatin1String("WordDetect"))
        rule = new WordDetect;
    else
        qDebug() << name << "rule not yet implemented";

    return rule;
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

int AnyChar::doMatch(const QString& text, int offset)
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

int DetectChar::doMatch(const QString& text, int offset)
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

int Detect2Char::doMatch(const QString& text, int offset)
{
    if (text.size() - offset < 2)
        return offset;
    if (text.at(offset) == m_char1 && text.at(offset + 1) == m_char2)
        return offset + 2;
    return offset;
}


int DetectSpaces::doMatch(const QString& text, int offset)
{
    while(offset < text.size() && text.at(offset).isSpace())
        ++offset;
    return offset;
}


QString KeywordListRule::listName() const
{
    return m_listName;
}

void KeywordListRule::setKeywordList(const KeywordList &keywordList)
{
    m_keywordList = keywordList;
}

bool KeywordListRule::doLoad(QXmlStreamReader& reader)
{
    m_listName = reader.attributes().value(QStringLiteral("String")).toString();
    return !m_listName.isEmpty();
}

int KeywordListRule::doMatch(const QString& text, int offset)
{
    if (offset > 0 && !isDelimiter(text.at(offset - 1)))
        return offset;

    int offset2 = offset;
    int wordLen = 0;
    int len = text.size();

    while ((len > offset2) && !isDelimiter(text[offset2])) {
        offset2++;
        wordLen++;
    }

    // TODO support case-insensitive keywords
    if (m_keywordList.keywords().contains(text.mid(offset, wordLen)))
        return offset2;
    return offset;
}


bool RegExpr::doLoad(QXmlStreamReader& reader)
{
    m_regexp.setPattern(reader.attributes().value(QStringLiteral("String")).toString());
    m_regexp.setMinimal(reader.attributes().value(QStringLiteral("minimal")) != QLatin1String("true"));
    m_regexp.setCaseSensitivity(reader.attributes().value(QStringLiteral("insensitive")) == QLatin1String("true") ? Qt::CaseInsensitive : Qt::CaseSensitive);
    return m_regexp.isValid();
}

int RegExpr::doMatch(const QString& text, int offset)
{
    Q_ASSERT(m_regexp.isValid());

    auto idx = m_regexp.indexIn(text, offset, QRegExp::CaretAtOffset);
    if (idx == offset)
        return offset + m_regexp.matchedLength();
    return offset;
}


bool WordDetect::doLoad(QXmlStreamReader& reader)
{
    m_word = reader.attributes().value(QStringLiteral("String")).toString();
    return !m_word.isEmpty();
}

int WordDetect::doMatch(const QString& text, int offset)
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
