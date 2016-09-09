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
#include "repository.h"

#include <QDebug>
#include <QString>
#include <QXmlStreamReader>

using namespace SyntaxHighlighting;

Context::Context() :
    m_resolveState(Unknown),
    m_fallthrough(false)
{
}

Context::~Context()
{
}

Definition Context::syntaxDefinition() const
{
    return m_def.definition();
}

void Context::setDefinition(const Definition &def)
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

ContextSwitch Context::lineEndContext() const
{
    return m_lineEndContext;
}

ContextSwitch Context::lineEmptyContext() const
{
    return m_lineEmptyContext;
}

bool Context::fallthrough() const
{
    return m_fallthrough;
}

ContextSwitch Context::fallthroughContext() const
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
    m_lineEndContext.parse(reader.attributes().value(QStringLiteral("lineEndContext")));
    m_lineEmptyContext.parse(reader.attributes().value(QStringLiteral("lineEmptyContext")));
    m_fallthrough = reader.attributes().value(QStringLiteral("fallthrough")) == QLatin1String("true");
    m_fallthroughContext.parse(reader.attributes().value(QStringLiteral("fallthroughContext")));
    if (m_fallthroughContext.isStay())
        m_fallthrough = false;

    reader.readNext();
    while (!reader.atEnd()) {
        switch (reader.tokenType()) {
            case QXmlStreamReader::StartElement:
            {
                auto rule = Rule::create(reader.name());
                if (rule) {
                    rule->setDefinition(m_def.definition());
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

void Context::resolveContexts()
{
    const auto def = m_def.definition();
    m_lineEndContext.resolve(def);
    m_lineEmptyContext.resolve(def);
    m_fallthroughContext.resolve(def);
    foreach (auto rule, m_rules)
        rule->resolveContext();
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
            context = m_def.definition().contextByName(inc->contextName());
        } else {
            auto def = m_def.definition().repository()->definitionForName(inc->definitionName());
            if (!def.isValid()) {
                qWarning() << "Unable to resolve external include rule for definition" << inc->definitionName() << "in" << m_def.definition().name();
                ++it;
                continue;
            }
            if (inc->contextName().isEmpty())
                context = def.initialContext();
            else
                context = def.contextByName(inc->contextName());
        }
        if (!context) {
            qWarning() << "Unable to resolve include rule for definition" << inc->contextName() << "##" << inc->definitionName() << "in" << m_def.definition().name();
            ++it;
            continue;
        }
        context->resolveIncludes();
        if (inc->includeAttribute()) {
            m_attribute = context->attribute();
        }
        it = m_rules.erase(it);
        foreach (auto rule, context->rules()) {
            it = m_rules.insert(it, rule);
            ++it;
        }
    }

    m_resolveState = Resolved;
}
