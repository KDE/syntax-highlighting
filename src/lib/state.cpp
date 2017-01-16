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

#include "state.h"
#include "state_p.h"

#include "context_p.h"

#include <QStringList>

using namespace KSyntaxHighlighting;

StateData::StateData() :
    m_defData(nullptr)
{
}

StateData* StateData::get(State &state)
{
    state.d.detach();
    return state.d.data();
}

bool StateData::isEmpty() const
{
    Q_ASSERT(m_contextStack.size() == m_captureStack.size());
    return m_contextStack.isEmpty();
}

void StateData::clear()
{
    m_contextStack.clear();
    m_captureStack.clear();
}

int StateData::size() const
{
    Q_ASSERT(m_contextStack.size() == m_captureStack.size());
    return m_contextStack.size();
}

void StateData::push(Context *context, const QStringList &captures)
{
    Q_ASSERT(context);
    m_contextStack.push(context);
    m_captureStack.push(captures);
    Q_ASSERT(m_contextStack.size() == m_captureStack.size());
}

void StateData::pop()
{
    m_contextStack.pop();
    m_captureStack.pop();
}

Context* StateData::topContext() const
{
    Q_ASSERT(!isEmpty());
    return m_contextStack.top();
}

QStringList StateData::topCaptures() const
{
    Q_ASSERT(!isEmpty());
    return m_captureStack.top();
}

State::State() :
    d(new StateData)
{
}

State::State(const State &other) :
    d(other.d)
{
}

State::~State()
{
}

State& State::operator=(const State &other)
{
    d = other.d;
    return *this;
}

bool State::operator==(const State &other) const
{
    return d->m_contextStack == other.d->m_contextStack && d->m_captureStack == other.d->m_captureStack && d->m_defData == other.d->m_defData;
}

bool State::operator!=(const State &other) const
{
    return !(*this == other);
}

bool State::indentationBasedFoldingEnabled() const
{
    if (d->m_contextStack.isEmpty())
        return false;
    return d->m_contextStack.top()->indentationBasedFoldingEnabled();
}
