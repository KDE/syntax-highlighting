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
#include "format.h"
#include "rule.h"
#include "state.h"
#include "state_p.h"
#include "theme.h"

#include <QDebug>

using namespace SyntaxHighlighting;

namespace SyntaxHighlighting {

class AbstractHighlighterPrivate
{
public:
    AbstractHighlighterPrivate();
    bool switchContext(StateData* data, const ContextSwitch &contextSwitch, const QStringList &captures);

    Definition m_definition;
    Theme m_theme;
};
}

AbstractHighlighterPrivate::AbstractHighlighterPrivate() :
    m_theme(Theme::defaultTheme())
{
}

/**
 * Returns the index of the first non-space character. If the line is empty,
 * or only contains white spaces, -1 is returned.
 */
static inline int firstNonSpaceChar(const QString & text)
{
    for (int i = 0; i < text.length(); ++i) {
        if (!text[i].isSpace()) {
            return i;
        }
    }
    return -1;
}

AbstractHighlighter::AbstractHighlighter() :
    d(new AbstractHighlighterPrivate)
{
}

AbstractHighlighter::~AbstractHighlighter()
{
}

Definition AbstractHighlighter::definition() const
{
    return d->m_definition;
}

void AbstractHighlighter::setDefinition(const Definition &def)
{
    d->m_definition = def;
    if (d->m_definition.isValid())
        d->m_definition.load();
}

Theme AbstractHighlighter::theme() const
{
    return d->m_theme;
}

void AbstractHighlighter::setTheme(const Theme &theme)
{
    d->m_theme = theme;
}

State AbstractHighlighter::highlightLine(const QString& text, const State &state)
{
    if (!d->m_definition.isValid()) {
        setFormat(0, text.size(), Format());
        return State();
    }

    auto newState = state;
    auto stateData = StateData::get(newState);
    if (stateData->isEmpty())
        stateData->push(d->m_definition.initialContext(), QStringList());

    if (text.isEmpty()) {
        while (!stateData->topContext()->lineEmptyContext().isStay())
            d->switchContext(stateData, stateData->topContext()->lineEmptyContext(), QStringList());
        setFormat(0, 0, Format());
        return newState;
    }

    Q_ASSERT(!stateData->isEmpty());
    int firstNonSpace = firstNonSpaceChar(text);
    if (firstNonSpace < 0) {
        firstNonSpace = text.size();
    }
    int offset = 0, beginOffset = 0;
    auto currentLookupContext = stateData->topContext();
    auto currentFormat = currentLookupContext->attribute();
    bool lineContinuation = false;
    QHash<Rule*, int> skipOffsets;

    do {
        bool isLookAhead = false;
        int newOffset = 0;
        QString newFormat;
        auto newLookupContext = currentLookupContext;
        foreach (auto rule, stateData->topContext()->rules()) {
            if (skipOffsets.value(rule.get()) > offset)
                continue;

            // filter out rules that only match for leading whitespace
            if (rule->firstNonSpace() && (offset > firstNonSpace)) {
                continue;
            }

            // filter out rules that require a specific column
            if ((rule->requiredColumn() >= 0) && (rule->requiredColumn() != offset)) {
                continue;
            }

            const auto newResult = rule->match(text, offset, stateData->topCaptures());
            newOffset = newResult.offset();
            if (newResult.skipOffset() > newOffset)
                skipOffsets.insert(rule.get(), newResult.skipOffset());
            if (newOffset <= offset)
                continue;

            if (rule->isLookAhead()) {
                Q_ASSERT(!rule->context().isStay());
                d->switchContext(stateData, rule->context(), newResult.captures());
                isLookAhead = true;
                break;
            }

            newLookupContext = stateData->topContext();
            d->switchContext(stateData, rule->context(), newResult.captures());
            newFormat = rule->attribute().isEmpty() ? stateData->topContext()->attribute() : rule->attribute();
            if (newOffset == text.size() && std::dynamic_pointer_cast<LineContinue>(rule))
                lineContinuation = true;
            break;
        }
        if (isLookAhead)
            continue;

        if (newOffset <= offset) { // no matching rule
            if (stateData->topContext()->fallthrough()) {
                d->switchContext(stateData, stateData->topContext()->fallthroughContext(), QStringList());
                continue;
            }

            newOffset = offset + 1;
            newLookupContext = stateData->topContext();
            newFormat = newLookupContext->attribute();
        }

        if (newFormat != currentFormat /*|| currentLookupDef != newLookupDef*/) {
            if (offset > 0)
                setFormat(beginOffset, offset - beginOffset, currentLookupContext->formatByName(currentFormat));
            beginOffset = offset;
            currentFormat = newFormat;
            currentLookupContext = newLookupContext;
        }
        Q_ASSERT(newOffset > offset);
        offset = newOffset;

    } while (offset < text.size());

    if (beginOffset < offset)
        setFormat(beginOffset, text.size() - beginOffset, currentLookupContext->formatByName(currentFormat));

    while (!stateData->topContext()->lineEndContext().isStay() && !lineContinuation) {
        if (!d->switchContext(stateData, stateData->topContext()->lineEndContext(), QStringList()))
            break;
    }

    return newState;
}

bool AbstractHighlighterPrivate::switchContext(StateData *data, const ContextSwitch &contextSwitch, const QStringList &captures)
{
    for (int i = 0; i < contextSwitch.popCount(); ++i) {
        // never pop the initial context
        if (data->size() == 1)
            return false;
        data->pop();
    }

    if (contextSwitch.context())
        data->push(contextSwitch.context(), captures);

    return true;
}
