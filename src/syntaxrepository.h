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

#ifndef KATESYNTAX_SYNTAXREPOSITORY_H
#define KATESYNTAX_SYNTAXREPOSITORY_H

#include "katesyntax_export.h"

#include <QVector>

namespace KateSyntax {

class SyntaxDefinition;

class KATESYNTAX_EXPORT SyntaxRepository
{
public:
    SyntaxRepository();
    ~SyntaxRepository();

    SyntaxDefinition definitionForName(const QString &defName) const;
    SyntaxDefinition definitionForFileName(const QString &fileName) const;
    QVector<SyntaxDefinition> definitions() const;

private:
    void load();

    QVector<SyntaxDefinition> m_defs;
};

}

#endif // KATESYNTAX_SYNTAXREPOSITORY_H
