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

#ifndef KSYNTAXHIGHLIGHTING_CONTEXT_P_H
#define KSYNTAXHIGHLIGHTING_CONTEXT_P_H

#include "rule_p.h"
#include "contextswitch_p.h"
#include "definition.h"
#include "definitionref_p.h"

#include <QString>
#include <QVector>

class QXmlStreamReader;

namespace KSyntaxHighlighting {

class Context
{
public:
    Context();
    ~Context();

    Definition definition() const;
    void setDefinition(const DefinitionRef &def);

    QString name() const;
    QString attribute() const;
    ContextSwitch lineEndContext() const;
    ContextSwitch lineEmptyContext() const;

    bool fallthrough() const;
    ContextSwitch fallthroughContext() const;

    /**
     * Returns @c true, when indentationBasedFolding is enabled for the
     * associated Definition and when "noIndentationBasedFolding" is NOT set.
     */
    bool indentationBasedFoldingEnabled() const;

    QVector<Rule::Ptr> rules() const;

    /** Attempts to find the format named @p name in the
     *  enclosing definition, or the source definition of any
     *  included rule.
     */
    Format formatByName(const QString &name) const;

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

    DefinitionRef m_def;
    QString m_name;
    QString m_attribute;
    ContextSwitch m_lineEndContext;
    ContextSwitch m_lineEmptyContext;
    ContextSwitch m_fallthroughContext;

    QVector<Rule::Ptr> m_rules;

    ResolveState m_resolveState;
    bool m_fallthrough;
    bool m_noIndentationBasedFolding;
};
}

#endif // KSYNTAXHIGHLIGHTING_CONTEXT_P_H
