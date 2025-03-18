/*
    SPDX-FileCopyrightText: 2016 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: MIT
*/

#ifndef KSYNTAXHIGHLIGHTING_REPOSITORY_H
#define KSYNTAXHIGHLIGHTING_REPOSITORY_H

#include "ksyntaxhighlighting_export.h"

#include <QList>
#include <QObject>
#include <QtGlobal>

#include <memory>

QT_BEGIN_NAMESPACE
class QString;
class QPalette;
QT_END_NAMESPACE

/*!
 * \namespace KSyntaxHighlighting
 *
 * \brief Syntax highlighting engine for Kate syntax definitions.
 *
 * In order to access the syntax highlighting Definition files, use the
 * class Repository.
 */
namespace KSyntaxHighlighting
{
class Definition;
class RepositoryPrivate;
class Theme;

/*!
 * \class KSyntaxHighlighting::Repository
 * \inheaderfile KSyntaxHighlighting/Repository
 * \inmodule KSyntaxHighlighting
 *
 * \brief Syntax highlighting repository.
 *
 * The Repository gives access to all syntax Definitions available on the
 * system, typically described in *.xml files. The Definition files are read
 * from the resource that is compiled into the executable, and from the file
 * system. If a Definition exists in the resource and on the file system,
 * then the one from the file system is chosen.
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
 * All highlighting Definition and Theme files are compiled into the shared
 * KSyntaxHighlighting library by using the Qt resource system. Loading
 * additional files from disk is supported as well.
 *
 * Loading syntax Definition files works as follows:
 * \list
 * \li First, all syntax highlighting files are loaded that are located in
 *    QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, QStringLiteral("org.kde.syntax-highlighting/syntax"), QStandardPaths::LocateDirectory);
 *    Under Unix, this uses $XDG_DATA_HOME and $XDG_DATA_DIRS, which could
 *    map to $HOME/.local5/share/org.kde.syntax-highlighting/syntax and
 *    /usr/share/org.kde.syntax-highlighting/syntax.
 *
 * \li Then, all files compiled into the library through resources are loaded.
 *    The internal resource path is ":/org.kde.syntax-highlighting/syntax".
 *    This path should never be touched by other applications.
 *
 * \li Then, all custom files compiled into resources are loaded.
 *    The resource path is ":/org.kde.syntax-highlighting/syntax-addons".
 *    This path can be used by other libraries/applications to bundle specialized definitions.
 *    Per default this path isn't used by the framework itself.
 *
 * \li Finally, the search path can be extended by calling addCustomSearchPath().
 *    A custom search path can either be a path on disk or again a path to
 *    a Qt resource.
 * \endlist
 *
 * Similarly, loading Theme files works as follows:
 * \list
 * \li First, all Theme files are loaded that are located in
 *    QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, QStringLiteral("org.kde.syntax-highlighting/themes"), QStandardPaths::LocateDirectory);
 *    Under Unix, this uses $XDG_DATA_HOME and $XDG_DATA_DIRS, which could
 *    map to $HOME/.local5/share/org.kde.syntax-highlighting/themes and
 *    /usr/share/org.kde.syntax-highlighting/themes.
 *
 * \li Then, all files compiled into the library through resources are loaded.
 *    The internal resource path is ":/org.kde.syntax-highlighting/themes".
 *    This path should never be touched by other applications.
 *
 * \li Then, all custom files compiled into resources are loaded.
 *    The resource path is ":/org.kde.syntax-highlighting/themes-addons".
 *    This path can be used by other libraries/applications to bundle specialized themes.
 *    Per default this path isn't used by the framework itself.
 *
 * \li Finally, all Theme%s located in the paths added addCustomSearchPath()
 *    are loaded.
 * \endlist
 *
 * \note Whenever a Definition or a Theme exists twice, the variant with
 *       higher version is used.
 *
 * \note The QStandardPaths lookup can be disabled by compiling the framework with the -DNO_STANDARD_PATHS define.
 *
 * \sa Definition, Theme, AbstractHighlighter
 * \since 5.28
 */
class KSYNTAXHIGHLIGHTING_EXPORT Repository : public QObject
{
    Q_OBJECT

    /*!
     * \property KSyntaxHighlighting::Repository::definitions
     */
    Q_PROPERTY(QList<KSyntaxHighlighting::Definition> definitions READ definitions NOTIFY reloaded)

    /*!
     * \property KSyntaxHighlighting::Repository::themes
     */
    Q_PROPERTY(QList<KSyntaxHighlighting::Theme> themes READ themes NOTIFY reloaded)

public:
    /*!
     * Create a new syntax definition repository.
     * This will read the meta data information of all available syntax
     * definition, which is a moderately expensive operation, it's therefore
     * recommended to keep a single instance of Repository around as long
     * as you need highlighting in your application.
     */
    Repository();

