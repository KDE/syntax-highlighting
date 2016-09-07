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
#include "context.h"
#include "rule.h"
#include "format.h"

#include <QDebug>

using namespace SyntaxHighlighting;

AbstractHighlighter::AbstractHighlighter() :
    m_theme(Theme::defaultTheme())
{
}

AbstractHighlighter::~AbstractHighlighter()
{
}

SyntaxDefinition AbstractHighlighter::definition() const
{
    return m_definition;
}

void AbstractHighlighter::setDefinition(const SyntaxDefinition &def)
{
    m_definition = def;
    reset();
}

void AbstractHighlighter::reset()
{
    m_context.clear();
    m_captureStack.clear();
    if (!m_definition.isValid())
        return;
    m_context.push(m_definition.initialContext());
    m_captureStack.push(QStringList());
}

Theme AbstractHighlighter::theme() const
{
    return m_theme;
}

void AbstractHighlighter::setTheme(const Theme &theme)
{
    m_theme = theme;
}

void AbstractHighlighter::highlightLine(const QString& text)
{
    if (!m_definition.isValid()) {
        setFormat(0, text.size(), Format());
        return;
    }

    if (text.isEmpty()) {
        while (!m_context.top()->lineEmptyContext().isStay())
            switchContext(m_context.top()->lineEmptyContext());
        setFormat(0, 0, Format());
        return;
    }

    Q_ASSERT(!m_context.isEmpty());
    Q_ASSERT(!m_captureStack.isEmpty());
    int offset = 0, beginOffset = 0;
    auto currentFormat = m_context.top()->attribute();
    auto currentLookupDef = m_context.top()->syntaxDefinition();
    bool lineContinuation = false;
    QHash<Rule*, int> skipOffsets;

    do {
        bool isLookAhead = false;
        int newOffset = 0;
        QString newFormat;
        auto newLookupDef = currentLookupDef;
        foreach (auto rule, m_context.top()->rules()) {
            if (skipOffsets.value(rule.get()) > offset)
                continue;

            const auto newResult = rule->match(text, offset, m_captureStack.top());
            newOffset = newResult.offset();
            if (newResult.skipOffset() > newOffset)
                skipOffsets.insert(rule.get(), newResult.skipOffset());
            if (newOffset <= offset)
                continue;

            if (rule->isLookAhead()) {
                Q_ASSERT(!rule->context().isStay());
                switchContext(rule->context());
                isLookAhead = true;
                break;
            }

            switchContext(rule->context(), newResult.captures());
            newFormat = rule->attribute().isEmpty() ? m_context.top()->attribute() : rule->attribute();
            newLookupDef = rule->syntaxDefinition();
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
            newLookupDef = m_context.top()->syntaxDefinition();
        }

        if (newFormat != currentFormat /*|| currentLookupDef != newLookupDef*/) {
            if (offset > 0)
                setFormat(beginOffset, offset - beginOffset, currentLookupDef.formatByName(currentFormat));
            beginOffset = offset;
            currentFormat = newFormat;
            currentLookupDef = newLookupDef;
        }
        Q_ASSERT(newOffset > offset);
        offset = newOffset;

    } while (offset < text.size());

    if (beginOffset < offset)
        setFormat(beginOffset, text.size() - beginOffset, currentLookupDef.formatByName(currentFormat));

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

void AbstractHighlighter::setFormat(int offset, int length, const Format& format)
{
    qDebug() << offset << length << format.name();
}
