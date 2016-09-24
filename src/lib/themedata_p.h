/*
    Copyright (C) 2016 Volker Krause <vkrause@kde.org>
    Copyright (C) 2016 Dominik Haumann <dhaumann@kde.org>

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

#ifndef SYNTAXHIGHLIGHTING_THEMEDATA_H
#define SYNTAXHIGHLIGHTING_THEMEDATA_H

#include "theme.h"
#include "textstyledata_p.h"

namespace SyntaxHighlighting {

/**
 * Data container for a Theme.
 */
class ThemeData
{
public:
    /**
     * Default constructor, creating an uninitialized ThemeData instance.
     */
    ThemeData();

    /**
     * Load the Theme data from the file @p filePath.
     * Note, that @p filePath either is a local file, or a qt resource location.
     */
    bool load(const QString &filePath);

    /**
     * Returns the unique name of this Theme.
     */
    QString name() const;

    /**
     * Returns @c true if this Theme is read-only.
     * Typically, themes that are shipped by default are read-only.
     */
    bool isReadOnly() const;

    /**
     * Returns the full path and filename to this Theme.
     * Themes from the Qt resource return the Qt resource path.
     * Themes from disk return the local path.
     *
     * If the theme is invalid (isValid()), an empty string is returned.
     */
    QString filePath() const;

    /**
     * Returns the text color to be used for @p style.
     * @c 0 is returned for styles that do not specify a text color,
     * use the default text color in that case.
     */
    QRgb textColor(Theme::TextStyle style) const;

    /**
     * Returns the text color for selected to be used for @p style.
     * @c 0 is returned for styles that do not specify a selected text color,
     * use the textColor() in that case.
     */
    QRgb selectedTextColor(Theme::TextStyle style) const;

    /**
     * Returns the background color to be used for @p style.
     * @c 0 is returned for styles that do not specify a background color,
     * use the default background color in that case.
     */
    QRgb backgroundColor(Theme::TextStyle style) const;

    /**
     * Returns the background color for selected text to be used for @p style.
     * @c 0 is returned for styles that do not specify a selected background
     * color, use the default backgroundColor() in that case.
     */
    QRgb selectedBackgroundColor(Theme::TextStyle style) const;

    /**
     * Returns whether the given style should be shown in bold.
     */
    bool isBold(Theme::TextStyle style) const;

    /**
     * Returns whether the given style should be shown in italic.
     */
    bool isItalic(Theme::TextStyle style) const;

    /**
     * Returns whether the given style should be shown underlined.
     */
    bool isUnderline(Theme::TextStyle style) const;

    /**
     * Returns whether the given style should be shown striked through.
     */
    bool isStrikeThrough(Theme::TextStyle style) const;

public:
    /**
     * Returns the background color that should be used to fill the text area.
     */
    QRgb areaBackgroundColor() const;

    /**
     * Returns the code folding color. This is used for a possible code folding
     * border as well as highlighting color for the background of the text of
     * of a code folding region.
     */
    QRgb codeFoldingColor() const;

    /**
     * Returns the background color used for bracket matching.
     */
    QRgb brackedMatchingColor() const;

    /**
     * Returns the background color for highlighting the current text line.
     */
    QRgb currentLineColor() const;

    /**
     * Returns the background color of the icon border.
     */
    QRgb iconBorderColor() const;

    /**
     * Returns the color for vertical indentation lines.
     */
    QRgb indentationLineColor() const;

    /**
     * Returns the foreground color for the line numbers.
     */
    QRgb lineNumberColor() const;

    /**
     * Returns the foreground color for the current line number.
     */
    QRgb currentLineNumberColor() const;

private:
    int m_revision;
    QString m_name;
    QString m_author;
    QString m_license;
    QString m_filePath;
    bool m_readOnly;

    //! TextStyles
    TextStyleData m_textStyles[Theme::Others + 1];

    //! Editor area colors
    QRgb m_areaBackgroundColor;
    QRgb m_codeFoldingColor;
    QRgb m_brackedMatchingColor;
    QRgb m_currentLineColor;
    QRgb m_iconBorderColor;
    QRgb m_indentationLineColor;
    QRgb m_lineNumberColor;
    QRgb m_currentLineNumberColor;
};

}

Q_DECLARE_TYPEINFO(SyntaxHighlighting::TextStyleData, Q_MOVABLE_TYPE);

#endif // SYNTAXHIGHLIGHTING_THEMEDATA_H