    ~Repository();

    /*!
     * Returns the Definition named \a defName.
     *
     * If no Definition is found, Definition::isValid() of the returned instance
     * returns false.
     *
     * \note The search is Qt::CaseInsensitive using untranslated names or
     *       alternative names. For instance, the cpp.xml definition file sets
     *       its name to C++ with an alternative name of CPP. Therefore, the
     *       strings "C++", "c++", "CPP" and "cpp" will all return a valid
     *       Definition file.
     */
    Q_INVOKABLE KSyntaxHighlighting::Definition definitionForName(const QString &defName) const;

    /*!
     * Returns the best matching Definition for the file named \a fileName.
     * The match is performed based on the \e extensions and mimetype of
     * the definition files. If multiple matches are found, the one with the
     * highest priority is returned.
     *
     * If no match is found, Definition::isValid() of the returned instance
     * returns false.
     */
    Q_INVOKABLE KSyntaxHighlighting::Definition definitionForFileName(const QString &fileName) const;

    /*!
     * Returns all Definition%s for the file named \a fileName sorted by priority.
     * The match is performed based on the \e extensions and mimetype of
     * the definition files.
     *
     * \since 5.56
     */
    Q_INVOKABLE QList<KSyntaxHighlighting::Definition> definitionsForFileName(const QString &fileName) const;

    /*!
     * Returns the best matching Definition to the type named \a mimeType
     *
     * If no match is found, Definition::isValid() of the returned instance
     * returns false.
     *
     * \since 5.50
     */
    Q_INVOKABLE KSyntaxHighlighting::Definition definitionForMimeType(const QString &mimeType) const;

    /*!
     * Returns all Definition%s to the type named \a mimeType sorted by priority
     *
     * \since 5.56
     */
    Q_INVOKABLE QList<KSyntaxHighlighting::Definition> definitionsForMimeType(const QString &mimeType) const;

    /*!
     * Returns all available Definition%s.
     * Definition%ss are ordered by translated section and translated names,
     * for consistent displaying.
     */
    Q_INVOKABLE QList<KSyntaxHighlighting::Definition> definitions() const;

    /*!
     * Returns all available color themes.
     * The returned list should never be empty.
     */
    Q_INVOKABLE QList<KSyntaxHighlighting::Theme> themes() const;

    /*!
     * Returns the theme called \a themeName.
     * If the requested theme cannot be found, the retunred Theme is invalid,
     * see Theme::isValid().
     */
    Q_INVOKABLE KSyntaxHighlighting::Theme theme(const QString &themeName) const;

    /*!
     * \enum ResolveState
     *
     * Built-in default theme types.
     * \sa defaultTheme()
     * 
     * \value LightTheme
     * \value DarkTheme
     */
    enum DefaultTheme {
        LightTheme,
        DarkTheme
    };
    Q_ENUM(DefaultTheme)

    /*!
     * Returns a default theme instance of the given type.
     * The returned Theme is guaranteed to be a valid theme.
     * \since 5.79
     */
    Q_INVOKABLE KSyntaxHighlighting::Theme defaultTheme(DefaultTheme t = LightTheme) const;

    /*!
     * Returns the best matching theme for the given palette
     * \since 5.79
     **/
    Theme themeForPalette(const QPalette &palette) const;

    /*!
     * Reloads the repository.
     * This is a moderately expensive operations and should thus only be
     * triggered when the installed syntax definition files changed.
     */
    void reload();

    /*!
     * Add a custom search path to the repository.
     * This path will be searched in addition to the usual locations for syntax
     * and theme definition files. Both locations on disk as well as Qt
     * resource paths are supported.
     *
     * \note Internally, the two sub-folders \a path/syntax as well as
     *       \a path/themes are searched for additional Definition%s and
     *       Theme%s. Do not append syntax or themes to \a path
     *       yourself.
     *
     * \note Calling this triggers a reload() of the repository.
     *
     * \since 5.39
     */
    void addCustomSearchPath(const QString &path);

    /*!
     * Returns the list of custom search paths added to the repository.
     * By default, this list is empty.
     *
     * \sa addCustomSearchPath()
     * \since 5.39
     */
    QList<QString> customSearchPaths() const;

Q_SIGNALS:
    /*!
     * This signal is emitted before the reload is started.
     * \since 6.0
     */
    void aboutToReload();

    /*!
     * This signal is emitted when the reload is finished.
     * \since 6.0
     */
    void reloaded();

private:
    Q_DISABLE_COPY(Repository)
    friend class RepositoryPrivate;
    std::unique_ptr<RepositoryPrivate> d;
};

}

#endif // KSYNTAXHIGHLIGHTING_REPOSITORY_H
