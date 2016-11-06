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

#ifndef KSYNTAXHIGHLIGHTING_REPOSITORY_H
#define KSYNTAXHIGHLIGHTING_REPOSITORY_H

#include "ksyntaxhighlighting_export.h"

#include <qglobal.h>
#include <memory>

class QString;
template <typename T> class QVector;

/**
 * @namespace KSyntaxHighlighting
 *
 * Syntax highlighting engine for Kate syntax definitions.
 * In order to access the syntax highlighting Definition files, use the
 * class Repository.
 *
 * @see Repository
 */
namespace KSyntaxHighlighting {

class Definition;
class RepositoryPrivate;
class Theme;

/**
 * @brief Syntax highlighting repository.
 *
 * @section repo_intro Introduction
 *
 * The Repository gives access to all syntax Definitions available on the
 * system, typically described in *.xml files. The Definition files are read
 * from the resource that is compiled into the executable, and from the file
 * system. If a Definition exists in the resource and on the file system,
 * then the one from the file system is chosen.
 *
 * @section repo_access Definitions and Themes
 *
 * Typically, only one instance of the Repository is needed. This single
 * instance can be thought of as a singleton you keep alive throughout the
 * lifetime of your application. Then, either call definitionForName() with the
 * given language name (e.g. "QML" or "Java"), or definitionForFileName() to
 * obtain a Definition based on the filename/mimetype of the file. The
 * function definitions() returns a list of all available syntax Definition%s.
 *
 * In addition to Definitions, the Repository also provides a list of Themes.
 * A Theme is defined by a set of default text style colors as well as editor
 * colors. These colors together provide all required colros for drawing all
 * primitives of a text editor. All available Theme%s can be queried through
 * themes(), and a Theme with a specific name is obtained through theme().
 * Additionally, defaultTheme() provides a way to obtain a default theme for
 * either a light or a black color theme.
 *
 * @see Definition, Theme, AbstractHighlighter
 */
class KSYNTAXHIGHLIGHTING_EXPORT Repository
{
public:
    /**
     * Create a new syntax definition repository.
     * This will read the meta data information of all available syntax
     * definition, which is a moderately expensive operation, it's therefore
     * recommended to keep a single instance of Repository around as long
     * as you need highlighting in your application.
     */
    Repository();
    ~Repository();

    /**
     * Returns the Definition named @p defName.
     *
     * If no Definition is found, Definition::isValid() of the returned instance
     * returns false.
     *
     * @note This uses case sensitive, untranslated names. For instance,
     *       the javascript.xml definition file sets its name to @e JavaScript.
     *       Therefore, only the string "JavaScript" will return a valid
     *       Definition file.
     */
    Definition definitionForName(const QString &defName) const;

    /**
     * Returns the best matching Definition for the file named @p fileName.
     * The match is performed based on the \e extensions and @e mimetype of
     * the definition files. If multiple matches are found, the one with the
     * highest priority is returned.
     *
     * If no match is found, Definition::isValid() of the returned instance
     * returns false.
     */
    Definition definitionForFileName(const QString &fileName) const;

    /**
     * Returns all available Definition%s.
     * Definition%ss are ordered by translated section and translated names,
     * for consistent displaying.
     */
    QVector<Definition> definitions() const;

    /**
     * Returns all available color themes.
     * The returned list should never be empty.
     */
    QVector<Theme> themes() const;

    /**
     * Returns the theme called @p themeName.
     * If the requested theme cannot be found, the retunred Theme is invalid,
     * see Theme::isValid().
     */
    Theme theme(const QString &themeName) const;

    /**
     * Built-in default theme types.
     * @see defaultTheme()
     */
    enum DefaultTheme {
        //! Theme with a light background color.
        LightTheme,
        //! Theme with a dark background color.
        DarkTheme
    };

    /**
     * Returns a default theme instance of the given type.
     * The returned Theme is guaranteed to be a valid theme.
     */
    Theme defaultTheme(DefaultTheme t = LightTheme);

    /**
     * Reloads the repository.
     * This is a moderately expensive operations and should thus only be
     * triggered when the installed syntax definition files changed.
     */
    void reload();

private:
    Q_DISABLE_COPY(Repository)
    friend class RepositoryPrivate;
    std::unique_ptr<RepositoryPrivate> d;
};

}

#endif // KSYNTAXHIGHLIGHTING_REPOSITORY_H
