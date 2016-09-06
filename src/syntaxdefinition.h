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

#ifndef SYNTAXHIGHLIGHTING_SYNTAXDEFINITION_H
#define SYNTAXHIGHLIGHTING_SYNTAXDEFINITION_H

#include "kf5syntaxhighlighting_export.h"

#include <QExplicitlySharedDataPointer>

class QChar;
class QString;
template <typename T> class QVector;
class QXmlStreamReader;

namespace KateSyntax {

class Context;
class Format;
class KeywordList;
class SyntaxRepository;

class SyntaxDefinitionPrivate;

class KF5SYNTAXHIGHLIGHTING_EXPORT SyntaxDefinition
{
public:
    SyntaxDefinition();
    SyntaxDefinition(const SyntaxDefinition &other);
    ~SyntaxDefinition();

    SyntaxDefinition& operator=(const SyntaxDefinition &rhs);

    bool isValid() const;

    SyntaxRepository* syntaxRepository() const;
    void setSyntaxRepository(SyntaxRepository *repo);

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
    QExplicitlySharedDataPointer<SyntaxDefinitionPrivate> d;
};

}

#endif
