/*
    Copyright (C) 2016 Volker Krause <vkrause@kde.org>

    Permission is hereby granted, free of charge, to any person obtaining
    a copy of this software and associated documentation files (the
    "Software"), to deal in the Software without restriction, including
    without limitation the rights to use, copy, modify, merge, publish,
    distribute, sublicense, and/or sell copies of the Software, and to
    permit persons to whom the Software is furnished to do so, subject to
    the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "state.h"
#include "state_p.h"

#include "context_p.h"

#include <QStringList>

using namespace KSyntaxHighlighting;

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
