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

#ifndef SYNTAXHIGHLIGHTING_FOLDINGREGION_H
#define SYNTAXHIGHLIGHTING_FOLDINGREGION_H

#include "kf5syntaxhighlighting_export.h"

#include <QTypeInfo>

namespace SyntaxHighlighting {

/** Represents a begin or end of a folding region. */
class KF5SYNTAXHIGHLIGHTING_EXPORT FoldingRegion
{
public:
    enum Type {
        None,
        Begin,
        End
    };

    /** Constructs an invalid folding region.
     *  To obtain valid instances, see AbstractHighlighter::applyFolding().
     */
    FoldingRegion();

    /** Returns @c true if this is a valid folding region.
     *  That is, if the type is not None.
     */
    bool isValid() const;

    /** Returns a unique identifier for this folding region. */
    quint16 id() const;
    /** Returns whether this is the begin or end of a region. */
    Type type() const;

private:
    friend class Rule;
    FoldingRegion(Type type, quint16 id);

    quint16 m_type : 2;
    quint16 m_id: 14;
};

}

Q_DECLARE_TYPEINFO(SyntaxHighlighting::FoldingRegion, Q_PRIMITIVE_TYPE);

#endif
