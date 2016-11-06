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

#include "matchresult_p.h"

using namespace KSyntaxHighlighting;

MatchResult::MatchResult(int offset, const QStringList &captures) :
    m_captures(captures),
    m_offset(offset),
    m_skipOffset(0)
{
}

MatchResult::MatchResult(int offset, int skipOffset) :
    m_offset(offset),
    m_skipOffset(skipOffset)
{
}

int MatchResult::offset() const
{
    return m_offset;
}

int MatchResult::skipOffset() const
{
    return m_skipOffset;
}

QStringList MatchResult::captures() const
{
    return m_captures;
}
