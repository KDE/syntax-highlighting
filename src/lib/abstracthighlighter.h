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
class Theme;

/** Abstract base class for highlighters. */
class KF5SYNTAXHIGHLIGHTING_EXPORT AbstractHighlighter
{
public:
    AbstractHighlighter();
    virtual ~AbstractHighlighter();

    /** The syntax definition used for highlighting. */
    Definition definition() const;
    /** Sets the syntax definition used for highlighting. */
    void setDefinition(const Definition &def);

    Theme theme() const;
    void setTheme(const Theme &theme);

protected:
    /** Highlight the given line. Call this from your derived class
     *  where appropriate. This will result in any number of setFormat()
     *  calls as a result.
     */
    void highlightLine(const QString &text);
    void reset();
    /** Reimplement this to apply formats to your output. */
    virtual void setFormat(int offset, int length, const Format &format);

private:
    Q_DISABLE_COPY(AbstractHighlighter)
    std::unique_ptr<AbstractHighlighterPrivate> d;
};
}

#endif // SYNTAXHIGHLIGHTING_ABSTRACTHIGHLIGHTERM_H
