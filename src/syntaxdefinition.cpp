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

#include "syntaxdefinition.h"

#include <QDebug>
#include <QFile>
#include <QXmlStreamReader>

using namespace KateSyntax;

SyntaxDefinition::SyntaxDefinition()
{
}

SyntaxDefinition::~SyntaxDefinition()
{
}

bool SyntaxDefinition::load(const QString& definitionFileName)
{
    qDebug() << "parsing" << definitionFileName;
    QFile file(definitionFileName);
    if (!file.open(QFile::ReadOnly))
        return false;

    QXmlStreamReader reader(&file);
    while (!reader.atEnd()) {
        const auto token = reader.readNext();
        if (token != QXmlStreamReader::StartElement)
            continue;

        if (reader.name() == QLatin1String("highlighting"))
            loadHighlighting(reader);
    }

    return true;
}

void SyntaxDefinition::loadHighlighting(QXmlStreamReader& reader)
{
    Q_ASSERT(reader.name() == QLatin1String("highlighting"));
    Q_ASSERT(reader.tokenType() == QXmlStreamReader::StartElement);

    while (!reader.atEnd()) {
        switch (reader.tokenType()) {
            case QXmlStreamReader::StartElement:
                qDebug() << reader.name() << "begin";
                if (reader.name() == QLatin1String("list")) {
                    KeywordList keywords;
                    keywords.load(reader);
                    qDebug() << "loaded keyword list: " <<  keywords.name();
                    m_keywordLists.push_back(keywords);
                } else if (reader.name() == QLatin1String("contexts")) {
                    loadContexts(reader);
                } else {
                    reader.readNext();
                }
                break;
            case QXmlStreamReader::EndElement:
                qDebug() << reader.name() << "end";
                return;
            default:
                reader.readNext();
                break;
        }
    }
}

void SyntaxDefinition::loadContexts(QXmlStreamReader& reader)
{
    Q_ASSERT(reader.name() == QLatin1String("contexts"));
    Q_ASSERT(reader.tokenType() == QXmlStreamReader::StartElement);

    while (!reader.atEnd()) {
        switch (reader.tokenType()) {
            case QXmlStreamReader::StartElement:
                if (reader.name() == QLatin1String("context")) {
                    Context context;
                    context.load(reader);
                    qDebug() << "loaded context" << context.name() << context.attribute();
                    m_contexts.push_back(context);
                }
                reader.readNext();
                break;
            case QXmlStreamReader::EndElement:
                qDebug() << reader.name() << "end";
                return;
            default:
                reader.readNext();
                break;
        }
    }
}
