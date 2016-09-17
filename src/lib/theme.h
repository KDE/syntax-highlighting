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

#ifndef SYNTAXHIGHLIGHTING_THEME_H
#define SYNTAXHIGHLIGHTING_THEME_H

#include "kf5syntaxhighlighting_export.h"

#include <QColor>

#include <memory>

namespace SyntaxHighlighting {

struct ThemeData;

/** Color theme definition used for highlighting.
 *  To determine the final output of highlighting a global theme (represented
 *  by this class) and specific formating instructions of the corresponding
 *  syntax definition (represented by the Format class) are merged.
 *
 *  @see Format
 */
class KF5SYNTAXHIGHLIGHTING_EXPORT Theme
{
public:
    ~Theme();

    /** Default styles that can be referenced from syntax definition XML files. */
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

    /** Returns the text color to be used for @p style.
     *  @c 0 is returned for styles that do not specify a text color,
     *  use the default text color in that case.
     */
    QRgb normalColor(Style style) const;
    /** Returns the background color to be used for @p style.
     *  @c 0 is returned for styles that do not specify a background color,
     *  use the default background color in that case.
     */
    QRgb backgroundColor(Style style) const;

    /** Returns whether the given style should be shown in bold. */
    bool isBold(Style style) const;
    /** Returns whether the given style should be shown in italic. */
    bool isItalic(Style style) const;
    /** Returns whether the given style should be shown underlined. */
    bool isUnderline(Style style) const;
    /** Returns whether the given style should be shown striked through. */
    bool isStrikeThrough(Style style) const;

    /** Built-in default theme types. */
    enum DefaultTheme {
        NormalTheme,
        DarkTheme
    };
    /** Returns a default theme instance of the given type. */
    static Theme defaultTheme(DefaultTheme t = NormalTheme);

private:
    Theme();
    std::shared_ptr<ThemeData> m_data;
};

}

#endif // SYNTAXHIGHLIGHTING_THEME_H
