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
#include "repository_p.h"
#include "definition.h"
#include "definition_p.h"
#include "theme.h"
#include "themedata_p.h"
#include "ksyntaxhighlighting_logging.h"
#include "wildcardmatcher_p.h"

#include <QDebug>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>

#include <limits>

using namespace KSyntaxHighlighting;

static void initResource()
{
    Q_INIT_RESOURCE(syntax_data);
}

RepositoryPrivate::RepositoryPrivate() :
    m_foldingRegionId(0),
    m_formatId(0)
{
}

RepositoryPrivate* RepositoryPrivate::get(Repository *repo)
{
    return repo->d.get();
}

Repository::Repository() :
    d(new RepositoryPrivate)
{
    initResource();
    d->load(this);
}

Repository::~Repository()
{
    // reset repo so we can detect in still alive definition instances
    // that the repo was deleted
    foreach (const auto &def, d->m_sortedDefs)
        DefinitionData::get(def)->repo = Q_NULLPTR;
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

Theme Repository::defaultTheme(Repository::DefaultTheme t)
{
    if (t == DarkTheme)
        return theme(QLatin1String("Breeze Dark"));
    return theme(QLatin1String("Default"));
}

void RepositoryPrivate::load(Repository *repo)
{
    auto dirs = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, QStringLiteral("org.kde.syntax-highlighting/syntax"), QStandardPaths::LocateDirectory);
    foreach (const auto &dir, dirs)
        loadSyntaxFolder(repo, dir);
    // backward compatibility with Kate
    dirs = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, QStringLiteral("katepart5/syntax"), QStandardPaths::LocateDirectory);
    foreach (const auto &dir, dirs)
        loadSyntaxFolder(repo, dir);

    loadSyntaxFolder(repo, QStringLiteral(":/org.kde.syntax-highlighting/syntax"));

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
    dirs = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, QStringLiteral("org.kde.syntax-highlighting/themes"), QStandardPaths::LocateDirectory);
    foreach (const auto &dir, dirs)
        loadThemeFolder(dir);
    loadThemeFolder(QStringLiteral(":/org.kde.syntax-highlighting/themes"));
}

void RepositoryPrivate::loadSyntaxFolder(Repository *repo, const QString &path)
{
    if (loadSyntaxFolderFromIndex(repo, path))
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

bool RepositoryPrivate::loadSyntaxFolderFromIndex(Repository *repo, const QString &path)
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

void RepositoryPrivate::loadThemeFolder(const QString &path)
{
    QDirIterator it(path);
    while (it.hasNext()) {
        auto themeData = std::unique_ptr<ThemeData>(new ThemeData);
        if (themeData->load(it.next()))
            addTheme(Theme(themeData.release()));
    }
}

static int themeRevision(const Theme &theme)
{
    auto data = ThemeData::get(theme);
    return data->revision();
}

void RepositoryPrivate::addTheme(const Theme &theme)
{
    const auto it = std::lower_bound(m_themes.begin(), m_themes.end(), theme, [](const Theme &lhs, const Theme &rhs) {
        return lhs.name() < rhs.name();
    });
    if (it == m_themes.end() || (*it).name() != theme.name()) {
        m_themes.insert(it, theme);
        return;
    }
    if (themeRevision(*it) < themeRevision(theme))
        *it = theme;
}

quint16 RepositoryPrivate::foldingRegionId(const QString &defName, const QString &foldName)
{
    const auto it = m_foldingRegionIds.constFind(qMakePair(defName, foldName));
    if (it != m_foldingRegionIds.constEnd())
        return it.value();
    m_foldingRegionIds.insert(qMakePair(defName, foldName), ++m_foldingRegionId);
    return m_foldingRegionId;
}

quint16 RepositoryPrivate::nextFormatId()
{
    Q_ASSERT(m_formatId < std::numeric_limits<quint16>::max());
    return ++m_formatId;
}

void Repository::reload()
{
    qCDebug(Log) << "Reloading syntax definitions!";
    foreach (const auto &def, d->m_sortedDefs)
        DefinitionData::get(def)->clear();
    d->m_defs.clear();
    d->m_sortedDefs.clear();

    d->m_themes.clear();

    d->m_foldingRegionId = 0;
    d->m_foldingRegionIds.clear();

    d->m_formatId = 0;

    d->load(this);
}
