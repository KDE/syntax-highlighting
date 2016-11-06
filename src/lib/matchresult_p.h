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

#ifndef KSYNTAXHIGHLIGHTING_MATCHRESULT_P_H
#define KSYNTAXHIGHLIGHTING_MATCHRESULT_P_H

#include <QStringList>

namespace KSyntaxHighlighting {

class MatchResult
{
public:
    MatchResult(int offset, const QStringList &captures = QStringList()); // implicit
    explicit MatchResult(int offset, int skipOffset);

    int offset() const;
    int skipOffset() const;
    QStringList captures() const;

private:
    QStringList m_captures;
    int m_offset;
    int m_skipOffset;
};
}

#endif // KSYNTAXHIGHLIGHTING_MATCHRESULT_P_H
