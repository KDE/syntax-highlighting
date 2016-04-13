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

#include "context.h"

#include <QDebug>
#include <QString>
#include <QXmlStreamReader>

using namespace KateSyntax;

Context::Context()
{
}

Context::~Context()
{
}

QString Context::name() const
{
    return m_name;
}

QString Context::attribute() const
{
    return m_attribute;
}

void Context::load(QXmlStreamReader& reader)
{
    Q_ASSERT(reader.name() == QLatin1String("context"));
    Q_ASSERT(reader.tokenType() == QXmlStreamReader::StartElement);

    m_name = reader.attributes().value(QStringLiteral("name")).toString();
    m_attribute = reader.attributes().value(QStringLiteral("attribute")).toString();

    while (!reader.atEnd()) {
        switch (reader.tokenType()) {
            case QXmlStreamReader::StartElement:
                qDebug() << reader.name() << "begin";
                reader.skipCurrentElement();
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
