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

#include "foldingregion.h"

using namespace KSyntaxHighlighting;

static_assert(sizeof(FoldingRegion) == 2, "FoldingRegion is size-sensitive to frequent use in KTextEditor!");

FoldingRegion::FoldingRegion() :
    m_type(None),
    m_id(0)
{
}

FoldingRegion::FoldingRegion(Type type, quint16 id) :
    m_type(type),
    m_id(id)
{
}

bool FoldingRegion::operator==(const FoldingRegion &other) const
{
    return m_id == other.m_id && m_type == other.m_type;
}

bool FoldingRegion::isValid() const
{
    return type() != None;
}

quint16 FoldingRegion::id() const
{
    return m_id;
}

FoldingRegion::Type FoldingRegion::type() const
{
    return static_cast<FoldingRegion::Type>(m_type);
}
