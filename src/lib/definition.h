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

#ifndef SYNTAXHIGHLIGHTING_DEFINITION_H
#define SYNTAXHIGHLIGHTING_DEFINITION_H

#include "kf5syntaxhighlighting_export.h"

#include <memory>

class QChar;
class QString;
template <typename T> class QVector;
class QXmlStreamReader;

namespace SyntaxHighlighting {

class Context;
class Format;
class KeywordList;
class Repository;

class DefinitionPrivate;

/** Represents syntax definition. */
class KF5SYNTAXHIGHLIGHTING_EXPORT Definition
{
public:
    Definition();
    Definition(const Definition &other);
    ~Definition();

    Definition& operator=(const Definition &rhs);

    /** Checks whether this object refers to a valid syntax definition. */
    bool isValid() const;

    /** Name of the syntax.
     *  Used for internal references, prefer translatedName() for display.
     */
    QString name() const;
    /** Translated name for display. */
    QString translatedName() const;
    /** The group this syntax definition belongs to.
     *  For display, consider translatedSection().
     */
    QString section() const;
    /** Translated group name for display. */
    QString translatedSection() const;
    /** Mime types associated with this syntax definition. */
    QVector<QString> mimeTypes() const;
    /** File extensions associated with this syntax definition. */
    QVector<QString> extensions() const;
    /** Returns the definition version. */
    float version() const;
    /** Returns the definition priority. */
    int priority() const;
    /** Returns @c true if this is an internal definition that should not be
     * displayed to the user.
     */
    bool isHidden() const;
    /** TODO ??? */
    QString style() const;
    /** Indention style to be used for this syntax. */
    QString indenter() const;
    /** Name and email of the author of this syntax definition. */
    QString author() const;
    /** License of this syntax definition. */
    QString license() const;

    // FIXME everything below is internal -> do not export this
    Repository* repository() const;
    void setRepository(Repository *repo);

    Context* initialContext() const;
    Context* contextByName(const QString &name) const;

    KeywordList keywordList(const QString &name) const;
    bool isDelimiter(QChar c) const;

    Format formatByName(const QString &name) const;

    bool load();
    bool loadMetaData(const QString &definitionFileName);

private:
    friend class DefinitionRef;
    std::shared_ptr<DefinitionPrivate> d;
};

}

#endif
