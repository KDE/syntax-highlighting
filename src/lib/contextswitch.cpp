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

#include "contextswitch_p.h"
#include "definition.h"
#include "definition_p.h"
#include "repository.h"
#include "ksyntaxhighlighting_logging.h"

#include <QDebug>

using namespace KSyntaxHighlighting;

ContextSwitch::ContextSwitch() :
    m_context(nullptr),
    m_popCount(0)
{
}

ContextSwitch::~ContextSwitch()
{
}

bool ContextSwitch::isStay() const
{
    return m_popCount == 0 && !m_context && m_contextName.isEmpty() && m_defName.isEmpty();
}

int ContextSwitch::popCount() const
{
    return m_popCount;
}

Context* ContextSwitch::context() const
{
    return m_context;
}

void ContextSwitch::parse(const QStringRef& contextInstr)
{
    if (contextInstr.isEmpty() || contextInstr == QLatin1String("#stay"))
        return;

    if (contextInstr.startsWith(QLatin1String("#pop!"))) {
        ++m_popCount;
        m_contextName = contextInstr.mid(5).toString();
        return;
    }

    if (contextInstr.startsWith(QLatin1String("#pop"))) {
        ++m_popCount;
        parse(contextInstr.mid(4));
        return;
    }

    const auto idx = contextInstr.indexOf(QLatin1String("##"));
    if (idx >= 0) {
        m_contextName = contextInstr.left(idx).toString();
        m_defName = contextInstr.mid(idx + 2).toString();
    } else {
        m_contextName = contextInstr.toString();
    }
}

void ContextSwitch::resolve(const Definition &def)
{
    auto d = def;
    if (!m_defName.isEmpty()) {
        d = DefinitionData::get(def)->repo->definitionForName(m_defName);
        auto data = DefinitionData::get(d);
        data->load();
        if (m_contextName.isEmpty())
            m_context = data->initialContext();
    }

    if (!m_contextName.isEmpty()) {
        m_context = DefinitionData::get(d)->contextByName(m_contextName);
        if (!m_context)
            qCWarning(Log) << "cannot find context" << m_contextName << "in" << def.name();
    }
}
