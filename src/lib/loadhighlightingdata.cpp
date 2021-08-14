/*
    SPDX-FileCopyrightText: 2021 Jonathan Poelen <jonathan.poelen@gmail.com>

    SPDX-License-Identifier: MIT
*/

#include "ksyntaxhighlighting_logging.h"
#include "loadhighlightingdata_p.hpp"
#include "xml_p.h"

#include <QXmlStreamReader>

using namespace KSyntaxHighlighting;

template<class Data, class... Args>
static void initRuleData(Data &data, Args &&...args)
{
    new (&data) Data{std::move(args)...};
}

static Qt::CaseSensitivity attrToCaseSensitivity(QStringView str)
{
    return Xml::attrToBool(str) ? Qt::CaseInsensitive : Qt::CaseSensitive;
}

static HighlightingContextData::Rule::WordDelimiters loadAdditionalWordDelimiters(QXmlStreamReader &reader)
{
    return HighlightingContextData::Rule::WordDelimiters{
        reader.attributes().value(QLatin1String("additionalDeliminator")).toString(),
        reader.attributes().value(QLatin1String("weakDeliminator")).toString(),
    };
}

static bool checkIsNotEmpty(const QStringRef &str, const char *attrName, const QString &defName, QXmlStreamReader &reader)
{
    if (!str.isEmpty()) {
        return true;
    }

    qCWarning(Log) << defName << "at line" << reader.lineNumber() << ": " << attrName << "attribute is empty";
    return false;
}

static bool checkIsChar(const QStringRef &str, const char *attrName, const QString &defName, QXmlStreamReader &reader)
{
    if (!str.isEmpty()) {
        return true;
    }

    qCWarning(Log) << defName << "at line" << reader.lineNumber() << ": " << attrName << "attribute must contain exactly 1 character";
    return false;
}

