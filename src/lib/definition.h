/*
    SPDX-FileCopyrightText: 2016 Volker Krause <vkrause@kde.org>
    SPDX-FileCopyrightText: 2020 Jonathan Poelen <jonathan.poelen@gmail.com>

    SPDX-License-Identifier: MIT
*/

#ifndef KSYNTAXHIGHLIGHTING_DEFINITION_H
#define KSYNTAXHIGHLIGHTING_DEFINITION_H

#include "ksyntaxhighlighting_export.h"

#include <QList>
#include <QPair>
#include <QString>
#include <memory>
#include <qobjectdefs.h>

namespace KSyntaxHighlighting
{
class Context;
class Format;
class KeywordList;

class DefinitionData;

/*!
 * Defines the insert position when commenting code.
 *
 * \since 5.50
 * \sa Definition::singleLineCommentPosition()
 *
 * \value StartOfLine
 * \value AfterWhitespace
 */
enum class CommentPosition {
    StartOfLine = 0,
    AfterWhitespace
};

/*!
 * \class KSyntaxHighlighting::Definition
 * \inheaderfile KSyntaxHighlighting/Definition
 * \inmodule KSyntaxHighlighting
 *
 * Represents a syntax definition.
 *
 * A Definition is the short term for a syntax highlighting definition. It
 * typically is defined in terms of an XML syntax highlighting file, containing
 * all information about a particular syntax highlighting. This includes the
 * highlighting of keywords, information about code folding regions, and
 * indentation preferences.
 *
 * Each Definition contains a non-translated unique name() and a section().
 * In addition, for putting this information e.g. into menus, the functions
 * translatedName() and translatedSection() are provided. However, if isHidden()
 * returns \c true, the Definition should not be visible in the UI. The location
 * of the Definition can be obtained through filePath(), which either is the
 * location on disk or a path to a compiled-in Qt resource.
 *
 * The supported files of a Definition are defined by the list of extensions(),
 * and additionally by the list of mimeTypes(). Note, that extensions() returns
 * wildcards that need to be matched against the filename of the file that
 * requires highlighting. If multiple Definition%s match the file, then the one
 * with higher priority() wins.
 *
 * Advanced text editors such as Kate require additional information from a
 * Definition. For instance, foldingEnabled() defines whether a Definition has
 * code folding regions that can be shown in a code folding pane. Or
 * singleLineCommentMarker() and multiLineCommentMarker() provide comment
 * markers that can be used for commenting/uncommenting code. Similarly,
 * formats() returns a list of Format items defined by this Definition (which
 * equal the itemDatas of a highlighting definition file). includedDefinitions()
 * returns a list of all included Definition%s referenced by this Definition via
 * the rule IncludeRules, which is useful for displaying all Format items for
 * color configuration in the user interface.
 *
 * \sa Repository
 * \since 5.28
 */
class KSYNTAXHIGHLIGHTING_EXPORT Definition
{
    Q_GADGET

    /*!
     * \property KSyntaxHighlighting::Definition::name
     */
    Q_PROPERTY(QString name READ name)

    /*!
     * \property KSyntaxHighlighting::Definition::translatedName
     */
    Q_PROPERTY(QString translatedName READ translatedName)

    /*!
     * \property KSyntaxHighlighting::Definition::section
     */
    Q_PROPERTY(QString section READ section)

    /*!
     * \property KSyntaxHighlighting::Definition::translatedSection
     */
    Q_PROPERTY(QString translatedSection READ translatedSection)

    /*!
     * \property KSyntaxHighlighting::Definition::author
     */
    Q_PROPERTY(QString author READ author)

    /*!
     * \property KSyntaxHighlighting::Definition::license
     */
    Q_PROPERTY(QString license READ license)
public:
    /*!
     * Default constructor, creating an empty (invalid) Definition instance.
     * isValid() for this instance returns \c false.
     *
     * Use the Repository instead to obtain valid instances.
     */
    Definition();

    /*!
     * Move constructor.
     * This definition takes the Definition data from \a other.
     *
     * \note \a other may only be assigned to or destroyed afterwards.
     * \since 5.86
     */
    Definition(Definition &&other) noexcept;

