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

#ifndef KATESYNTAX_ABSTRACTHIGHLIGHTERM_H
#define KATESYNTAX_ABSTRACTHIGHLIGHTERM_H

#include "katesyntax_export.h"

#include <QStack>

class QString;

namespace KateSyntax {

class Context;
class ContextSwitch;
class SyntaxDefinition;

class KATESYNTAX_EXPORT AbstractHighlighter
{
public:
    AbstractHighlighter();
    virtual ~AbstractHighlighter();

    void setDefinition(SyntaxDefinition *def);

    void highlightLine(const QString &text);

    virtual void setFormat(int offset, int length, const QString &format);

private:
    void switchContext(const ContextSwitch &contextSwitch);

    SyntaxDefinition *m_definition;
    QStack<Context*> m_context;
};
}

#endif // KATESYNTAX_ABSTRACTHIGHLIGHTERM_H
