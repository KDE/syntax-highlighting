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

#include "syntaxrepository.h"
#include "syntaxdefinition.h"

#include <QDebug>
#include <QDirIterator>
#include <QFileInfo>

using namespace KateSyntax;

SyntaxRepository::SyntaxRepository()
{
    load();
}

SyntaxRepository::~SyntaxRepository()
{
}

SyntaxDefinition SyntaxRepository::definitionForName(const QString& defName) const
{
   foreach (auto def, m_defs) {
        if (def.name() == defName) {
            def.load();
            return def;
        }
    }
    return SyntaxDefinition();
}

SyntaxDefinition SyntaxRepository::definitionForFileName(const QString& fileName) const
{
    QFileInfo fi(fileName);
    const auto ext = fi.suffix();

    foreach (auto def, m_defs) {
        if (def.extensions().contains(ext)) {
            def.load();
            return def;
        }
    }

    return SyntaxDefinition();
}

QVector<SyntaxDefinition> SyntaxRepository::definitions() const
{
    return m_defs;
}

void SyntaxRepository::load()
{
    QDirIterator it(QStringLiteral(":/kate-syntax"));
    while (it.hasNext()) {
        SyntaxDefinition def;
        def.setSyntaxRepository(this);
        if (def.loadMetaData(it.next())) {
            m_defs.push_back(def);
        }
    }
}
