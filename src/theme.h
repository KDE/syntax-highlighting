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

#ifndef KATESYNTAX_THEME_H
#define KATESYNTAX_THEME_H

#include "katesyntax_export.h"

#include <QColor>

#include <memory>

namespace KateSyntax {

struct ThemeData;

class KATESYNTAX_EXPORT Theme
{
public:
    ~Theme();

    enum Style {
        Normal,
        Keyword,
        Function,
        Variable,
        ControlFlow,
        Operator,
        BuiltIn,
        Extension,
        Preprocessor,
        Attribute,
        Char,
        SpecialChar,
        String,
        VerbatimString,
        SpecialString,
        Import,
        DataType,
        DecVal,
        BaseN,
        Float,
        Constant,
        Comment,
        Documentation,
        Annotation,
        CommentVar,
        RegionMarker,
        Information,
        Warning,
        Alert,
        Error,
        Others
    };

    QRgb normalColor(Style style) const;
    QRgb backgroundColor(Style style) const;

    bool isBold(Style style) const;
    bool isItalic(Style style) const;
    bool isUnderline(Style style) const;
    bool isStrikeThrough(Style style) const;

    enum DefaultTheme {
        NormalTheme,
        DarkTheme
    };
    static Theme defaultTheme(DefaultTheme t = NormalTheme);

private:
    Theme();
    std::shared_ptr<ThemeData> m_data;
};

}

#endif // KATESYNTAX_THEME_H
