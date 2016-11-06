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

#ifndef KSYNTAXHIGHLIGHTING_XML_P_H
#define KSYNTAXHIGHLIGHTING_XML_P_H

#include <QString>

namespace KSyntaxHighlighting {
/** Utilities for XML parsing. */
namespace Xml {

/** Parse a xs:boolean attribute. */
inline bool attrToBool(const QStringRef &str)
{
    return str == QLatin1String("1") || str.compare(QLatin1String("true"), Qt::CaseInsensitive) == 0;
}

}
}

#endif
