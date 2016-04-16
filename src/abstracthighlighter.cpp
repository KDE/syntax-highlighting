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
    int offset = 0, prevOffset = 0;
    do {
        bool foundMatch = false;
        foreach (auto rule, m_context.top()->rules()) {
            auto newOffset = rule->match(text, offset);
            if (newOffset <= offset)
                continue;

            if (prevOffset < offset)
                setFormat(prevOffset, offset-prevOffset, m_context.top()->attribute());

            setFormat(offset, newOffset - offset, rule->attribute().isEmpty() ? m_context.top()->attribute() : rule->attribute());
            offset = newOffset;
            prevOffset = offset;
            switchContext(rule->context());
            foundMatch = true;
            break;
        }
        if (!foundMatch)
            ++offset;
    } while (offset < text.size());

    if (prevOffset < offset)
        setFormat(prevOffset, text.size() - prevOffset, m_context.top()->attribute());

    switchContext(m_context.top()->lineEndContext());
}

void AbstractHighlighter::switchContext(const QString& contextName)
{
    Q_ASSERT(m_definition);
    Q_ASSERT(!m_context.isEmpty());

    if (contextName == QLatin1String("#stay"))
        return;

    if (contextName == QLatin1String("#pop")) {
        m_context.pop();
    } else {
        auto newContext = m_definition->contextByName(contextName);
        if (!newContext)
            qWarning() << "cannot find context" << contextName;
        else
            m_context.push(newContext);
    }

    Q_ASSERT(!m_context.isEmpty());
}

void AbstractHighlighter::setFormat(int offset, int length, const QString& format)
{
    qDebug() << offset << length << format;
}
