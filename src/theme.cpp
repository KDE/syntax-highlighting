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

#include "theme.h"

using namespace KateSyntax;

namespace KateSyntax {

struct ThemeData {
    QRgb normalColor;
    QRgb backgroundColor;
    bool bold;
    bool italic;
    bool underline;
    bool strikeThrough;
};

static const ThemeData normal_default_theme[] {
    { 0x00000000, 0x00000000, false, false, false, false }, // Normal
    { 0x00000000, 0x00000000, true , false, false, false }, // Keyword
    { 0xff644a9b, 0x00000000, false, false, false, false }, // Function
    { 0xff0057ae, 0x00000000, false, false, false, false }, // Variable
    { 0x00000000, 0x00000000, true , false, false, false }, // ControlFlow
    { 0x00000000, 0x00000000, false, false, false, false }, // Operator
    { 0xff644a9b, 0x00000000, false, false, false, false }, // BuiltIn
    { 0xff0095ff, 0x00000000, true , false, false, false }, // Extension
    { 0xff006e28, 0x00000000, false, false, false, false }, // Preprocessor
    { 0xff0057ae, 0x00000000, false, false, false, false }, // Attribute
    { 0xff924c9d, 0x00000000, false, false, false, false }, // Char
    { 0xff3daee9, 0x00000000, false, false, false, false }, // SpecialChar
    { 0xffbf0303, 0x00000000, false, false, false, false }, // String
    { 0xff0057ae, 0x00000000, false, false, false, false }, // VerbatimString
    { 0xffff5500, 0x00000000, false, false, false, false }, // SpecialString
    { 0xffff5500, 0x00000000, false, false, false, false }, // Import
    { 0xff0057ae, 0x00000000, false, false, false, false }, // DataType
    { 0xffb08000, 0x00000000, false, false, false, false }, // DecVal
    { 0xffb08000, 0x00000000, false, false, false, false }, // BaseN
    { 0xffb08000, 0x00000000, false, false, false, false }, // Float
    { 0xffaa5500, 0x00000000, false, false, false, false }, // Constant
    { 0xff898887, 0x00000000, false, false, false, false }, // Comment
    { 0xff607880, 0x00000000, false, false, false, false }, // Documentation
    { 0xffca60ca, 0x00000000, false, false, false, false }, // Annotation
    { 0xff0095ff, 0x00000000, false, false, false, false }, // CommentVar
    { 0xff0057ae, 0xffe0e9f8, false, false, false, false }, // RegionMarker
    { 0xffb08000, 0x00000000, false, false, false, false }, // Information
    { 0xffbf0303, 0x00000000, false, false, false, false }, // Warning
    { 0xffbf0303, 0xfff7e6e6, true , false, false, false }, // Alert
    { 0xffbf0303, 0x00000000, false, false, true , false }, // Error
    { 0xff006e28, 0x00000000, false, false, false, false }, // Others
};

static void noDeleter(ThemeData*) {}

}

Theme::Theme()
{
}

Theme::~Theme()
{
}

Theme Theme::defaultTheme(Theme::DefaultTheme t)
{
    Theme theme;
    switch (t) {
        case NormalTheme:
            theme.m_data.reset(const_cast<ThemeData*>(normal_default_theme), noDeleter);
            break;
        case DarkTheme:
            // TODO
            break;
    }
    return theme;
}

QRgb Theme::normalColor(Theme::Style style) const
{
    Q_ASSERT(m_data);
    return m_data.get()[style].normalColor;
}

QRgb Theme::backgroundColor(Theme::Style style) const
{
    Q_ASSERT(m_data);
    return m_data.get()[style].backgroundColor;
}

bool Theme::isBold(Theme::Style style) const
{
    Q_ASSERT(m_data);
    return m_data.get()[style].bold;
}

bool Theme::isItalic(Theme::Style style) const
{
    Q_ASSERT(m_data);
    return m_data.get()[style].italic;
}

bool Theme::isUnderline(Theme::Style style) const
{
    Q_ASSERT(m_data);
    return m_data.get()[style].underline;
}

bool Theme::isStrikeThrough(Theme::Style style) const
{
    Q_ASSERT(m_data);
    return m_data.get()[style].strikeThrough;
}
