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

// TODO move this outside and make it changeable
struct def_format_t {
    quint32 normalColor;
};

static const def_format_t default_formats[] {
    { 0x00000000 }, // Normal
    { 0xff1f1c1b }, // Keyword
    { 0xff644a9b }, // Function
    { 0xff0057ae }, // Variable
    { 0xff1f1c1b }, // ControlFlow
    { 0xff1f1c1b }, // Operator
    { 0xff644a9b }, // BuiltIn
    { 0xff0095ff }, // Extension
    { 0xff006e28 }, // Preprocessor
    { 0xff0057ae }, // Attribute
    { 0xff924c9d }, // Char
    { 0xff3daee9 }, // SpecialChar
    { 0xffbf0303 }, // String
    { 0xff0057ae }, // VerbatimString
    { 0xffff5500 }, // SpecialString
    { 0xffff5500 }, // Import
    { 0xff0057ae }, // DataType
    { 0xffb08000 }, // DecVal
    { 0xffb08000 }, // BaseN
    { 0xffb08000 }, // Float
    { 0xffaa5500 }, // Constant
    { 0xff898887 }, // Comment
    { 0xff607880 }, // Documentation
    { 0xffca60ca }, // Annotation
    { 0xff0095ff }, // CommentVar
    { 0xff0057ae }, // RegionMarker
    { 0xffb08000 }, // Information
    { 0xffbf0303 }, // Warning
    { 0xffbf0303 }, // Alert
    { 0xffbf0303 }, // Error
    { 0xff006e28 }, // Others

};

static Format::DefaultFormat stringToDefaultFormat(const QStringRef &str)
{
#define D(type) if (str == QLatin1String("ds" #type)) return Format:: type;
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
    return Format::Normal;
}

Format::Format() :
    m_default(Normal),
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

bool Format::isNormal() const
{
    return !hasColor() && !m_hasSelColor && !m_hasBgColor && !m_bold && !m_italic && !m_underline && !m_strikeout;
}

bool Format::hasColor() const
{
    return m_hasColor || (default_formats[m_default].normalColor & 0xff000000);
}

QColor Format::color() const
{
    if (m_hasColor)
        return m_color;
    return QColor(QRgb(default_formats[m_default].normalColor));
}

bool Format::hasBackgroundColor() const
{
    return m_hasBgColor;
}

QColor Format::backgroundColor() const
{
    return m_backgroundColor;
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
    m_strikeout = reader.attributes().value(QStringLiteral("strikeout")) == QLatin1String("true");
}
