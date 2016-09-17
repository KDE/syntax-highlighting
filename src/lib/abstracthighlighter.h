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

#ifndef SYNTAXHIGHLIGHTING_ABSTRACTHIGHLIGHTERM_H
#define SYNTAXHIGHLIGHTING_ABSTRACTHIGHLIGHTERM_H

#include "kf5syntaxhighlighting_export.h"

#include <qglobal.h>

#include <memory>

class QString;

namespace SyntaxHighlighting {

class AbstractHighlighterPrivate;
class Definition;
class Format;
class State;
class Theme;

/** Abstract base class for highlighters. */
class KF5SYNTAXHIGHLIGHTING_EXPORT AbstractHighlighter
{
public:
    virtual ~AbstractHighlighter();

    /** The syntax definition used for highlighting. */
    Definition definition() const;
    /** Sets the syntax definition used for highlighting. */
    void setDefinition(const Definition &def);

    /** Returns the currently selected theme for highlighting. */
    Theme theme() const;
    /** Sets the theme used for highlighting. */
    void setTheme(const Theme &theme);

protected:
    AbstractHighlighter();

    /** Highlight the given line. Call this from your derived class
     *  where appropriate. This will result in any number of setFormat()
     *  calls as a result.
     *  @param text A string containing the text of the line to highlight.
     *  @param state The highlighting state handle returned by the call
     *   to highlightLine() for the previous line.
     *  @returns The state of the highlighing engine after processing the
     *   given line. This needs to passed into highlightLine() in for the
     *   the next line. You can store the state for efficient partial
     *   re-highlighting for example during editing.
     */
    State highlightLine(const QString &text, const State &state);

    /** Reimplement this to apply formats to your output. */
    virtual void setFormat(int offset, int length, const Format &format) = 0;

private:
    Q_DISABLE_COPY(AbstractHighlighter)
    std::unique_ptr<AbstractHighlighterPrivate> d;
};
}

#endif // SYNTAXHIGHLIGHTING_ABSTRACTHIGHLIGHTERM_H
