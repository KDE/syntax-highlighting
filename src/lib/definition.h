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

class KF5SYNTAXHIGHLIGHTING_EXPORT Definition
{
public:
    Definition();
    Definition(const Definition &other);
    ~Definition();

    Definition& operator=(const Definition &rhs);

    bool isValid() const;

    Repository* repository() const;
    void setRepository(Repository *repo);

    QString name() const;
    QVector<QString> extensions() const;
    float version() const;

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
