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

#ifndef KSYNTAXHIGHLIGHTING_CONTEXTSWITCH_P_H
#define KSYNTAXHIGHLIGHTING_CONTEXTSWITCH_P_H

#include <QString>

namespace KSyntaxHighlighting {

class Context;
class Definition;

class ContextSwitch
{
public:
    ContextSwitch();
    ~ContextSwitch();

    bool isStay() const;

    int popCount() const;
    Context* context() const;

    void parse(const QStringRef &contextInstr);
    void resolve(const Definition &def);

private:
    QString m_defName;
    QString m_contextName;
    Context *m_context;
    int m_popCount;
};
}

#endif // KSYNTAXHIGHLIGHTING_CONTEXTSWITCH_P_H
