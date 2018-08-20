/*
    Copyright (C) 2016 Volker Krause <vkrause@kde.org>

    Permission is hereby granted, free of charge, to any person obtaining
    a copy of this software and associated documentation files (the
    "Software"), to deal in the Software without restriction, including
    without limitation the rights to use, copy, modify, merge, publish,
    distribute, sublicense, and/or sell copies of the Software, and to
    permit persons to whom the Software is furnished to do so, subject to
    the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "keywordlist_p.h"

#include <QDebug>
#include <QXmlStreamReader>

using namespace KSyntaxHighlighting;

bool KeywordList::isEmpty() const
{
    return m_keywords.isEmpty();
}

QString KeywordList::name() const
{
    return m_name;
}

QStringList KeywordList::keywords() const
{
    return m_keywords.values();
}

bool KeywordList::contains(const QStringRef &str) const
{
    return contains(str, m_caseSensitive);
}

bool KeywordList::contains(const QStringRef &str, Qt::CaseSensitivity caseSensitivityOverride) const
{
    if (Q_UNLIKELY(caseSensitivityOverride == Qt::CaseInsensitive && m_lowerCaseKeywords.isEmpty())) {
        foreach (const auto &kw, m_keywords)
            m_lowerCaseKeywords.insert(kw.toLower());
    }

    // TODO avoid the copy in toString!
    if (caseSensitivityOverride == Qt::CaseSensitive)
        return m_keywords.contains(str.toString());
    return m_lowerCaseKeywords.contains(str.toString().toLower());
}

void KeywordList::load(QXmlStreamReader& reader)
{
    Q_ASSERT(reader.name() == QLatin1String("list"));
    Q_ASSERT(reader.tokenType() == QXmlStreamReader::StartElement);

    m_name = reader.attributes().value(QStringLiteral("name")).toString();

    while (!reader.atEnd()) {
        switch (reader.tokenType()) {
            case QXmlStreamReader::StartElement:
                if (reader.name() == QLatin1String("item")) {
                    m_keywords.insert(reader.readElementText().trimmed());
                    reader.readNextStartElement();
                    break;
                }
                reader.readNext();
                break;
            case QXmlStreamReader::EndElement:
                reader.readNext();
                return;
            default:
                reader.readNext();
                break;
        }
    }
}

void KeywordList::setCaseSensitivity(Qt::CaseSensitivity caseSensitive)
{
    m_caseSensitive = caseSensitive;
}
