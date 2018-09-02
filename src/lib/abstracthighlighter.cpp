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
    const DefinitionRef currentDefRef(d->m_definition);
    if (!stateData->isEmpty() && (stateData->m_defRef != currentDefRef)) {
        qCDebug(Log) << "Got invalid state, resetting.";
        stateData->clear();
    }
    if (stateData->isEmpty()) {
        stateData->push(defData->initialContext(), QStringList());
        stateData->m_defRef = currentDefRef;
    }

    // process empty lines
    if (text.isEmpty()) {
        while (!stateData->topContext()->lineEmptyContext().isStay())
            d->switchContext(stateData, stateData->topContext()->lineEmptyContext(), QStringList());
        auto context = stateData->topContext();
        applyFormat(0, 0, context->attributeFormat());
        return newState;
    }

    Q_ASSERT(!stateData->isEmpty());
    int firstNonSpace = firstNonSpaceChar(text);
    if (firstNonSpace < 0) {
        firstNonSpace = text.size();
    }
    int offset = 0, beginOffset = 0;
    auto currentFormat = stateData->topContext()->attributeFormat();
    bool lineContinuation = false;
    QHash<Rule*, int> skipOffsets;

    do {
        bool isLookAhead = false;
        int newOffset = 0;
        Format newFormat;
        foreach (const auto &rule, stateData->topContext()->rules()) {
            /**
             * shall we skip application of this rule? two cases:
             *   - rule can't match at all => currentSkipOffset < 0
             *   - rule will only match for some higher offset => currentSkipOffset > offset
             */
            const auto currentSkipOffset = skipOffsets.value(rule.get());
            if (currentSkipOffset < 0 || currentSkipOffset > offset)
                continue;

            // filter out rules that only match for leading whitespace
            if (rule->firstNonSpace() && (offset > firstNonSpace)) {
                continue;
            }

            // filter out rules that require a specific column
            if ((rule->requiredColumn() >= 0) && (rule->requiredColumn() != offset)) {
                continue;
            }

            const auto newResult = rule->doMatch(text, offset, stateData->topCaptures());
            newOffset = newResult.offset();

            /**
             * update skip offset if new one rules out any later match or is larger than current one
             */
            if (newResult.skipOffset() < 0 || newResult.skipOffset() > currentSkipOffset)
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

            d->switchContext(stateData, rule->context(), newResult.captures());
            newFormat = rule->attributeFormat().isValid() ? rule->attributeFormat() : stateData->topContext()->attributeFormat();
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
            newFormat = stateData->topContext()->attributeFormat();
        }

        /**
         * on format change, apply the last one and switch to new one
         */
        if (newFormat.id() != currentFormat.id()) {
            if (offset > 0)
                applyFormat(beginOffset, offset - beginOffset, currentFormat);
            beginOffset = offset;
            currentFormat = newFormat;
        }

        Q_ASSERT(newOffset > offset);
        offset = newOffset;

    } while (offset < text.size());

    if (beginOffset < offset)
        applyFormat(beginOffset, text.size() - beginOffset, currentFormat);

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
