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

#include "format.h"

#include <QXmlStreamReader>

using namespace KateSyntax;

Format::Format() :
    m_italic(false),
    m_bold(false),
    m_underline(false),
    m_strikeout(false)
{
}

Format::~Format()
{
}

QString Format::name() const
{
    return m_name;
}

QColor Format::color() const
{
    return m_color;
}

void Format::load(QXmlStreamReader& reader)
{
    m_name = reader.attributes().value(QStringLiteral("name")).toString();

    auto colStr = reader.attributes().value(QStringLiteral("color"));
    if (!colStr.isEmpty())
        m_color = QColor(colStr.toString());
    colStr = reader.attributes().value(QStringLiteral("selColor"));
    if (!colStr.isEmpty())
        m_selColor = QColor(colStr.toString());

    m_italic = reader.attributes().value(QStringLiteral("italic")) == QLatin1String("true");
    m_bold = reader.attributes().value(QStringLiteral("bold")) == QLatin1String("true");
    m_underline = reader.attributes().value(QStringLiteral("underline")) == QLatin1String("true");
    m_strikeout = reader.attributes().value(QStringLiteral("strikeout")) == QLatin1String("true");
}
