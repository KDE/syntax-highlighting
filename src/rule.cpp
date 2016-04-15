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

Rule::Rule()
{
}

Rule::~Rule()
{
    qDeleteAll(m_subRules);
}

void Rule::load(QXmlStreamReader &reader)
{
    m_attribute = reader.attributes().value(QStringLiteral("attribute")).toString();
    m_context = reader.attributes().value(QStringLiteral("context")).toString();

    doLoad(reader);

    // TODO load sub-rules
}

int Rule::match(const QString &text, int offset)
{
    auto result = doMatch(text, offset);

    // TODO match sub-rules

    return result;
}

Rule* Rule::create(const QStringRef& name)
{
    qDebug() << name;
    Rule *rule = nullptr;
    if (name == QLatin1String("keyword"))
        rule = new KeywordListRule;

    return rule;
}


QString KeywordListRule::listName() const
{
    return m_listName;
}

void KeywordListRule::setKeywordList(const KeywordList &keywordList)
{
    m_keywordList = keywordList;
}

void KeywordListRule::doLoad(QXmlStreamReader& reader)
{
    m_listName = reader.attributes().value(QStringLiteral("String")).toString();
}

int KeywordListRule::doMatch(const QString& text, int offset)
{
    return offset;
}
