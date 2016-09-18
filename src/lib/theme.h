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

/** Default styles that can be referenced from syntax definition XML files. */
enum TextStyle {
    Normal = 0,
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
    /**
     * Default constructor, creating an invalid Theme, see isValid().
     */
    Theme();

    /**
     * Copy constructor, sharing the Theme data with @p copy.
     */
    Theme(const Theme &copy);

    /**
     * Destructor.
     */
    ~Theme();

    /**
     * Assignment operator, sharing the Theme data with @p other.
     */
    Theme &operator=(const Theme &other);

    /**
     * Returns @c true if this is a valid Theme.
     * If the theme is invalid, none of the returned colors are well-defined.
     */
    bool isValid() const;

    /**
     * Returns the unique name of this Theme.
     * @see translatedName()
     */
    QString name() const;

    /**
     * Returns the translated name of this Theme. The translated name can be
     * used in the user interface.
     */
    QString translatedName() const;

    /**
     * Returns @c true if this Theme is read-only.
     * Typically, themes that are shipped by default are read-only.
     */
    bool isReadOnly() const;

    /**
     * Returns the text color to be used for @p style.
     * @c 0 is returned for styles that do not specify a text color,
     * use the default text color in that case.
     */
    QRgb textColor(TextStyle style) const;

    /**
     * Returns the selected text color to be used for @p style.
     * @c 0 is returned for styles that do not specify a selected text color,
     * use the default textColor() in that case.
     */
    QRgb selectedTextColor(TextStyle style) const;

    /**
     * Returns the background color to be used for @p style.
     * @c 0 is returned for styles that do not specify a background color,
     * use the default background color in that case.
     */
    QRgb backgroundColor(TextStyle style) const;

    /**
     * Returns the background color to be used for selected text for @p style.
     * @c 0 is returned for styles that do not specify a background color,
     * use the default backgroundColor() in that case.
     */
    QRgb selectedBackgroundColor(TextStyle style) const;

    /** Returns whether the given style should be shown in bold. */
    bool isBold(TextStyle style) const;
    /** Returns whether the given style should be shown in italic. */
    bool isItalic(TextStyle style) const;
    /** Returns whether the given style should be shown underlined. */
    bool isUnderline(TextStyle style) const;
    /** Returns whether the given style should be shown striked through. */
    bool isStrikeThrough(TextStyle style) const;

public:
    /**
     * Constructor taking a shared ThemeData instance.
     */
    Theme(std::shared_ptr<ThemeData> data);

private:
    /**
     * Shared data holder.
     */
    std::shared_ptr<ThemeData> m_data;
};

}

#endif // SYNTAXHIGHLIGHTING_THEME_H
