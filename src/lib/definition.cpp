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


#include "definition.h"
#include "definition_p.h"
#include "definitionref_p.h"

#include "context_p.h"
#include "format.h"
#include "rule_p.h"
#include "syntaxhighlighting_logging.h"
#include "syntaxhighlighting_version.h"
#include "xml_p.h"

#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QHash>
#include <QJsonObject>
#include <QVector>
#include <QXmlStreamReader>

#include <algorithm>

using namespace SyntaxHighlighting;

DefinitionData::DefinitionData() :
    repo(Q_NULLPTR),
    delimiters(QStringLiteral(".():!+,-<=>%&*/;?[]^{|}~\\ \t")),
    caseSensitive(Qt::CaseSensitive),
    version(0.0f),
    priority(0),
    hidden(false)
{
}

DefinitionData::~DefinitionData()
{
    qDeleteAll(contexts);
}

DefinitionData* DefinitionData::get(const Definition &def)
{
    return def.d.get();
}

Definition::Definition() :
    d(new DefinitionData)
{
}

Definition::Definition(const Definition &other) :
    d(other.d)
{
}

Definition::~Definition()
{
}

Definition& Definition::operator=(const Definition &rhs)
{
    d = rhs.d;
    return *this;
}

bool Definition::isValid() const
{
    return d->repo && !d->fileName.isEmpty();
}

QString Definition::filePath() const
{
    return d->fileName;
}

Repository* Definition::repository() const
{
    return d->repo;
}

QString Definition::name() const
{
    return d->name;
}

QString Definition::translatedName() const
{
    return QCoreApplication::instance()->translate("Language", d->name.toUtf8().constData());
}

QString Definition::section() const
{
    return d->section;
}

QString Definition::translatedSection() const
{
    return QCoreApplication::instance()->translate("Language Section", d->section.toUtf8().constData());
}

QVector<QString> Definition::mimeTypes() const
{
    return d->mimetypes;
}

QVector<QString> Definition::extensions() const
{
    return d->extensions;
}

float Definition::version() const
{
    return d->version;
}

int Definition::priority() const
{
    return d->priority;
}

bool Definition::isHidden() const
{
    return d->hidden;
}

QString Definition::style() const
{
    return d->style;
}

QString Definition::indenter() const
{
    return d->indenter;
}

QString Definition::author() const
{
    return d->author;
}

QString Definition::license() const
{
    return d->license;
}

Context* Definition::initialContext() const
{
    Q_ASSERT(!d->contexts.isEmpty());
    return d->contexts.first();
}

Context* Definition::contextByName(const QString& name) const
{
    foreach (auto context, d->contexts) {
        if (context->name() == name)
            return context;
    }
    return Q_NULLPTR;
}

KeywordList Definition::keywordList(const QString& name) const
{
    return d->keywordLists.value(name);
}

bool Definition::isDelimiter(QChar c) const
{
    return d->delimiters.contains(c);
}

Format Definition::formatByName(const QString& name) const
{
    const auto it = d->formats.constFind(name);
    if (it != d->formats.constEnd())
        return it.value();

    return Format();
}

bool DefinitionData::isLoaded() const
{
    return !contexts.isEmpty();
}

bool Definition::load()
{
    if (d->isLoaded())
        return true;

    Q_ASSERT(!d->fileName.isEmpty());
    QFile file(d->fileName);
    if (!file.open(QFile::ReadOnly))
        return false;

    QXmlStreamReader reader(&file);
    while (!reader.atEnd()) {
        const auto token = reader.readNext();
        if (token != QXmlStreamReader::StartElement)
            continue;

        if (reader.name() == QLatin1String("highlighting"))
            d->loadHighlighting(this, reader);

        else if (reader.name() == QLatin1String("general"))
            d->loadGeneral(reader);
    }

    for (auto it = d->keywordLists.begin(); it != d->keywordLists.end(); ++it)
        (*it).setCaseSensitivity(d->caseSensitive);

    foreach (auto context, d->contexts) {
        context->resolveContexts();
        context->resolveIncludes();
    }

    return true;
}