    /*!
     * Copy constructor.
     *
     * Both this definition as well as \a other share the Definition data.
     */
    Definition(const Definition &other);

    ~Definition();

    /*!
     * Move assignment operator.
     *
     * This definition takes the Definition data from \a other.
     *
     * \note \a other may only be assigned to or destroyed afterwards.
     * \since 5.86
     */
    Definition &operator=(Definition &&other) noexcept;

    /*!
     * Copy assignment operator.
     *
     * Both this definition as well as \a rhs share the Definition data.
     */
    Definition &operator=(const Definition &rhs);

    /*!
     * Checks two definitions for equality.
     */
    bool operator==(const Definition &other) const;

    /*!
     * Checks two definitions for inequality.
     */
    bool operator!=(const Definition &other) const;

    /*!
     * Checks whether this object refers to a valid syntax definition.
     */
    bool isValid() const;

    /*!
     * Returns the full path to the definition XML file containing
     * the syntax definition. Note that this can be a path to QRC content.
     */
    QString filePath() const;

    /*! Name of the syntax.
     *  Used for internal references, prefer translatedName() for display.
     */
    QString name() const;

    /*!
     * Alternate names the syntax can be referred to by.
     *
     * \since 6.1
     */
    QStringList alternativeNames() const;

    /*!
     * Translated name for display.
     */
    QString translatedName() const;

    /*!
     * The group this syntax definition belongs to.
     * For display, consider translatedSection().
     */
    QString section() const;

    /*!
     * Translated group name for display.
     */
    QString translatedSection() const;

    /*!
     * Mime types associated with this syntax definition.
     */
    QList<QString> mimeTypes() const;

    /*!
     * File extensions associated with this syntax definition.
     * The returned list contains wildcards.
     */
    QList<QString> extensions() const;

    /*!
     * Returns the definition version.
     */
    int version() const;

    /*!
     * Returns the definition priority.
     * A Definition with higher priority wins over Definitions with lower priorities.
     */
    int priority() const;

    /*!
     * Returns \c true if this is an internal definition that should not be
     * displayed to the user.
     */
    bool isHidden() const;

    /*!
     * Generalized language style, used for indentation.
     */
    QString style() const;

    /*!
     * Indentation style to be used for this syntax.
     */
    QString indenter() const;

    /*!
     * Name and email of the author of this syntax definition.
     */
    QString author() const;

    /*!
     * License of this syntax definition.
     */
    QString license() const;

    /*!
     * Returns whether the character \a c is a word delimiter.
     * A delimiter defines whether a characters is a word boundary. Internally,
     * delimiters are used for matching keyword lists. As example, typically the
     * dot '.' is a word delimiter. However, if you have a keyword in a keyword
     * list that contains a dot, you have to add the dot to the
     * weakDeliminator attribute of the general section in your
     * highlighting definition. Similarly, sometimes additional delimiters are
     * required, which can be specified in additionalDeliminator.
     *
     * Checking whether a character is a delimiter is useful for instance if
     * text is selected with double click. Typically, the whole word should be
     * selected in this case. Similarly to the example above, the dot '.'
     * usually acts as word delimiter. However, using this function you can
     * implement text selection in such a way that keyword lists are correctly
     * selected.
     *
     * \note By default, the list of delimiters contains the following
     *       characters: \\t !%&()*+,-./:;<=>?[\\]^{|}~
     *
     * \since 5.50
     * \sa isWordWrapDelimiter()
     */
    bool isWordDelimiter(QChar c) const;

    /*!
     * Returns whether it is safe to break a line at before the character \c.
     * This is useful when wrapping a line e.g. by applying static word wrap.
     *
     * As example, consider the LaTeX code
     * \code
     * \command1\command2
     * \endcode
     * Applying static word wrap could lead to the following code:
     * \code
     * \command1\
     * command2
     * \endcode
     * command2 without a leading backslash is invalid in LaTeX. If '\\' is set
     * as word wrap delimiter, isWordWrapDelimiter('\\') then returns true,
     * meaning that it is safe to break the line before \c. The resulting code
     * then would be
     * \code
     * \command1
     * \command2
     * \endcode
     *
     * \note By default, the word wrap delimiters are equal to the word
     *       delimiters in isWordDelimiter().
     *
     * \since 5.50
     * \sa isWordDelimiter()
     */
    bool isWordWrapDelimiter(QChar c) const;

