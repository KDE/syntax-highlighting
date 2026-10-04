/*
    SPDX-FileCopyrightText: 2026 Jonathan Poelen <jonathan.poelen@gmail.com>

    SPDX-License-Identifier: MIT
*/

#ifndef KSYNTAXHIGHLIGHTING_HIGHLIGHTINGCONTEXT_P_H
#define KSYNTAXHIGHLIGHTING_HIGHLIGHTINGCONTEXT_P_H

#include "dynamicregexpcache_p.h"
#include "rule_p.h"

#include <QHash>
#include <QRegularExpression>

#include <utility>
#include <vector>

namespace KSyntaxHighlighting
{

class HighlightingContext
{
public:
    DynamicRegexpCache dynamicRegexpCache;

    Q_DISABLE_COPY_MOVE(HighlightingContext)

    HighlightingContext() = default;

    void clear()
    {
        m_rules.clear();
        m_regexes.clear();
    }

    Rule *pushRule(Rule::Ptr &&rule)
    {
        if (rule) {
            return m_rules.emplace_back(std::move(rule)).get();
        }
        return nullptr;
    }

    QRegularExpression registryRegexp(QString const &pattern, QRegularExpression::PatternOptions options)
    {
        auto result = m_regexes.tryEmplace(RegexKey(pattern, options), pattern, options);
        return *result.iterator;
    }

private:
    using RegexKey = std::pair<QString, QRegularExpression::PatternOptions>;

    std::vector<std::unique_ptr<Rule>> m_rules;
    QHash<RegexKey, QRegularExpression> m_regexes;
};

}

#endif
