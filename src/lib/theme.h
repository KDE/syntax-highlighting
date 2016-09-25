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
#include <QExplicitlySharedDataPointer>
#include <qobjectdefs.h>
#include <QTypeInfo>

namespace SyntaxHighlighting {

class ThemeData;
class RepositoryPrivate;

/**
 * Color theme definition used for highlighting.
 *
 * @section theme_intro Introduction
 *
 * The Theme provides a full color theme for painting the highlighted text.
 * One Theme is defined either as a *.theme file on disk, or as a file compiled
 * into the SyntaxHighlighting library by using Qt's resource system. Each
 * Theme has a unique name(), including a translatedName() if put into the UI.
 * Themes shipped by default are typically read-only, see isReadOnly().
 *
 * A Theme defines two sets of colors:
 * - Text colors, including foreground and background colors, colors for
 *   selected text, and properties such as bold and italic. These colors are
 *   used e.g. by the SyntaxHighlighter.
 * - Editor colors, including a background color for the entire editor widget,
 *   the line number color, code folding colors, etc.
 *
 * @section theme_text_colors Text Colors and the Class Format
 *
 * The text colors are used for syntax highlighting.
 * // TODO: elaborate more and explain relation to Format class
 *
 * @section theme_editor_colors Editor Colors
 *
 * If you want to use the SyntaxHighlighting framework to write your own text
 * editor, you also need to paint the background of the editing widget. In
 * addition, the editor may support showing line numbers, a folding bar, a
 * highlight for the current text line, and similar features. All these colors
 * are defined in terms of the "editor colors".
 *
 * @section theme_access Accessing a Theme
 *
 * All available Theme%s are accessed through the Repository. These themes are
 * typically valid themes. If you create a Theme on your own, isValid() will
 * return @e false, and all colors provided by this Theme are in fact invalid
 * and therefore unusable.
 *
 * @see Format
 */
class KF5SYNTAXHIGHLIGHTING_EXPORT Theme
{
    Q_GADGET
public:
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
    Q_ENUM(TextStyle)

    /** Editor color roles, used to paint line numbers, editor background etc. */
    enum EditorColorRole {
        BackgroundColor = 0,
        TextSelection,
        CurrentLine,
        SearchHighlight,
        ReplaceHighlight,
        BracketMatching,
        TabMarker,
        SpellChecking,
        IndentationLine,
        IconBorder,
        CodeFolding,
        LineNumbers,
        CurrentLineNumber,
        WordWrapMarker,
        ModifiedLines,
        SavedLines,
        Separator,
        MarkBookmark,
        MarkBreakpointActive,
        MarkBreakpointReached,
        MarkBreakpointDisabled,
        MarkExecution,
        MarkWarning,
        MarkError,
        TemplateBackground,
        TemplatePlaceholder,
        TemplateFocusedPlaceholder,
        TemplateReadOnlyPlaceholder
    };
    Q_ENUM(EditorColorRole)

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

    /**
     * Returns whether the given style should be shown in bold.
     */
    bool isBold(TextStyle style) const;

    /**
     * Returns whether the given style should be shown in italic.
     */
    bool isItalic(TextStyle style) const;

    /**
     * Returns whether the given style should be shown underlined.
     */
    bool isUnderline(TextStyle style) const;

    /**
     * Returns whether the given style should be shown striked through.
     */
    bool isStrikeThrough(TextStyle style) const;

public:
    /**
     * Returns the editor color for the requested @p role.
     */
    QRgb editorColor(EditorColorRole role) const;

private:
    /**
     * Constructor taking a shared ThemeData instance.
     */
    Theme(ThemeData* data);
    friend class RepositoryPrivate;
    friend class ThemeData;

private:
    /**
     * Shared data holder.
     */
    QExplicitlySharedDataPointer<ThemeData> m_data;
};

}

Q_DECLARE_TYPEINFO(SyntaxHighlighting::Theme, Q_MOVABLE_TYPE);

#endif // SYNTAXHIGHLIGHTING_THEME_H