bool DefinitionData::loadMetaData(const QString& definitionFileName)
{
    fileName = definitionFileName;

    QFile file(definitionFileName);
    if (!file.open(QFile::ReadOnly))
        return false;

    QXmlStreamReader reader(&file);
    while (!reader.atEnd()) {
        const auto token = reader.readNext();
        if (token != QXmlStreamReader::StartElement)
            continue;
        if (reader.name() == QLatin1String("language")) {
            return loadLanguage(reader);
        }
    }

    return false;
}

bool DefinitionData::loadMetaData(const QString &file, const QJsonObject &obj)
{
    name     = obj.value(QLatin1String("name")).toString();
    section  = obj.value(QLatin1String("section")).toString();
    version  = obj.value(QLatin1String("version")).toDouble();
    priority = obj.value(QLatin1String("priority")).toInt();
    style    = obj.value(QLatin1String("style")).toString();
    author   = obj.value(QLatin1String("author")).toString();
    license  = obj.value(QLatin1String("license")).toString();
    indenter = obj.value(QLatin1String("indenter")).toString();
    hidden   = obj.value(QLatin1String("hidden")).toBool();
    fileName = file;

    const auto exts = obj.value(QLatin1String("extensions")).toString();
    foreach (const auto &ext, exts.split(QLatin1Char(';'), QString::SkipEmptyParts))
        extensions.push_back(ext);
    const auto mts = obj.value(QLatin1String("mimetype")).toString();
    foreach (const auto &mt, mts.split(QLatin1Char(';'), QString::SkipEmptyParts))
        mimetypes.push_back(mt);

    return true;
}

bool DefinitionData::loadLanguage(QXmlStreamReader &reader)
{
    Q_ASSERT(reader.name() == QLatin1String("language"));
    Q_ASSERT(reader.tokenType() == QXmlStreamReader::StartElement);

    if (!checkKateVersion(reader.attributes().value(QStringLiteral("kateversion"))))
        return false;

    name = reader.attributes().value(QStringLiteral("name")).toString();
    section = reader.attributes().value(QStringLiteral("section")).toString();
    version = reader.attributes().value(QStringLiteral("version")).toFloat();
    priority = reader.attributes().value(QStringLiteral("priority")).toInt();
    hidden = reader.attributes().value(QStringLiteral("hidden")) == QLatin1String("true");
    style = reader.attributes().value(QStringLiteral("style")).toString();
    indenter = reader.attributes().value(QStringLiteral("indenter")).toString();
    author = reader.attributes().value(QStringLiteral("author")).toString();
    license = reader.attributes().value(QStringLiteral("license")).toString();
    const auto exts = reader.attributes().value(QStringLiteral("extensions")).toString();
    foreach (const auto &ext, exts.split(QLatin1Char(';'), QString::SkipEmptyParts))
        extensions.push_back(ext);
    const auto mts = reader.attributes().value(QStringLiteral("mimetype")).toString();
    foreach (const auto &mt, mts.split(QLatin1Char(';'), QString::SkipEmptyParts))
        mimetypes.push_back(mt);
    if (reader.attributes().hasAttribute(QStringLiteral("casesensitive")))
        caseSensitive = Xml::attrToBool(reader.attributes().value(QStringLiteral("casesensitive"))) ? Qt::CaseSensitive : Qt::CaseInsensitive;
    return true;
}

void DefinitionData::loadHighlighting(Definition *def, QXmlStreamReader& reader)
{
    Q_ASSERT(reader.name() == QLatin1String("highlighting"));
    Q_ASSERT(reader.tokenType() == QXmlStreamReader::StartElement);

    while (!reader.atEnd()) {
        switch (reader.tokenType()) {
            case QXmlStreamReader::StartElement:
                if (reader.name() == QLatin1String("list")) {
                    KeywordList keywords;
                    keywords.load(reader);
                    keywordLists.insert(keywords.name(), keywords);
                } else if (reader.name() == QLatin1String("contexts")) {
                    loadContexts(def, reader);
                    reader.readNext();
                } else if (reader.name() == QLatin1String("itemDatas")) {
                    loadItemData(reader);
                } else {
                    reader.readNext();
                }
                break;
            case QXmlStreamReader::EndElement:
                return;
            default:
                reader.readNext();
                break;
        }
    }
}

