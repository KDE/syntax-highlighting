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

#ifndef KATESYNTAX_CONTEXT_H
#define KATESYNTAX_CONTEXT_H

#include "rule.h"
#include "contextswitch.h"

#include <QString>
#include <QVector>

class QXmlStreamReader;

namespace KateSyntax {

class SyntaxDefinition;

class Context
{
public:
    Context();
    ~Context();

    SyntaxDefinition* syntaxDefinition() const;
    void setSyntaxDefinition(SyntaxDefinition *def);

    QString name() const;
    QString attribute() const;
    ContextSwitch lineEndContext() const;

    bool fallthrough() const;
    ContextSwitch fallthroughContext() const;

    QVector<Rule::Ptr> rules() const;

    void load(QXmlStreamReader &reader);
    void resolveContexts();
    void resolveIncludes();

private:
    Q_DISABLE_COPY(Context)

    enum ResolveState {
        Unknown,
        Unresolved,
        Resolving,
        Resolved
    };
    ResolveState resolveState();

    SyntaxDefinition *m_def;
    QString m_name;
    QString m_attribute;
    ContextSwitch m_lineEndContext;
    ContextSwitch m_fallthroughContext;

    QVector<Rule::Ptr> m_rules;

    ResolveState m_resolveState;
    bool m_fallthrough;
};
}

#endif // KATESYNTAX_CONTEXT_H
