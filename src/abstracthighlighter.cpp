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
    if (!m_definition)
        return;
    m_context.push(m_definition->initialContext());
}

void AbstractHighlighter::highlightLine(const QString& text)
{
    qDebug() << text;
    if (!m_definition || text.isEmpty()) {
        setFormat(0, text.size(), QStringLiteral("dsNormal"));
        return;
    }

    Q_ASSERT(!m_context.isEmpty());
    int offset = 0, beginOffset = 0;
    auto currentFormat = m_context.top()->attribute();
    bool lineContinuation = false;

    do {
        bool isLookAhead = false;
        int newOffset = 0;
        QString newFormat;
        foreach (auto rule, m_context.top()->rules()) {
            newOffset = rule->match(text, offset);
            if (newOffset <= offset)
                continue;

            if (rule->isLookAhead()) {
                switchContext(rule->context(), rule->syntaxDefinition());
                isLookAhead = true;
                break;
            }

            newFormat = rule->attribute().isEmpty() ? m_context.top()->attribute() : rule->attribute();
            switchContext(rule->context(), rule->syntaxDefinition());
            if (newOffset == text.size() && std::dynamic_pointer_cast<LineContinue>(rule))
                lineContinuation = true;
            break;
        }
        if (isLookAhead)
            continue;

        if (newOffset <= offset) { // no matching rule
            if (m_context.top()->fallthrough()) {
                switchContext(m_context.top()->fallthroughContext(), m_context.top()->syntaxDefinition());
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

    while (m_context.top()->lineEndContext() != QLatin1String("#stay") && !lineContinuation)
        switchContext(m_context.top()->lineEndContext(), m_context.top()->syntaxDefinition());
}

void AbstractHighlighter::switchContext(const QString& contextName, SyntaxDefinition *def)
{
    Q_ASSERT(def);
    Q_ASSERT(!m_context.isEmpty());

    if (contextName == QLatin1String("#stay"))
        return;

    if (contextName == QLatin1String("#pop")) {
        m_context.pop();
    } else {
        auto newContext = def->contextByName(contextName);
        if (!newContext)
            qWarning() << "cannot find context" << contextName;
        else
            m_context.push(newContext);
    }

    Q_ASSERT(!m_context.isEmpty());
    Q_ASSERT(m_context.top());
}

void AbstractHighlighter::setFormat(int offset, int length, const QString& format)
{
    qDebug() << offset << length << format;
}
