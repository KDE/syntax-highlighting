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
#include "syntaxdefinition.h"
#include "theme.h"

#include <QStack>
#include <QStringList>

class QString;

namespace SyntaxHighlighting {

class Context;
class ContextSwitch;
class Format;

class KF5SYNTAXHIGHLIGHTING_EXPORT AbstractHighlighter
{
public:
    AbstractHighlighter();
    virtual ~AbstractHighlighter();

    SyntaxDefinition definition() const;
    void setDefinition(const SyntaxDefinition &def);

    Theme theme() const;
    void setTheme(const Theme &theme);

protected:
    void highlightLine(const QString &text);
    void reset();
    virtual void setFormat(int offset, int length, const Format &format);

private:
    void switchContext(const ContextSwitch &contextSwitch, const QStringList &captures = QStringList());

    SyntaxDefinition m_definition;
    QStack<Context*> m_context;
    QStack<QStringList> m_captureStack;
    Theme m_theme;
};
}

#endif // SYNTAXHIGHLIGHTING_ABSTRACTHIGHLIGHTERM_H
