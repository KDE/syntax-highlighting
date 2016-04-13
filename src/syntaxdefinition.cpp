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
    while (!reader.atEnd()) {
        const auto token = reader.readNext();
        switch (token) {
            case QXmlStreamReader::StartElement:
                if (reader.name() == QLatin1String("list")) {
                    qDebug() << "TODO parse keyword list" << reader.attributes().value(QStringLiteral("name"));
                    break;
                }
            case QXmlStreamReader::EndElement:
                return;
            default:
                break;
        }
    }
}
