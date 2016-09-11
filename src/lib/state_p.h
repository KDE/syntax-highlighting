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

#ifndef SYNTAXHIGHLIGHTING_STATE_P_H
#define SYNTAXHIGHLIGHTING_STATE_P_H

#include <QSharedData>
#include <QStack>

QT_BEGIN_NAMESPACE
class QStringList;
QT_END_NAMESPACE

namespace SyntaxHighlighting
{

class Context;

class StateData : public QSharedData
{
public:
    static StateData* get(State &state);

    bool isEmpty() const;
    int size() const;
    void push(Context *context, const QStringList &captures);
    void pop();
    Context* topContext() const;
    QStringList topCaptures() const;

private:
    QStack<Context*> m_contextStack;
    QStack<QStringList> m_captureStack;
};

}

#endif
