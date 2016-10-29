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

#ifndef SYNTAXHIGHLIGHTING_FORMAT_P_H
#define SYNTAXHIGHLIGHTING_FORMAT_P_H

#include "definitionref_p.h"
#include "textstyledata_p.h"
#include "theme.h"

#include <QString>

namespace SyntaxHighlighting {

class FormatPrivate
{
public:
    FormatPrivate();
    TextStyleData styleOverride(const Theme &theme) const;

    DefinitionRef definition;
    QString name;
    TextStyleData style;
    Theme::TextStyle defaultStyle;
    bool spellCheck;
};

}

#endif
