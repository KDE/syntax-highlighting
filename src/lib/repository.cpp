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
#include "definition.h"
#include "definition_p.h"
#include "theme.h"
#include "themedata_p.h"
#include "syntaxhighlighting_logging.h"
#include "wildcardmatcher_p.h"

#include <QDebug>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>

using namespace SyntaxHighlighting;

namespace SyntaxHighlighting {
class RepositoryPrivate
{
public:
    void load(Repository *repo);
    void loadFolder(Repository *repo, const QString &path);
    bool loadFolderFromIndex(Repository *repo, const QString &path);
    void addDefinition(const Definition &def);

    QHash<QString, Definition> m_defs;
    QVector<Definition> m_sortedDefs;

    QVector<Theme> m_themes;
};
}

static void initResource()
{
    Q_INIT_RESOURCE(syntax_data);
}

Repository::Repository() :
    d(new RepositoryPrivate)
{
    initResource();
    d->load(this);
}

Repository::~Repository()
{
}

Definition Repository::definitionForName(const QString& defName) const
{
    return d->m_defs.value(defName);
}

Definition Repository::definitionForFileName(const QString& fileName) const
{
    QFileInfo fi(fileName);
    const auto name = fi.fileName();

    QVector<Definition> candidates;
    for (auto it = d->m_defs.constBegin(); it != d->m_defs.constEnd(); ++it) {
        auto def = it.value();
        foreach (const auto &pattern, def.extensions()) {
            if (WildcardMatcher::exactMatch(name, pattern)) {
                candidates.push_back(def);
                break;
            }
        }
    }

    if (candidates.isEmpty())
        return Definition();

    std::partial_sort(candidates.begin(), candidates.begin() + 1, candidates.end(), [](const Definition &lhs, const Definition &rhs) {
        return lhs.priority() > rhs.priority();
    });

    return candidates.at(0);
}

QVector<Definition> Repository::definitions() const
{
    return d->m_sortedDefs;
}

QVector<Theme> Repository::themes() const
{
    return d->m_themes;
}

Theme Repository::theme(const QString &themeName) const
{
    for (auto theme : d->m_themes) {
        if (theme.name() == themeName) {
            return theme;
        }
    }

    return Theme();
}

void RepositoryPrivate::load(Repository *repo)
{
    auto dirs = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, QStringLiteral("org.kde.syntax-highlighting/syntax"), QStandardPaths::LocateDirectory);
    foreach (const auto &dir, dirs)
        loadFolder(repo, dir);
    // backward compatiblity with kate
    dirs = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, QStringLiteral("katepart5/syntax"), QStandardPaths::LocateDirectory);
    foreach (const auto &dir, dirs)
        loadFolder(repo, dir);

    loadFolder(repo, QStringLiteral(":/syntaxhighlighting/syntax"));

    m_sortedDefs.reserve(m_defs.size());
    for (auto it = m_defs.constBegin(); it != m_defs.constEnd(); ++it)
        m_sortedDefs.push_back(it.value());
    std::sort(m_sortedDefs.begin(), m_sortedDefs.end(), [](const Definition &left, const Definition &right) {
        auto comparison = left.translatedSection().compare(right.translatedSection(), Qt::CaseInsensitive);
        if (comparison == 0)
            comparison = left.translatedName().compare(right.translatedName(), Qt::CaseInsensitive);
        return comparison < 0;
    });

    // load themes
    const QString themePath = QStringLiteral(":/syntaxhighlighting/themes");
    QDirIterator it(themePath);
    while (it.hasNext()) {
        std::shared_ptr<ThemeData> themeData = std::make_shared<ThemeData>();
        if (themeData->load(it.next())) {
            m_themes.push_back(Theme(themeData));
        }
    }
}

void RepositoryPrivate::loadFolder(Repository *repo, const QString &path)
{
    if (loadFolderFromIndex(repo, path))
        return;

    QDirIterator it(path);
    while (it.hasNext()) {
        Definition def;
        auto defData = DefinitionData::get(def);
        defData->repo = repo;
        if (defData->loadMetaData(it.next()))
            addDefinition(def);
    }
}

bool RepositoryPrivate::loadFolderFromIndex(Repository *repo, const QString &path)
{
    QFile indexFile(path + QLatin1String("/index.katesyntax"));
    if (!indexFile.open(QFile::ReadOnly))
        return false;

    const auto indexDoc(QJsonDocument::fromBinaryData(indexFile.readAll()));
    const auto index = indexDoc.object();
    for (auto it = index.begin(); it != index.end(); ++it) {
        if (!it.value().isObject())
            continue;
        const auto fileName = QString(path + QLatin1Char('/') + it.key());
        const auto defMap = it.value().toObject();
        Definition def;
        auto defData = DefinitionData::get(def);
        defData->repo = repo;
        if (defData->loadMetaData(fileName, defMap))
            addDefinition(def);
    }
    return true;
}

void RepositoryPrivate::addDefinition(const Definition &def)
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

void Repository::reload()
{
    qCDebug(Log) << "Reloading syntax definitions!";
    foreach (auto def, d->m_sortedDefs)
        DefinitionData::get(def)->clear();
    d->m_defs.clear();
    d->m_sortedDefs.clear();

    d->m_themes.clear();

    d->load(this);
}
