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

#include "context.h"
#include "rule.h"
#include "syntaxdefinition.h"
#include "syntaxrepository.h"

#include <QDebug>
#include <QString>
#include <QXmlStreamReader>

using namespace KateSyntax;

Context::Context() :
    m_def(nullptr),
    m_resolveState(Unknown),
    m_fallthrough(false)
{
}

Context::~Context()
{
}

SyntaxDefinition* Context::syntaxDefinition() const
{
    return m_def;
}

void Context::setSyntaxDefinition(SyntaxDefinition* def)
{
    m_def = def;
}

QString Context::name() const
{
    return m_name;
}

QString Context::attribute() const
{
    return m_attribute;
}

QString Context::lineEndContext() const
{
    return m_lineEndContext;
}

bool Context::fallthrough() const
{
    return m_fallthrough;
}

QString Context::fallthroughContext() const
{
    return m_fallthroughContext;
}

QVector<Rule::Ptr> Context::rules() const
{
    return m_rules;
}

void Context::load(QXmlStreamReader& reader)
{
    Q_ASSERT(reader.name() == QLatin1String("context"));
    Q_ASSERT(reader.tokenType() == QXmlStreamReader::StartElement);

    m_name = reader.attributes().value(QStringLiteral("name")).toString();
    m_attribute = reader.attributes().value(QStringLiteral("attribute")).toString();
    m_lineEndContext = reader.attributes().value(QStringLiteral("lineEndContext")).toString();
    if (m_lineEndContext.isEmpty())
        m_lineEndContext = QStringLiteral("#stay");
    m_fallthrough = reader.attributes().value(QStringLiteral("fallthrough")) == QLatin1String("true");
    m_fallthroughContext = reader.attributes().value(QStringLiteral("fallthroughContext")).toString();
    if (m_fallthroughContext.isEmpty())
        m_fallthrough = false;

    reader.readNext();
    while (!reader.atEnd()) {
        switch (reader.tokenType()) {
            case QXmlStreamReader::StartElement:
            {
                auto rule = Rule::create(reader.name());
                if (rule) {
                    rule->setSyntaxDefinition(m_def);
                    if (rule->load(reader))
                        m_rules.push_back(rule);
                } else {
                    reader.skipCurrentElement();
                }
                reader.readNext();
                break;
            }
            case QXmlStreamReader::EndElement:
                return;
            default:
                reader.readNext();
                break;
        }
    }
}

Context::ResolveState Context::resolveState()
{
    if (m_resolveState == Unknown) {
        foreach (auto rule, m_rules) {
            auto inc = std::dynamic_pointer_cast<IncludeRules>(rule);
            if (inc) {
                m_resolveState = Unresolved;
                return m_resolveState;
            }
        }
        m_resolveState = Resolved;
    }
    return m_resolveState;
}

void Context::resolveIncludes()
{
    if (resolveState() == Resolved)
        return;
    if (resolveState() == Resolving) {
        qWarning() << "Cyclic dependency!";
        return;
    }

    Q_ASSERT(resolveState() == Unresolved);
    m_resolveState = Resolving; // cycle guard

    for (auto it = m_rules.begin(); it != m_rules.end();) {
        auto inc = std::dynamic_pointer_cast<IncludeRules>(*it);
        if (!inc) {
            ++it;
            continue;
        }
        Context* context = nullptr;
        if (inc->definitionName().isEmpty()) { // local include
            context = m_def->contextByName(inc->contextName());
        } else {
            auto def = m_def->syntaxRepository()->definitionForName(inc->definitionName());
            if (!def) {
                qWarning() << "Unable to resolve external include rule for definition" << inc->definitionName() << "in" << m_def->name();
                ++it;
                continue;
            }
            context = def->contextByName(inc->contextName());
        }
        if (!context) {
            qWarning() << "Unable to resolve include rule for definition" << inc->contextName() << "##" << inc->definitionName() << "in" << m_def->name();
            ++it;
            continue;
        }
        context->resolveIncludes();
        it = m_rules.erase(it);
        foreach (auto rule, context->rules()) {
            it = m_rules.insert(it, rule);
            ++it;
        }
    }

    m_resolveState = Resolved;
}
