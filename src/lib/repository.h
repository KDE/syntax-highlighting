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
    Repository();
    ~Repository();

    Definition definitionForName(const QString &defName) const;
    Definition definitionForFileName(const QString &fileName) const;
    QVector<Definition> definitions() const;

private:
    Q_DISABLE_COPY(Repository)

    std::unique_ptr<RepositoryPrivate> d;
};

}

#endif // SYNTAXHIGHLIGHTING_REPOSITORY_H
