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

#include "syntaxdefinition.h"
#include "context.h"
#include "rule.h"

#include <QDebug>
#include <QFile>
#include <QXmlStreamReader>

using namespace KateSyntax;

SyntaxDefinition::SyntaxDefinition() :
    m_repo(nullptr),
    m_hidden(false)
{
}

SyntaxDefinition::~SyntaxDefinition()
{
    qDeleteAll(m_contexts);
}

SyntaxRepository* SyntaxDefinition::syntaxRepository() const
{
    return m_repo;
}

void SyntaxDefinition::setSyntaxRepository(SyntaxRepository* repo)
{
    m_repo = repo;
}

QString SyntaxDefinition::name() const
{
    return m_name;
}

QVector<QString> SyntaxDefinition::extensions() const
{
    return m_extensions;
}

Context* SyntaxDefinition::initialContext() const
{
    Q_ASSERT(!m_contexts.isEmpty());
    return m_contexts.first();
}

Context* SyntaxDefinition::contextByName(const QString& name) const
{
    foreach (auto context, m_contexts) {
        if (context->name() == name)
            return context;
    }
    return nullptr;
}

KeywordList SyntaxDefinition::keywordList(const QString& name) const
{
    return m_keywordLists.value(name);
}

Format SyntaxDefinition::formatByName(const QString& name) const
{
    const auto it = m_formats.constFind(name);
    if (it != m_formats.constEnd())
        return it.value();

    qWarning() << "Unknown format" << name << "in" << m_name;
    return Format();
}

bool SyntaxDefinition::load(const QString& definitionFileName)
{
    QFile file(definitionFileName);
    if (!file.open(QFile::ReadOnly))
        return false;

    QXmlStreamReader reader(&file);
    while (!reader.atEnd()) {
        const auto token = reader.readNext();
        if (token != QXmlStreamReader::StartElement)
            continue;

        if (reader.name() == QLatin1String("language")) {
            m_name = reader.attributes().value(QStringLiteral("name")).toString();
            m_section = reader.attributes().value(QStringLiteral("section")).toString();
            m_hidden = reader.attributes().value(QStringLiteral("hidden")) == QLatin1String("true");
            const auto exts = reader.attributes().value(QStringLiteral("extensions")).toString();
            foreach (const auto &ext, exts.split(QLatin1Char(';'), QString::SkipEmptyParts)) {
                if (ext.startsWith(QLatin1String("*.")))
                    m_extensions.push_back(ext.mid(2));
                else
                    m_extensions.push_back(ext);
            }
            const auto mts = reader.attributes().value(QStringLiteral("mimetypes")).toString();
            foreach (const auto &mt, mts.split(QLatin1Char(';'), QString::SkipEmptyParts))
                m_mimetypes.push_back(mt);
        }

        if (reader.name() == QLatin1String("highlighting"))
            loadHighlighting(reader);
    }

    return true;
}

void SyntaxDefinition::loadHighlighting(QXmlStreamReader& reader)
{
    Q_ASSERT(reader.name() == QLatin1String("highlighting"));
    Q_ASSERT(reader.tokenType() == QXmlStreamReader::StartElement);

    while (!reader.atEnd()) {
        switch (reader.tokenType()) {
            case QXmlStreamReader::StartElement:
                if (reader.name() == QLatin1String("list")) {
                    KeywordList keywords;
                    keywords.load(reader);
                    m_keywordLists.insert(keywords.name(), keywords);
                } else if (reader.name() == QLatin1String("contexts")) {
                    loadContexts(reader);
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

void SyntaxDefinition::loadContexts(QXmlStreamReader& reader)
{
    Q_ASSERT(reader.name() == QLatin1String("contexts"));
    Q_ASSERT(reader.tokenType() == QXmlStreamReader::StartElement);

    while (!reader.atEnd()) {
        switch (reader.tokenType()) {
            case QXmlStreamReader::StartElement:
                if (reader.name() == QLatin1String("context")) {
                    auto context = new Context;
                    context->setSyntaxDefinition(this);
                    context->load(reader);
                    m_contexts.push_back(context);
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

void SyntaxDefinition::loadItemData(QXmlStreamReader& reader)
{
    Q_ASSERT(reader.name() == QLatin1String("itemDatas"));
    Q_ASSERT(reader.tokenType() == QXmlStreamReader::StartElement);

    while (!reader.atEnd()) {
        switch (reader.tokenType()) {
            case QXmlStreamReader::StartElement:
                if (reader.name() == QLatin1String("itemData")) {
                    Format f;
                    f.load(reader);
                    m_formats.insert(f.name(), f);
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

void SyntaxDefinition::assemble()
{
    foreach (auto context, m_contexts) {
        context->resolveContexts();
        context->resolveIncludes();
    }
}
