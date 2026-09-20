/*
    SPDX-FileCopyrightText: 2016 Volker Krause <vkrause@kde.org>
    SPDX-FileCopyrightText: 2024 Jonathan Poelen <jonathan.poelen@gmail.com>

    SPDX-License-Identifier: MIT
*/

#include "contextswitch_p.h"
#include "definition_p.h"
#include "isdigit_p.hpp"
#include "ksyntaxhighlighting_logging.h"
#include <QStringTokenizer>

using namespace KSyntaxHighlighting;

void ContextSwitch::resolve(DefinitionData &def, QStringView context)
{
    if (context.isEmpty() || context == QStringLiteral("#stay")) {
        return;
    }

    while (context.startsWith(QStringLiteral("#pop"))) {
        qsizetype offset = 4;
        ++m_popCount;

        // find "#pop(count)" with maximum 2 digits for `count`.
        if (context.size() > offset + 2 && context.at(offset) == u'(') {
            qsizetype offset2 = offset + 1;
            if (isDigit(context.at(offset2))) {
                int popCount = context.at(offset2).unicode() - '0';
                ++offset2;
                if (isDigit(context.at(offset2))) {
                    popCount *= 10;
                    popCount += context.at(offset2).unicode() - '0';
                    ++offset2;
                }
                if (context.size() > offset2 && context.at(offset2) == u')') {
                    offset = offset2 + 1;
                    m_popCount += popCount - 1;
                }
            }
        }

        if (context.size() > offset && context.at(offset) == u'!') {
            context = context.sliced(offset + 1);
            break;
        }
        context = context.sliced(offset);
    }

    m_isStay = !m_popCount;

    if (context.isEmpty()) {
        return;
    }

    for (auto contextPart : QStringTokenizer{context, u'!'}) {
        const qsizetype defNameIndex = contextPart.indexOf(QStringLiteral("##"));
        auto defName = (defNameIndex <= -1) ? QStringView() : contextPart.sliced(defNameIndex + 2);
        auto contextName = (defNameIndex <= -1) ? contextPart : contextPart.sliced(0, defNameIndex);

        auto resolvedCtx = def.resolveIncludedContext(defName, contextName);
        if (resolvedCtx.context) {
            m_contexts.append(resolvedCtx.context);
        } else {
            auto part = (defName.isEmpty() || resolvedCtx.def) ? "context" : "definition in";
            qCWarning(Log) << "cannot find" << part << contextPart << "in" << def.name;
        }
    }

    m_isStay = m_contexts.isEmpty();
}
