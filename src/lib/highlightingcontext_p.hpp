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
        skipOffsetId = SkipOffsetId{};
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

    struct RegexpWithSkipOffsetId {
        QRegularExpression regexp;
        SkipOffsetId skipOffsetId;

        RegexpWithSkipOffsetId(QString const &pattern, QRegularExpression::PatternOptions options)
            : regexp(pattern, options)
        {
        }
    };

    const RegexpWithSkipOffsetId &registryRegexp(QString const &pattern, QRegularExpression::PatternOptions options)
    {
        auto result = m_regexes.tryEmplace(RegexKey(pattern, options), pattern, options);
        if (result.inserted) {
            result.iterator->skipOffsetId = nextSkipOffsetId();
        }
        return *result.iterator;
    }

    SkipOffsetId nextSkipOffsetId()
    {
        using Int = std::underlying_type_t<SkipOffsetId>;
        auto id = static_cast<Int>(skipOffsetId) + 1;
        skipOffsetId = SkipOffsetId(id);
        return skipOffsetId;
    }

private:
    using RegexKey = std::pair<QString, QRegularExpression::PatternOptions>;

    SkipOffsetId skipOffsetId{};
    std::vector<std::unique_ptr<Rule>> m_rules;
    QHash<RegexKey, RegexpWithSkipOffsetId> m_regexes;
};

}

#endif
