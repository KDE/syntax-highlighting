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

#include <QDebug>
#include <QXmlStreamReader>

using namespace KateSyntax;

static Theme::Style stringToDefaultFormat(const QStringRef &str)
{
#define D(type) if (str == QLatin1String("ds" #type)) return Theme:: type;
    D(Normal)
    D(Keyword)
    D(Function)
    D(Variable)
    D(ControlFlow)
    D(Operator)
    D(BuiltIn)
    D(Extension)
    D(Preprocessor)
    D(Attribute)
    D(Char)
    D(SpecialChar)
    D(String)
    D(VerbatimString)
    D(SpecialString)
    D(Import)
    D(DataType)
    D(DecVal)
    D(BaseN)
    D(Float)
    D(Constant)
    D(Comment)
    D(Documentation)
    D(Annotation)
    D(CommentVar)
    D(RegionMarker)
    D(Information)
    D(Warning)
    D(Alert)
    D(Error)
    D(Others)
#undef D
    return Theme::Normal;
}

Format::Format() :
    m_default(Theme::Normal),
    m_italic(false),
    m_bold(false),
    m_underline(false),
    m_strikeout(false),
    m_hasColor(false),
    m_hasSelColor(false),
    m_hasBgColor(false)
{
}

Format::~Format()
{
}

QString Format::name() const
{
    return m_name;
}

bool Format::isNormal(const Theme &theme) const
{
    return !hasColor(theme) && !m_hasSelColor && !hasBackgroundColor(theme)
        && !isBold(theme) && !isItalic(theme) && !isUnderline(theme) && !isStrikeThrough(theme);
}

bool Format::hasColor(const Theme &theme) const
{
    return m_hasColor || (theme.normalColor(m_default) & 0xff000000);
}

QColor Format::color(const Theme &theme) const
{
    if (m_hasColor)
        return m_color;
    return QColor(theme.normalColor(m_default));
}

bool Format::hasBackgroundColor(const Theme &theme) const
{
    return m_hasBgColor || (theme.backgroundColor(m_default) & 0xff000000);
}

QColor Format::backgroundColor(const Theme &theme) const
{
    if (m_hasBgColor)
        return m_backgroundColor;
    return QColor(theme.backgroundColor(m_default));
}

bool Format::isBold(const Theme &theme) const
{
    // FIXME: an explicit false here should override theme
    return m_bold || theme.isBold(m_default);
}

bool Format::isItalic(const Theme &theme) const
{
    // FIXME: an explicit false here should override theme
    return m_italic || theme.isItalic(m_default);
}

bool Format::isUnderline(const Theme &theme) const
{
    // FIXME: an explicit false here should override theme
    return m_underline || theme.isUnderline(m_default);
}

bool Format::isStrikeThrough(const Theme &theme) const
{
    // FIXME: an explicit false here should override theme
    return m_strikeout || theme.isStrikeThrough(m_default);
}

void Format::load(QXmlStreamReader& reader)
{
    m_name = reader.attributes().value(QStringLiteral("name")).toString();
    m_default = stringToDefaultFormat(reader.attributes().value(QStringLiteral("defStyleNum")));

    auto colStr = reader.attributes().value(QStringLiteral("color"));
    if ((m_hasColor = !colStr.isEmpty()))
        m_color = QColor(colStr.toString());
    colStr = reader.attributes().value(QStringLiteral("selColor"));
    if ((m_hasSelColor = !colStr.isEmpty()))
        m_selColor = QColor(colStr.toString());
    colStr = reader.attributes().value(QStringLiteral("backgroundColor"));
    if ((m_hasBgColor = !colStr.isEmpty()))
        m_backgroundColor = QColor(colStr.toString());

    m_italic = reader.attributes().value(QStringLiteral("italic")) == QLatin1String("true");
    m_bold = reader.attributes().value(QStringLiteral("bold")) == QLatin1String("true");
    m_underline = reader.attributes().value(QStringLiteral("underline")) == QLatin1String("true");
    m_strikeout = reader.attributes().value(QStringLiteral("strikeOut")) == QLatin1String("true");
}