static void loadRule(const QString &defName, std::vector<HighlightingContextData::Rule> &rules, QXmlStreamReader &reader)
{
    using Rule = HighlightingContextData::Rule;
    Rule rule;

    QStringView name = reader.name();
    const auto attrs = reader.attributes();
    bool isIncludeRules = false;

    if (name == QLatin1String("DetectChar")) {
        const auto s = attrs.value(QLatin1String("char"));
        if (!checkIsChar(s, "char", defName, reader)) {
            return;
        }
        const QChar c = s.at(0);
        const bool dynamic = Xml::attrToBool(attrs.value(QLatin1String("dynamic")));

        initRuleData(rule.data.detectChar, c, dynamic);
        rule.type = Rule::Type::DetectChar;
    } else if (name == QLatin1String("RegExpr")) {
        const auto pattern = attrs.value(QLatin1String("String"));
        if (!checkIsNotEmpty(pattern, "String", defName, reader)) {
            return;
        }

        const auto isCaseInsensitive = attrToCaseSensitivity(attrs.value(QLatin1String("insensitive")));
        const auto isMinimal = Xml::attrToBool(attrs.value(QLatin1String("minimal")));
        const auto dynamic = Xml::attrToBool(attrs.value(QLatin1String("dynamic")));

        initRuleData(rule.data.regExpr, pattern.toString(), isCaseInsensitive, isMinimal, dynamic);
        rule.type = Rule::Type::RegExpr;
    } else if (name == QLatin1String("IncludeRules")) {
        const auto context = attrs.value(QLatin1String("context"));
        if (!checkIsNotEmpty(context, "context", defName, reader)) {
            return;
        }
        const bool includeAttribute = Xml::attrToBool(attrs.value(QLatin1String("includeAttrib")));

        initRuleData(rule.data.includeRules, context.toString(), includeAttribute);
        rule.type = Rule::Type::IncludeRules;
        isIncludeRules = true;
    } else if (name == QLatin1String("Detect2Chars")) {
        const auto s1 = attrs.value(QLatin1String("char"));
        const auto s2 = attrs.value(QLatin1String("char1"));
        if (!checkIsChar(s1, "char", defName, reader)) {
            return;
        }
        if (!checkIsChar(s2, "char1", defName, reader)) {
            return;
        }

        initRuleData(rule.data.detect2Chars, s1.at(0), s2.at(0));
        rule.type = Rule::Type::Detect2Chars;
    } else if (name == QLatin1String("keyword")) {
        const auto s = attrs.value(QLatin1String("String"));
        if (!checkIsNotEmpty(s, "String", defName, reader)) {
            return;
        }
        Qt::CaseSensitivity caseSensitivityOverride = Qt::CaseInsensitive;
        bool hasCaseSensitivityOverride = false;

        /**
         * we might overwrite the case sensitivity
         * then we need to init the list for lookup of that sensitivity setting
         */
        if (attrs.hasAttribute(QLatin1String("insensitive"))) {
            hasCaseSensitivityOverride = true;
            caseSensitivityOverride = attrToCaseSensitivity(attrs.value(QLatin1String("insensitive")));
        }

        initRuleData(rule.data.keyword, s.toString(), loadAdditionalWordDelimiters(reader), caseSensitivityOverride, hasCaseSensitivityOverride);
        rule.type = Rule::Type::Keyword;
    } else if (name == QLatin1String("DetectSpaces")) {
        rule.type = Rule::Type::DetectSpaces;
    } else if (name == QLatin1String("StringDetect")) {
        const auto string = attrs.value(QLatin1String("String"));
        if (!checkIsNotEmpty(string, "String", defName, reader)) {
            return;
        }
        const auto caseSensitivity = attrToCaseSensitivity(attrs.value(QLatin1String("insensitive")));
        const auto dynamic = Xml::attrToBool(attrs.value(QLatin1String("dynamic")));

        initRuleData(rule.data.stringDetect, string.toString(), caseSensitivity, dynamic);
        rule.type = Rule::Type::StringDetect;
    } else if (name == QLatin1String("WordDetect")) {
        const auto word = attrs.value(QLatin1String("String"));
        if (!checkIsNotEmpty(word, "String", defName, reader)) {
            return;
        }
        const auto caseSensitivity = attrToCaseSensitivity(attrs.value(QLatin1String("insensitive")));

        initRuleData(rule.data.wordDetect, word.toString(), loadAdditionalWordDelimiters(reader), caseSensitivity);
        rule.type = Rule::Type::WordDetect;
    } else if (name == QLatin1String("AnyChar")) {
        const auto chars = attrs.value(QLatin1String("String"));
        if (!checkIsNotEmpty(chars, "String", defName, reader)) {
            return;
        }

        initRuleData(rule.data.anyChar, chars.toString());
        rule.type = Rule::Type::AnyChar;
    } else if (name == QLatin1String("DetectIdentifier")) {
        rule.type = Rule::Type::DetectIdentifier;
    } else if (name == QLatin1String("LineContinue")) {
        const auto s = attrs.value(QLatin1String("char"));
        const QChar c = s.isEmpty() ? QLatin1Char('\\') : s.at(0);

        initRuleData(rule.data.lineContinue, c);
        rule.type = Rule::Type::LineContinue;
    } else if (name == QLatin1String("Int")) {
        initRuleData(rule.data.detectInt, loadAdditionalWordDelimiters(reader));
        rule.type = Rule::Type::Int;
    } else if (name == QLatin1String("Float")) {
        initRuleData(rule.data.detectFloat, loadAdditionalWordDelimiters(reader));
        rule.type = Rule::Type::Float;
    } else if (name == QLatin1String("HlCStringChar")) {
        rule.type = Rule::Type::HlCStringChar;
    } else if (name == QLatin1String("RangeDetect")) {
        const auto s1 = attrs.value(QLatin1String("char"));
        const auto s2 = attrs.value(QLatin1String("char1"));
        if (!checkIsChar(s1, "char", defName, reader)) {
            return;
        }
        if (!checkIsChar(s2, "char1", defName, reader)) {
            return;
        }

        initRuleData(rule.data.rangeDetect, s1.at(0), s2.at(0));
        rule.type = Rule::Type::RangeDetect;
    } else if (name == QLatin1String("HlCHex")) {
        initRuleData(rule.data.hlCHex, loadAdditionalWordDelimiters(reader));
        rule.type = Rule::Type::HlCHex;
    } else if (name == QLatin1String("HlCChar")) {
        rule.type = Rule::Type::HlCChar;
    } else if (name == QLatin1String("HlCOct")) {
        initRuleData(rule.data.hlCOct, loadAdditionalWordDelimiters(reader));
        rule.type = Rule::Type::HlCOct;
    } else {
        qCWarning(Log) << "Unknown rule type:" << name;
        return;
    }

    if (!isIncludeRules) {
        rule.common.contextName = attrs.value(QLatin1String("context")).toString();
        rule.common.beginRegionName = attrs.value(QLatin1String("beginRegion")).toString();
        rule.common.endRegionName = attrs.value(QLatin1String("endRegion")).toString();
        rule.common.attributeName = attrs.value(QLatin1String("attribute")).toString();
        rule.common.firstNonSpace = Xml::attrToBool(attrs.value(QLatin1String("firstNonSpace")));
        rule.common.lookAhead = Xml::attrToBool(attrs.value(QLatin1String("lookAhead")));
        bool colOk = false;
        rule.common.column = attrs.value(QLatin1String("column")).toInt(&colOk);
        if (!colOk) {
            rule.common.column = -1;
        }
    }

    rules.push_back(std::move(rule));
}

HighlightingContextData KSyntaxHighlighting::loadContextData(const QString &defName, QXmlStreamReader &reader)
{
    Q_ASSERT(reader.name() == QLatin1String("context"));
    Q_ASSERT(reader.tokenType() == QXmlStreamReader::StartElement);

    HighlightingContextData data;

    data.name = reader.attributes().value(QLatin1String("name")).toString();
    data.attribute = reader.attributes().value(QLatin1String("attribute")).toString();
    data.lineEndContext = reader.attributes().value(QLatin1String("lineEndContext")).toString();
    data.lineEmptyContext = reader.attributes().value(QLatin1String("lineEmptyContext")).toString();
    data.fallthroughContext = reader.attributes().value(QLatin1String("fallthroughContext")).toString();
    data.noIndentationBasedFolding = Xml::attrToBool(reader.attributes().value(QLatin1String("noIndentationBasedFolding")));

    data.rules.reserve(8);

    reader.readNext();
    while (!reader.atEnd()) {
        switch (reader.tokenType()) {
        case QXmlStreamReader::StartElement: {
            loadRule(defName, data.rules, reader);
            // be done with this rule, skip all subelements, e.g. no longer supported sub-rules
            reader.skipCurrentElement();
            reader.readNext();
            break;
        }
        case QXmlStreamReader::EndElement:
            return data;
        default:
            reader.readNext();
            break;
        }
    }

    return data;
}
