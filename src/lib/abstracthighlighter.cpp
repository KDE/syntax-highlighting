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
#include "abstracthighlighter_p.h"
#include "context_p.h"
#include "definition_p.h"
#include "foldingregion.h"
#include "format.h"
#include "repository.h"
#include "rule_p.h"
#include "state.h"
#include "state_p.h"
#include "ksyntaxhighlighting_logging.h"
#include "theme.h"

#include <QDebug>

using namespace KSyntaxHighlighting;

AbstractHighlighterPrivate::AbstractHighlighterPrivate()
{
}

AbstractHighlighterPrivate::~AbstractHighlighterPrivate()
{
}

void AbstractHighlighterPrivate::ensureDefinitionLoaded()
{
    auto defData = DefinitionData::get(m_definition);
    if (Q_UNLIKELY(!m_definition.isValid() && defData->repo && !m_definition.name().isEmpty())) {
        qCDebug(Log) << "Definition became invalid, trying re-lookup.";
        m_definition = defData->repo->definitionForName(m_definition.name());
        defData = DefinitionData::get(m_definition);
    }

    if (Q_UNLIKELY(!defData->repo && !defData->name.isEmpty()))
        qCCritical(Log) << "Repository got deleted while a highlighter is still active!";

    if (m_definition.isValid())
        defData->load();
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
    d_ptr(new AbstractHighlighterPrivate)
{
}

AbstractHighlighter::AbstractHighlighter(AbstractHighlighterPrivate *dd) :
    d_ptr(dd)
{
}

AbstractHighlighter::~AbstractHighlighter()
{
    delete d_ptr;
}

Definition AbstractHighlighter::definition() const
{
    return d_ptr->m_definition;
}

void AbstractHighlighter::setDefinition(const Definition &def)
{
    Q_D(AbstractHighlighter);
    d->m_definition = def;
}

Theme AbstractHighlighter::theme() const
{
    Q_D(const AbstractHighlighter);
    return d->m_theme;
}

void AbstractHighlighter::setTheme(const Theme &theme)
{
    Q_D(AbstractHighlighter);
    d->m_theme = theme;
}

State AbstractHighlighter::highlightLine(const QString& text, const State &state)
{
    Q_D(AbstractHighlighter);

    // verify definition, deal with no highlighting being enabled
    d->ensureDefinitionLoaded();
    if (!d->m_definition.isValid()) {
        applyFormat(0, text.size(), Format());
        return State();
    }

    // verify/initialize state
    auto defData = DefinitionData::get(d->m_definition);
    auto newState = state;
    auto stateData = StateData::get(newState);
    if (stateData->m_defData && defData != stateData->m_defData) {
        qCDebug(Log) << "Got invalid state, resetting.";
        stateData->clear();
    }
    if (stateData->isEmpty()) {
        stateData->push(defData->initialContext(), QStringList());
        stateData->m_defData = defData;
    }

    // process empty lines
    if (text.isEmpty()) {
        while (!stateData->topContext()->lineEmptyContext().isStay())
            d->switchContext(stateData, stateData->topContext()->lineEmptyContext(), QStringList());
        applyFormat(0, 0, Format());
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
        foreach (const auto &rule, stateData->topContext()->rules()) {
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

            // apply folding
            if (rule->endRegion().isValid())
                applyFolding(offset, newOffset - offset, rule->endRegion());
            if (rule->beginRegion().isValid())
                applyFolding(offset, newOffset - offset, rule->beginRegion());

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
                applyFormat(beginOffset, offset - beginOffset, currentLookupContext->formatByName(currentFormat));
            beginOffset = offset;
            currentFormat = newFormat;
            currentLookupContext = newLookupContext;
        }
        Q_ASSERT(newOffset > offset);
        offset = newOffset;

    } while (offset < text.size());

    if (beginOffset < offset)
        applyFormat(beginOffset, text.size() - beginOffset, currentLookupContext->formatByName(currentFormat));

    while (!stateData->topContext()->lineEndContext().isStay() && !lineContinuation) {
        if (!d->switchContext(stateData, stateData->topContext()->lineEndContext(), QStringList()))
            break;
    }

    return newState;
}

bool AbstractHighlighterPrivate::switchContext(StateData *data, const ContextSwitch &contextSwitch, const QStringList &captures)
{
    for (int i = 0; i < contextSwitch.popCount(); ++i) {
        // don't pop the last context if we can't push one
        if (data->size() == 1 && !contextSwitch.context())
            return false;
        if (data->size() == 0)
            break;
        data->pop();
    }

    if (contextSwitch.context())
        data->push(contextSwitch.context(), captures);

    Q_ASSERT(!data->isEmpty());
    return true;
}

void AbstractHighlighter::applyFolding(int offset, int length, FoldingRegion region)
{
    Q_UNUSED(offset);
    Q_UNUSED(length);
    Q_UNUSED(region);
}
