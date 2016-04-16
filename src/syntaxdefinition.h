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

#ifndef KATESYNTAX_SYNTAXDEFINITION_H
#define KATESYNTAX_SYNTAXDEFINITION_H

#include "katesyntax_export.h"

#include "keywordlist.h"

#include <QHash>
#include <QString>
#include <QVector>

class QXmlStreamReader;

namespace KateSyntax {

class Context;
class KeywordList;
class Rule;

class KATESYNTAX_EXPORT SyntaxDefinition
{
public:
    SyntaxDefinition();
    ~SyntaxDefinition();

    QString name() const;
    QVector<QString> extensions() const;

    Context* initialContext() const;
    Context* contextByName(const QString &name) const;

    bool load(const QString &definitionFileName);
    void assemble();

private:
    Q_DISABLE_COPY(SyntaxDefinition)

    void loadHighlighting(QXmlStreamReader &reader);
    void loadContexts(QXmlStreamReader &reader);
    void assembleKeywordList(Rule *rule);

    QHash<QString, KeywordList> m_keywordLists;
    QVector<Context*> m_contexts;

    QString m_name;
    QString m_section;
    QVector<QString> m_extensions;
    QVector<QString> m_mimetypes;
    bool m_hidden;
};

}

#endif
