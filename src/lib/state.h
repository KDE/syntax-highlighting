/*
    SPDX-FileCopyrightText: 2016 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: MIT
*/

#ifndef KSYNTAXHIGHLIGHTING_STATE_H
#define KSYNTAXHIGHLIGHTING_STATE_H

#include "ksyntaxhighlighting_export.h"

#include <QExplicitlySharedDataPointer>
#include <QHash>

namespace KSyntaxHighlighting
{
class State;
class StateData;

KSYNTAXHIGHLIGHTING_EXPORT std::size_t qHash(const State &state, std::size_t seed = 0);

/*!
 * \class KSyntaxHighlighting::State
 * \inheaderfile KSyntaxHighlighting/State
 * \inmodule KSyntaxHighlighting
 *
 * \brief Opaque handle to the state of the highlighting engine.
 *
 * This needs to be fed into AbstractHighlighter for every line of text
 * and allows concrete highlighter implementations to store state per
 * line for fast re-highlighting of specific lines (e.g. during editing).
 *
 * \since 5.28
 */
class KSYNTAXHIGHLIGHTING_EXPORT State
{
public:
    /*! Creates an initial state, ie. what should be used for the first line
     *  in a document.
     */
    State();
    State(State &&other) noexcept;
    State(const State &other) noexcept;
    ~State();
    State &operator=(State &&rhs) noexcept;
    State &operator=(const State &rhs) noexcept;

    /*! Compares two states for equality.
     *  For two equal states and identical text input, AbstractHighlighter
     *  guarantees to produce equal results. This can be used to only
     *  re-highlight as many lines as necessary during editing.
     */
    bool operator==(const State &other) const;
    /*! Compares two states for inequality.
     *  This is the opposite of operator==().
     */
    bool operator!=(const State &other) const;

    /*!
     * Returns whether or not indentation-based folding is enabled in this state.
     * When using a Definition with indentation-based folding, use
     * this method to check if indentation-based folding has been
     * suspended in the current line.
     *
     * \sa Definition::indentationBasedFoldingEnabled()
     */
    bool indentationBasedFoldingEnabled() const;

private:
    friend class StateData;
    friend KSYNTAXHIGHLIGHTING_EXPORT std::size_t qHash(const State &, std::size_t);
    QExplicitlySharedDataPointer<StateData> d;
};
}

QT_BEGIN_NAMESPACE
Q_DECLARE_TYPEINFO(KSyntaxHighlighting::State, Q_RELOCATABLE_TYPE);
QT_END_NAMESPACE

#endif // KSYNTAXHIGHLIGHTING_STATE_H
