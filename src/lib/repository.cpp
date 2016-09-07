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

#include "repository.h"
#include "syntaxdefinition.h"

#include <QDebug>
#include <QDirIterator>
#include <QFileInfo>
#include <QStandardPaths>

using namespace SyntaxHighlighting;

namespace SyntaxHighlighting {
class RepositoryPrivate
{
public:
    void load(Repository *repo);
    void loadFolder(Repository *repo, const QString &path);
    void addDefinition(const SyntaxDefinition &def);

    QHash<QString, SyntaxDefinition> m_defs;
};
}

Repository::Repository() :
    d(new RepositoryPrivate)
{
    d->load(this);
}

Repository::~Repository()
{
}

SyntaxDefinition Repository::definitionForName(const QString& defName) const
{
    auto def = d->m_defs.value(defName);
    if (def.isValid())
        def.load();
    return def;
}

SyntaxDefinition Repository::definitionForFileName(const QString& fileName) const
{
    QFileInfo fi(fileName);
    const auto ext = fi.suffix();

    for (auto it = d->m_defs.constBegin(); it != d->m_defs.constEnd(); ++it) {
        auto def = it.value();
        if (def.extensions().contains(ext)) {
            def.load();
            return def;
        }
    }

    return SyntaxDefinition();
}

QVector<SyntaxDefinition> Repository::definitions() const
{
    QVector<SyntaxDefinition> defs;
    defs.reserve(d->m_defs.size());
    for (auto it = d->m_defs.constBegin(); it != d->m_defs.constEnd(); ++it)
        defs.push_back(it.value());
    return defs;
}

void RepositoryPrivate::load(Repository *repo)
{
    foreach (const auto &dir, QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation)) {
        loadFolder(repo, dir + QStringLiteral("/KateSyntax"));
    }
    loadFolder(repo, QStringLiteral(":/syntaxhighlighting/syntax"));
}

void RepositoryPrivate::loadFolder(Repository *repo, const QString &path)
{
    QDirIterator it(path);
    while (it.hasNext()) {
        SyntaxDefinition def;
        def.setRepository(repo);
        if (def.loadMetaData(it.next()))
            addDefinition(def);
    }
}

void RepositoryPrivate::addDefinition(const SyntaxDefinition &def)
{
    const auto it = m_defs.constFind(def.name());
    if (it == m_defs.constEnd()) {
        m_defs.insert(def.name(), def);
        return;
    }

    if (it.value().version() >= def.version())
        return;
    m_defs.insert(def.name(), def);
}
