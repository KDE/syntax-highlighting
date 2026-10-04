/*
    SPDX-FileCopyrightText: 2026 Jonathan Poelen <jonathan.poelen@gmail.com>

    SPDX-License-Identifier: MIT
*/

#ifndef KSYNTAXHIGHLIGHTING_HIGHLIGHTINGCONTEXT_P_H
#define KSYNTAXHIGHLIGHTING_HIGHLIGHTINGCONTEXT_P_H

#include "rule_p.h"

#include <vector>

namespace KSyntaxHighlighting
{

class HighlightingContext
{
public:
    Q_DISABLE_COPY_MOVE(HighlightingContext)

    HighlightingContext() = default;

    void clear()
    {
        m_rules.clear();
    }

    Rule *pushRule(Rule::Ptr &&rule)
    {
        if (rule) {
            return m_rules.emplace_back(std::move(rule)).get();
        }
        return nullptr;
    }

private:
    std::vector<std::unique_ptr<Rule>> m_rules;
};

}

#endif
