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

#ifndef SYNTAXHIGHLIGHTING_REPOSITORY_H
#define SYNTAXHIGHLIGHTING_REPOSITORY_H

#include "kf5syntaxhighlighting_export.h"

#include <qglobal.h>
#include <memory>

class QString;
template <typename T> class QVector;

/** @namespce SyntaxHighlighting
 *  Syntax highlighting engine for Kate syntax definitions.
 */
namespace SyntaxHighlighting {

class RepositoryPrivate;
class Definition;

/** Holds all syntax definitions found on the system or compiled in. */
class KF5SYNTAXHIGHLIGHTING_EXPORT Repository
{
public:
    /** Create a new syntax definition repository.
     *  This will read the meta data information of all available syntax
     *  definition, which is a moderately expensive operation, it's therefore
     *  recommended to keep a single instance of Repository around as long
     *  as you need highlighting in your application.
     */
    Repository();
    ~Repository();

    /** Returns the definition named @p defName.
     *  Note: This uses untranslated names.
     */
    Definition definitionForName(const QString &defName) const;
    /** Returns the best matching definition for a file named @p fileName.
     *  This matches the file name against the supported file name patterns
     *  of the definition, if multiple matches are found, the one with the
     *  highest priority is returned.
     */
    Definition definitionForFileName(const QString &fileName) const;
    /** Returns all available definition.
     *  Definitions are ordered by translated section and translated names,
     *  for consistent displaying.
     */
    QVector<Definition> definitions() const;

private:
    Q_DISABLE_COPY(Repository)
    std::unique_ptr<RepositoryPrivate> d;
};

}

#endif // SYNTAXHIGHLIGHTING_REPOSITORY_H
