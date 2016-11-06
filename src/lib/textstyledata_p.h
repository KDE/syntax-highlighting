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

#ifndef KSYNTAXHIGHLIGHTING_TEXTSTYLEDATA_P_H
#define KSYNTAXHIGHLIGHTING_TEXTSTYLEDATA_P_H

#include <QColor>

namespace KSyntaxHighlighting {

class TextStyleData
{
public:
    // Constructor initializing all data.
    TextStyleData()
        : textColor(0x0)
        , backgroundColor(0x0)
        , selectedTextColor(0x0)
        , selectedBackgroundColor(0x0)
        , bold(false)
        , italic(false)
        , underline(false)
        , strikeThrough(false)
        , hasBold(false)
        , hasItalic(false)
        , hasUnderline(false)
        , hasStrikeThrough(false)
    {}

    QRgb textColor;
    QRgb backgroundColor;
    QRgb selectedTextColor;
    QRgb selectedBackgroundColor;
    bool bold :1;
    bool italic :1;
    bool underline :1;
    bool strikeThrough :1;

    bool hasBold :1;
    bool hasItalic :1;
    bool hasUnderline :1;
    bool hasStrikeThrough :1;
};

}

#endif