    /*!
     * Returns whether the highlighting supports code folding.
     * Code folding is supported either if the highlighting defines code folding
     * regions or if indentationBasedFoldingEnabled() returns \c true.
     * \since 5.50
     * \sa indentationBasedFoldingEnabled()
     */
    bool foldingEnabled() const;

    /*!
     * Returns whether indentation-based folding is enabled.
     * An example for indentation-based folding is Python.
     * When indentation-based folding is enabled, make sure to also check
     * foldingIgnoreList() for lines that should be treated as empty.
     *
     * \sa foldingIgnoreList(), State::indentationBasedFoldingEnabled()
     */
    bool indentationBasedFoldingEnabled() const;

    /*!
     * If indentationBasedFoldingEnabled() returns \c true, this function returns
     * a list of regular expressions that represent empty lines. That is, all
     * lines matching entirely one of the regular expressions should be treated
     * as empty lines when calculating the indentation-based folding ranges.
     *
     * \note This list is only of relevance, if indentationBasedFoldingEnabled()
     *       returns \c true.
     *
     * \sa indentationBasedFoldingEnabled()
     */
    QStringList foldingIgnoreList() const;

    /*!
     * Returns the section names of keywords.
     * \since 5.49
     * \sa keywordList()
     */
    QStringList keywordLists() const;

    /*!
     * Returns the list of keywords for the keyword list \a name.
     * \since 5.49
     * \sa keywordLists(), setKeywordList()
     */
    QStringList keywordList(const QString &name) const;

    /*!
     * Set the contents of the keyword list \a name to \a content.
     * Only existing keywordLists() can be changed. For non-existent keyword lists,
     * false is returned.
     *
     * Whenever you change a keyword list, make sure to trigger a rehighlight of
     * your documents. In case you are using QSyntaxHighlighter via SyntaxHighlighter,
     * this can be done by calling SyntaxHighlighter::rehighlight().
     *
     * \note In general, changing keyword lists via setKeywordList() is discouraged,
     *       since if a keyword list name in the syntax highlighting definition
     *       file changes, the call setKeywordList() may suddenly fail.
     *
     * \sa keywordList(), keywordLists()
     * \since 5.62
     */
    bool setKeywordList(const QString &name, const QStringList &content);

    /*!
     * Returns a list of all Format items used by this definition.
     * The order of the Format items equals the order of the itemDatas in the xml file.
     * \since 5.49
     */
    QList<Format> formats() const;

    /*!
     * Returns a list of Definitions that are referenced with the IncludeRules rule.
     * The returned list does not include this Definition. In case no other
     * Definitions are referenced via IncludeRules, the returned list is empty.
     *
     * \since 5.49
     */
    QList<Definition> includedDefinitions() const;

    /*!
     * Returns the marker that starts a single line comment.
     * For instance, in C++ the single line comment marker is "//".
     * \since 5.50
     * \sa singleLineCommentPosition();
     */
    QString singleLineCommentMarker() const;

    /*!
     * Returns the insert position of the comment marker for sinle line
     * comments.
     * \since 5.50
     * \sa singleLineCommentMarker();
     */
    CommentPosition singleLineCommentPosition() const;

    /*!
     * Returns the markers that start and end multiline comments.
     * For instance, in XML this is defined as "<!--" and "-->".
     * \since 5.50
     */
    QPair<QString, QString> multiLineCommentMarker() const;

    /*!
     * Returns a list of character/string mapping that can be used for spell
     * checking. This is useful for instance when spell checking LaTeX, where
     * the string \"{A} represents the character Ä.
     * \since 5.50
     */
    QList<QPair<QChar, QString>> characterEncodings() const;

private:
    friend class DefinitionData;
    KSYNTAXHIGHLIGHTING_NO_EXPORT explicit Definition(const DefinitionData &defData);
    std::shared_ptr<DefinitionData> d;
};

}

QT_BEGIN_NAMESPACE
Q_DECLARE_TYPEINFO(KSyntaxHighlighting::Definition, Q_RELOCATABLE_TYPE);
QT_END_NAMESPACE

#endif
