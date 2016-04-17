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
    assemble();
}

SyntaxRepository::~SyntaxRepository()
{
}

SyntaxDefinition* SyntaxRepository::definitionForName(const QString& defName) const
{
   foreach (auto def, m_defs) {
        if (def->name() == defName)
            return def;
    }
    return nullptr;
}

SyntaxDefinition* SyntaxRepository::definitionForFileName(const QString& fileName) const
{
    QFileInfo fi(fileName);
    const auto ext = fi.suffix();

    foreach (auto def, m_defs) {
        if (def->extensions().contains(ext))
            return def;
    }

    return nullptr;
}

void SyntaxRepository::load()
{
    QDirIterator it(QStringLiteral(":/kate-syntax"));
    while (it.hasNext()) {
        auto def = new SyntaxDefinition;
        def->setSyntaxRepository(this);
        if (def->load(it.next())) {
            m_defs.push_back(def);
        }
    }

    qDebug() << "Found" << m_defs.size() << "syntax definitions.";
}

void SyntaxRepository::assemble()
{
    foreach (auto def, m_defs)
        def->assemble();
}
