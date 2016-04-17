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

#include "abstracthighlighter.h"
#include "syntaxdefinition.h"
#include "context.h"
#include "rule.h"

#include <QDebug>

using namespace KateSyntax;

AbstractHighlighter::AbstractHighlighter() :
    m_definition(nullptr)
{
}

AbstractHighlighter::~AbstractHighlighter()
{
}

void AbstractHighlighter::setDefinition(SyntaxDefinition* def)
{
    m_definition = def;
    m_context.clear();
    m_captureStack.clear();
    if (!m_definition)
        return;
    m_context.push(m_definition->initialContext());
    m_captureStack.push(QStringList());
}

void AbstractHighlighter::highlightLine(const QString& text)
{
    qDebug() << text;
    if (!m_definition || text.isEmpty()) {
        setFormat(0, text.size(), QStringLiteral("dsNormal"));
        return;
    }

    Q_ASSERT(!m_context.isEmpty());
    Q_ASSERT(!m_captureStack.isEmpty());
    int offset = 0, beginOffset = 0;
    auto currentFormat = m_context.top()->attribute();
    bool lineContinuation = false;

    do {
        bool isLookAhead = false;
        int newOffset = 0;
        QString newFormat;
        foreach (auto rule, m_context.top()->rules()) {
            const auto newResult = rule->match(text, offset, m_captureStack.top());
            newOffset = newResult.offset();
            if (newOffset <= offset)
                continue;

            if (rule->isLookAhead()) {
                switchContext(rule->context());
                isLookAhead = true;
                break;
            }

            newFormat = rule->attribute().isEmpty() ? m_context.top()->attribute() : rule->attribute();
            switchContext(rule->context(), newResult.captures());
            if (newOffset == text.size() && std::dynamic_pointer_cast<LineContinue>(rule))
                lineContinuation = true;
            break;
        }
        if (isLookAhead)
            continue;

        if (newOffset <= offset) { // no matching rule
            if (m_context.top()->fallthrough()) {
                switchContext(m_context.top()->fallthroughContext());
                continue;
            }

            newOffset = offset + 1;
            newFormat = m_context.top()->attribute();
        }

        if (newFormat != currentFormat) {
            if (offset > 0)
                setFormat(beginOffset, offset - beginOffset, currentFormat);
            beginOffset = offset;
            currentFormat = newFormat;
        }
        Q_ASSERT(newOffset > offset);
        offset = newOffset;

    } while (offset < text.size());

    if (beginOffset < offset)
        setFormat(beginOffset, text.size() - beginOffset, currentFormat);

    while (!m_context.top()->lineEndContext().isStay() && !lineContinuation)
        switchContext(m_context.top()->lineEndContext());
}

void AbstractHighlighter::switchContext(const ContextSwitch &contextSwitch, const QStringList &captures)
{
    Q_ASSERT(m_context.size() >= contextSwitch.popCount());
    Q_ASSERT(m_context.size() == m_captureStack.size());

    for (int i = 0; i < contextSwitch.popCount(); ++i) {
        m_context.pop();
        m_captureStack.pop();
    }

    if (contextSwitch.context()) {
        m_context.push(contextSwitch.context());
        m_captureStack.push(captures);
    }

    Q_ASSERT(!m_context.isEmpty());
    Q_ASSERT(m_context.top());
    Q_ASSERT(m_context.size() == m_captureStack.size());
}

void AbstractHighlighter::setFormat(int offset, int length, const QString& format)
{
    qDebug() << offset << length << format;
}
