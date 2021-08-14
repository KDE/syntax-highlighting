/*
    SPDX-FileCopyrightText: 2021 Jonathan Poelen <jonathan.poelen@gmail.com>

    SPDX-License-Identifier: MIT
*/

#include "highlightingdata_p.hpp"

using namespace KSyntaxHighlighting;

template<class Data1, class Data2, class Visitor>
static void dataRuleVisit(HighlightingContextData::Rule::Type type, Data1 &&data1, Data2 &&data2, Visitor &&visitor)
{
    using Rule = HighlightingContextData::Rule;
    using Type = Rule::Type;
    switch (type) {
    case Type::AnyChar:
        visitor(data1.anyChar, data2.anyChar);
        break;
    case Type::DetectChar:
        visitor(data1.detectChar, data2.detectChar);
        break;
    case Type::Detect2Chars:
        visitor(data1.detect2Chars, data2.detect2Chars);
        break;
    case Type::HlCOct:
        visitor(data1.hlCOct, data2.hlCOct);
        break;
    case Type::IncludeRules:
        visitor(data1.includeRules, data2.includeRules);
        break;
    case Type::Int:
        visitor(data1.detectInt, data2.detectInt);
        break;
    case Type::Keyword:
        visitor(data1.keyword, data2.keyword);
        break;
    case Type::LineContinue:
        visitor(data1.lineContinue, data2.lineContinue);
        break;
    case Type::RangeDetect:
        visitor(data1.rangeDetect, data2.rangeDetect);
        break;
    case Type::RegExpr:
        visitor(data1.regExpr, data2.regExpr);
        break;
    case Type::StringDetect:
        visitor(data1.stringDetect, data2.stringDetect);
        break;
    case Type::WordDetect:
        visitor(data1.wordDetect, data2.wordDetect);
        break;
    case Type::Float:
        visitor(data1.detectFloat, data2.detectFloat);
        break;
    case Type::HlCHex:
        visitor(data1.hlCHex, data2.hlCHex);
        break;

    case Type::HlCStringChar:
    case Type::DetectIdentifier:
    case Type::DetectSpaces:
    case Type::HlCChar:
    case Type::Unknown:;
    }
}

HighlightingContextData::Rule::Rule() noexcept = default;

HighlightingContextData::Rule::Rule(Rule &&other) noexcept
    : common(std::move(other.common))
{
    dataRuleVisit(other.type, data, other.data, [](auto &data1, auto &data2) {
        using Data = std::remove_reference_t<decltype(data1)>;
        new (&data1) Data(std::move(data2));
    });
    type = other.type;
}

HighlightingContextData::Rule::Rule(const Rule &other)
    : common(other.common)
{
    dataRuleVisit(other.type, data, other.data, [](auto &data1, auto &data2) {
        using Data = std::remove_reference_t<decltype(data1)>;
        new (&data1) Data(data2);
    });
    type = other.type;
}

HighlightingContextData::Rule::~Rule()
{
    dataRuleVisit(type, data, data, [](auto &data, auto &) {
        using Data = std::remove_reference_t<decltype(data)>;
        data.~Data();
    });
}

HighlightingContextData::ContextSwitch::ContextSwitch(QStringView str)
{
    if (str.isEmpty() || str == QStringLiteral("#stay")) {
        return;
    }

    while (str.startsWith(QStringLiteral("#pop"))) {
        ++m_popCount;
        if (str.size() > 4 && str.at(4) == QLatin1Char('!')) {
            str = str.mid(5);
            break;
        }
        str = str.mid(4);
    }

    if (str.isEmpty()) {
        return;
    }

    m_contextAndDefName = str.toString();
    m_defNameIndex = str.indexOf(QStringLiteral("##"));
}

bool HighlightingContextData::ContextSwitch::isStay() const
{
    return m_popCount == -1 && m_contextAndDefName.isEmpty();
}

QStringView HighlightingContextData::ContextSwitch::contextName() const
{
    if (m_defNameIndex == -1) {
        return m_contextAndDefName;
    }
    return QStringView(m_contextAndDefName).left(m_defNameIndex);
}

QStringView HighlightingContextData::ContextSwitch::defName() const
{
    if (m_defNameIndex == -1) {
        return QStringView();
    }
    return QStringView(m_contextAndDefName).mid(m_defNameIndex + 2);
}