void DefinitionData::loadContexts(Definition *def, QXmlStreamReader& reader)
{
    Q_ASSERT(reader.name() == QLatin1String("contexts"));
    Q_ASSERT(reader.tokenType() == QXmlStreamReader::StartElement);

    while (!reader.atEnd()) {
        switch (reader.tokenType()) {
            case QXmlStreamReader::StartElement:
                if (reader.name() == QLatin1String("context")) {
                    auto context = new Context;
                    context->setDefinition(*def);
                    context->load(reader);
                    contexts.push_back(context);
                }
                reader.readNext();
                break;
            case QXmlStreamReader::EndElement:
                return;
            default:
                reader.readNext();
                break;
        }
    }
}

void DefinitionData::loadItemData(QXmlStreamReader& reader)
{
    Q_ASSERT(reader.name() == QLatin1String("itemDatas"));
    Q_ASSERT(reader.tokenType() == QXmlStreamReader::StartElement);

    while (!reader.atEnd()) {
        switch (reader.tokenType()) {
            case QXmlStreamReader::StartElement:
                if (reader.name() == QLatin1String("itemData")) {
                    Format f;
                    f.load(reader);
                    formats.insert(f.name(), f);
                    reader.readNext();
                }
                reader.readNext();
                break;
            case QXmlStreamReader::EndElement:
                return;
            default:
                reader.readNext();
                break;
        }
    }
}

void DefinitionData::loadGeneral(QXmlStreamReader& reader)
{
    Q_ASSERT(reader.name() == QLatin1String("general"));
    Q_ASSERT(reader.tokenType() == QXmlStreamReader::StartElement);
    reader.readNext();

    while (!reader.atEnd()) {
        switch (reader.tokenType()) {
            case QXmlStreamReader::StartElement:
                if (reader.name() == QLatin1String("keywords")) {
                    if (reader.attributes().hasAttribute(QStringLiteral("casesensitive")))
                        caseSensitive = Xml::attrToBool(reader.attributes().value(QStringLiteral("casesensitive"))) ? Qt::CaseSensitive : Qt::CaseInsensitive;
                    delimiters += reader.attributes().value(QStringLiteral("additionalDeliminator"));
                    std::sort(delimiters.begin(), delimiters.end());
                    auto it = std::unique(delimiters.begin(), delimiters.end());
                    delimiters.truncate(std::distance(delimiters.begin(), it));
                    foreach (const auto c, reader.attributes().value(QLatin1String("weakDeliminator")))
                        delimiters.remove(c);
                } else {
                    reader.skipCurrentElement();
                }
                reader.readNext();
                break;
            case QXmlStreamReader::EndElement:
                return;
            default:
                reader.readNext();
                break;
        }
    }
}

bool DefinitionData::checkKateVersion(const QStringRef& verStr)
{
    const auto idx = verStr.indexOf(QLatin1Char('.'));
    if (idx <= 0) {
        qCWarning(Log) << "Skipping" << fileName << "due to having no valid kateversion attribute:" << verStr;
        return false;
    }
    const auto major = verStr.left(idx).toInt();
    const auto minor = verStr.mid(idx + 1).toInt();

    if (major > SyntaxHighlighting_VERSION_MAJOR || (major == SyntaxHighlighting_VERSION_MAJOR && minor > SyntaxHighlighting_VERSION_MINOR)) {
        qCWarning(Log) << "Skipping" << fileName << "due to being too new, version:" << verStr;
        return false;
    }

    return true;
}

DefinitionRef::DefinitionRef()
{
}

DefinitionRef::DefinitionRef(const Definition &def) :
    d(def.d)
{
}

DefinitionRef::~DefinitionRef()
{
}

DefinitionRef& DefinitionRef::operator=(const Definition &def)
{
    d = def.d;
    return *this;
}

Definition DefinitionRef::definition() const
{
    Definition def;
    if (!d.expired())
        def.d = d.lock();
    return def;
}
