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

#ifndef SYNTAXHIGHLIGHTING_STATE_H
#define SYNTAXHIGHLIGHTING_STATE_H

#include "kf5syntaxhighlighting_export.h"

#include <QExplicitlySharedDataPointer>

namespace SyntaxHighlighting {

class StateData;

/** Opaque handle to the state of the higlighting engine.
 *  This needs to be fed into AbstractHighlighter for every line of text
 *  and allows concrete highlighter implementations to store state per
 *  line for fast re-highlighting of specific lines (e.g. during editing).
 */
class KF5SYNTAXHIGHLIGHTING_EXPORT State
{
public:
    /** Creates an initial state, ie. what should be used for the first line
     *  in a document.
     */
    State();
    State(const State &other);
    ~State();
    State& operator=(const State &rhs);

private:
    friend class StateData;
    QExplicitlySharedDataPointer<StateData> d;
};

}

#endif // SYNTAXHIGHLIGHTING_STATE_H
