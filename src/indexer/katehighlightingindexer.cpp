/*
    SPDX-FileCopyrightText: 2014 Christoph Cullmann <cullmann@kde.org>
    SPDX-FileCopyrightText: 2020 Jonathan Poelen <jonathan.poelen@gmail.com>

    SPDX-License-Identifier: MIT
*/

#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QCborValue>
#include <QRegularExpression>
#include <QVariant>
#include <QXmlStreamReader>
#include <QMutableMapIterator>

#ifdef QT_XMLPATTERNS_LIB
#include <QXmlSchema>
#include <QXmlSchemaValidator>
#endif

#include "../lib/xml_p.h"
#include "../lib/worddelimiters_p.h"


using KSyntaxHighlighting::Xml::attrToBool;
using KSyntaxHighlighting::WordDelimiters;

class HlFilesChecker
{
public:
    void setDefinition(const QStringRef &verStr, const QString &filename, const QString &name)
    {
        m_currentDefinition = &*m_definitions.insert(name, Definition{});
        m_currentDefinition->languageName = name;
        m_currentDefinition->filename = filename;
        m_currentDefinition->kateVersionStr = verStr.toString();

        const auto idx = verStr.indexOf(QLatin1Char('.'));
        if (idx <= 0) {
            qWarning() << filename << "invalid kateversion" << verStr;
            m_success = false;
        } else {
            m_currentDefinition->kateVersion = {
                verStr.left(idx).toInt(),
                verStr.mid(idx + 1).toInt()
            };
        }
    }

    void processElement(QXmlStreamReader &xml)
    {
        if (xml.isStartElement()) {
            if (m_currentContext) {
                m_currentContext->rules.append(Context::Rule{});
                auto &rule = m_currentContext->rules.back();
                m_success = rule.parseElement(m_currentDefinition->filename, xml) && m_success;
            }
            else if (m_currentKeywords) {
                m_success = m_currentKeywords->items.parseElement(m_currentDefinition->filename, xml) && m_success;
            }
            else if (xml.name() == QStringLiteral("context")) {
                processContextElement(xml);
            }
            else if (xml.name() == QStringLiteral("list")) {
                processListElement(xml);
            }
            else if (xml.name() == QStringLiteral("keywords")) {
                m_success = m_currentDefinition->parseKeywords(xml) && m_success;
            }
            else if (xml.name() == QStringLiteral("emptyLine")) {
                m_success = parseEmptyLine(m_currentDefinition->filename, xml) && m_success;
            }
            else if (xml.name() == QStringLiteral("itemData")) {
                m_success = m_currentDefinition->itemDatas.parseElement(m_currentDefinition->filename, xml) && m_success;
            }
        }
        else if (xml.isEndElement()) {
            if (m_currentContext && xml.name() == QStringLiteral("context")) {
                m_currentContext = nullptr;
            }
            else if (m_currentKeywords && xml.name() == QStringLiteral("list")) {
                m_currentKeywords = nullptr;
            }
        }
    }

    void resolveContexts()
    {
        QMutableMapIterator<QString, Definition> def(m_definitions);
        while (def.hasNext()) {
            def.next();
            auto &definition = def.value();
            auto &contexts = definition.contexts;

            if (contexts.isEmpty()) {
                qWarning() << definition.filename << "has no context";
                m_success = false;
                continue;
            }

            QMutableMapIterator<QString, Context> contextIt(contexts);
            while (contextIt.hasNext()) {
                contextIt.next();
                auto& context = contextIt.value();
                resolveContextName(definition, context.lineEndContext, context.line);
                resolveContextName(definition, context.lineEmptyContext, context.line);
                resolveContextName(definition, context.fallthroughContext, context.line);
                for (auto &rule : context.rules) {
                    resolveContextName(definition, rule.context, rule.line);
                }
            }

            definition.firstContext = &*definition.contexts.find(definition.firstContextName);
        }

        resolveIncludeRules();
    }

    bool check() const
    {
        bool success = m_success;

        const auto usedContexts = extractUsedContexts();

        QMapIterator<QString, Definition> def(m_definitions);
        while (def.hasNext()) {
            def.next();
            const auto &definition = def.value();
            const auto &filename = definition.filename;

            checkKateVersion(definition);

            QSet<const Keywords*> referencedKeywords;
            QSet<ItemDatas::Style> usedAttributeNames;
            checkKeywordsList(definition, referencedKeywords);
            checkContexts(definition, referencedKeywords, usedAttributeNames, usedContexts);

            // search for non-existing or unreferenced keyword lists.
            for (const auto &keywords : definition.keywordsList) {
                if (!referencedKeywords.contains(&keywords)) {
                    qWarning() << filename << "line" << keywords.line << "unused keyword:" << keywords.name;
                }
            }

            // search for non-existing itemDatas.
            const auto invalidNames = usedAttributeNames - definition.itemDatas.styleNames;
            for (const auto &styleName : invalidNames) {
                qWarning() << filename << "line" << styleName.line << "reference of non-existing itemData attributes:" << styleName.name;
                success = false;
            }

            // search for unused itemDatas.
            const auto unusedNames = definition.itemDatas.styleNames - usedAttributeNames;
            for (const auto &styleName : unusedNames) {
                qWarning() << filename << "line" << styleName.line << "unused itemData:" << styleName.name;
                success = false;
            }
        }

        return success;
    }

private:
    enum class XmlBool
    {
        Unspecified,
        False,
        True,
    };

    struct Context;

    struct ContextName
    {
        QString name;
        int popCount = 0;
        bool stay = false;

        const Context *context = nullptr;
    };

    struct Parser
    {
        const QString &filename;
        QXmlStreamReader &xml;
        QXmlStreamAttribute &attr;
        bool success;

        //! Read a string type attribute, \c sucess = \c false when \p str is not empty
        //! \return \c true when attr.name() == attrName, otherwise false
        bool extractString(QString& str, const QString &attrName)
        {
            if (attr.name() != attrName)
                return false;

            checkDuplicateAttr(str.isEmpty());

            str = attr.value().toString();
            if (str.isEmpty()) {
                qWarning() << filename << "line" << xml.lineNumber() << attrName << "attribute is empty";
                success = false;
            }

            return true;
        }

        //! Read a bool type attribute, \c sucess = \c false when \p xmlBool is not \c XmlBool::Unspecified.
        //! \return \c true when attr.name() == attrName, otherwise false
        bool extractXmlBool(XmlBool& xmlBool, const QString &attrName, bool alwaysTrue = true)
        {
            if (attr.name() != attrName)
                return false;

            checkDuplicateAttr(xmlBool == XmlBool::Unspecified);

            xmlBool = attr.value().isNull() ? XmlBool::Unspecified
                : attrToBool(attr.value()) ? XmlBool::True
                : XmlBool::False;

            if (alwaysTrue && xmlBool == XmlBool::False) {
                qWarning() << filename << "line" << xml.lineNumber() << attrName << "must be 1 or true:" << attr.value();
                success = false;
            }

            return true;
        }

        //! Read a positive integer type attribute, \c sucess = \c false when \p positive is already greater than or equal to 0
        //! \return \c true when attr.name() == attrName, otherwise false
        bool extractPositive(int& positive, const QString &attrName)
        {
            if (attr.name() != attrName)
                return false;

            checkDuplicateAttr(positive < 0);

            bool ok = true;
            positive = attr.value().toInt(&ok);

            if (!ok || positive < 0) {
                qWarning() << filename << "line" << xml.lineNumber() << attrName << "should be a positive integer:" << attr.value();
                success = false;
            }

            return true;
        }

        //! Read a color, \c sucess = \c false when \p color is already greater than or equal to 0
        //! \return \c true when attr.name() == attrName, otherwise false
        bool extractColor(qint64& color, const QString &attrName)
        {
            if (attr.name() != attrName)
                return false;

            checkDuplicateAttr(color < 0);

            const auto value = attr.value().toString();
            if (value.isEmpty() /*|| QColor(value).isValid()*/) {
                qWarning() << filename << "line" << xml.lineNumber() << attrName << "should be a color:" << attr.value();
                success = false;
            }

            return true;
        }

        //! Read a QChar, \c sucess = \c false when \p c is not \c '\0' or does not have one char
        //! \return \c true when attr.name() == attrName, otherwise false
        bool extractChar(QChar& c, const QString &attrName)
        {
            if (attr.name() != attrName)
                return false;

            checkDuplicateAttr(c == QLatin1Char('\0'));

            if (attr.value().size() == 1)
                c = attr.value()[0];
            else {
                c = QLatin1Char('_');
                qWarning() << filename << "line" << xml.lineNumber() << attrName << "must contain exactly one char:" << attr.value();
                success = false;
            }

            return true;
        }

        //! \return parsing status when \p isExtracted is \c true, otherwise \c false
        bool checkIfExtracted(bool isExtracted)
        {
            if (isExtracted)
                return success;

            qWarning() << filename << "line" << xml.lineNumber() << "unknown attribute:" << attr.name();
            return false;
        }

    private:
        void checkDuplicateAttr(bool isDuplicated)
        {
            if (!isDuplicated) {
                qWarning() << filename << "line" << xml.lineNumber() << "duplicate attribute:" << attr.name();
                success = false;
            }
        }
    };

    struct Keywords
    {
        struct Items
        {
            struct Item
            {
                QString content;
                int line;

                friend uint qHash(const Item &item, uint seed = 0)
                {
                    return qHash(item.content, seed);
                }

                friend bool operator==(const Item &item0, const Item &item1)
                {
                    return item0.content == item1.content;
                }
            };

            QVector<Item> keywords;
            QSet<Item> includes;

            bool parseElement(const QString &filename, QXmlStreamReader &xml)
            {
                bool success = true;

                const int line = xml.lineNumber();
                QString content = xml.readElementText();

                if (content.isEmpty()) {
                    qWarning() << filename << "line" << line << "is empty:" << xml.name();
                    success = false;
                }

                if (xml.name() == QStringLiteral("include")) {
                    includes.insert({content, line});
                }
                else if (xml.name() == QStringLiteral("item")) {
                    keywords.append({content, line});
                }
                else {
                    qWarning() << filename << "line" << line << "invalid element:" << xml.name();
                    success = false;
                }

                return success;
            }
        };

        QString name;
        Items items;
        int line;

        bool parseElement(const QString &filename, QXmlStreamReader &xml)
        {
            line = xml.lineNumber();

            bool success = true;
            for (auto& attr : xml.attributes()) {
                Parser parser{filename, xml, attr, success};

                const bool isExtracted = parser.extractString(name, QStringLiteral("name"));

                success = parser.checkIfExtracted(isExtracted);
            }
            return success;
        }
    };

    struct Context
    {
        struct Rule
        {
            enum class Type
            {
                Unknown,
                AnyChar,
                Detect2Chars,
                DetectChar,
                DetectIdentifier,
                DetectSpaces,
                Float,
                HlCChar,
                HlCHex,
                HlCOct,
                HlCStringChar,
                IncludeRules,
                Int,
                LineContinue,
                RangeDetect,
                RegExpr,
                StringDetect,
                WordDetect,
                keyword,
            };

            Type type {};
            int line;

            // commonAttributes
            QString attribute;
            ContextName context;
            QString beginRegion;
            QString endRegion;
            XmlBool lookhAhead {};
            XmlBool firstNonSpace {};
            int column = -1;

            // StringDetect, WordDetect, keyword
            XmlBool insensitive {};

            // DetectChar, StringDetect, RegExpr, keyword
            XmlBool dynamic {};

            // Regex
            XmlBool minimal {};

            // DetectChar, Detect2Chars, LineContinue, RangeDetect
            QChar char0;
            // Detect2Chars, RangeDetect
            QChar char1;

            // AnyChar, DetectChar, StringDetect, RegExpr, WordDetect, keyword
            QString string;

            // included by IncludeRules
            QVector<const Rule*> includedRules;

            bool parseElement(const QString &filename, QXmlStreamReader &xml)
            {
                line = xml.lineNumber();

                using Pair = QPair<QString, Type>;
                static const auto pairs = {
                    Pair{QStringLiteral("AnyChar"), Type::AnyChar},
                    Pair{QStringLiteral("Detect2Chars"), Type::Detect2Chars},
                    Pair{QStringLiteral("DetectChar"), Type::DetectChar},
                    Pair{QStringLiteral("DetectIdentifier"), Type::DetectIdentifier},
                    Pair{QStringLiteral("DetectSpaces"), Type::DetectSpaces},
                    Pair{QStringLiteral("Float"), Type::Float},
                    Pair{QStringLiteral("HlCChar"), Type::HlCChar},
                    Pair{QStringLiteral("HlCHex"), Type::HlCHex},
                    Pair{QStringLiteral("HlCOct"), Type::HlCOct},
                    Pair{QStringLiteral("HlCStringChar"), Type::HlCStringChar},
                    Pair{QStringLiteral("IncludeRules"), Type::IncludeRules},
                    Pair{QStringLiteral("Int"), Type::Int},
                    Pair{QStringLiteral("LineContinue"), Type::LineContinue},
                    Pair{QStringLiteral("RangeDetect"), Type::RangeDetect},
                    Pair{QStringLiteral("RegExpr"), Type::RegExpr},
                    Pair{QStringLiteral("StringDetect"), Type::StringDetect},
                    Pair{QStringLiteral("WordDetect"), Type::WordDetect},
                    Pair{QStringLiteral("keyword"), Type::keyword},
                };

                for (auto pair : pairs) {
                    if (xml.name() == pair.first) {
                        type = pair.second;
                        bool success = parseAttributes(filename, xml);
                        success = checkMandoryAttributes(filename, xml) && success;
                        return success;
                    }
                }

                qWarning() << filename << "line" << xml.lineNumber() << "unknown element:" << xml.name();
                return false;
            }

        private:
            bool parseAttributes(const QString &filename, QXmlStreamReader &xml)
            {
                bool success = true;

                for (auto& attr : xml.attributes()) {
                    Parser parser{filename, xml, attr, success};
                    XmlBool includeAttrib{};

                    const bool isExtracted
                      = parser.extractString(attribute, QStringLiteral("attribute"))
                     || parser.extractString(context.name, QStringLiteral("context"))
                     || parser.extractXmlBool(lookhAhead, QStringLiteral("lookAhead"))
                     || parser.extractXmlBool(firstNonSpace, QStringLiteral("firstNonSpace"))
                     || parser.extractString(beginRegion, QStringLiteral("beginRegion"))
                     || parser.extractString(endRegion, QStringLiteral("endRegion"))
                     || parser.extractPositive(column, QStringLiteral("column"))
                     || ((type == Type::RegExpr
                       || type == Type::StringDetect
                       || type == Type::WordDetect
                       || type == Type::keyword
                       ) && parser.extractXmlBool(insensitive, QStringLiteral("insensitive"), type != Type::keyword))
                     || ((type == Type::DetectChar
                       || type == Type::RegExpr
                       || type == Type::StringDetect
                       || type == Type::keyword
                       ) && parser.extractXmlBool(dynamic, QStringLiteral("dynamic")))
                     || ((type == Type::RegExpr
                       ) && parser.extractXmlBool(minimal, QStringLiteral("minimal")))
                     || ((type == Type::DetectChar
                       || type == Type::Detect2Chars
                       || type == Type::LineContinue
                       || type == Type::RangeDetect
                       ) && parser.extractChar(char0, QStringLiteral("char")))
                     || ((type == Type::Detect2Chars
                       || type == Type::RangeDetect
                       ) && parser.extractChar(char1, QStringLiteral("char1")))
                     || ((type == Type::AnyChar
                       || type == Type::RegExpr
                       || type == Type::StringDetect
                       || type == Type::WordDetect
                       || type == Type::keyword
                       ) && parser.extractString(string, QStringLiteral("String")))
                     || ((type == Type::IncludeRules
                       ) && parser.extractXmlBool(includeAttrib, QStringLiteral("includeAttrib")))
                    ;

                    success = parser.checkIfExtracted(isExtracted);
                }

                return success;
            }

            bool checkMandoryAttributes(const QString &filename, QXmlStreamReader &xml)
            {
                QString missingAttr;

                switch (type) {
                    case Type::Unknown:
                        return false;

                    case Type::AnyChar:
                    case Type::RegExpr:
                    case Type::StringDetect:
                    case Type::WordDetect:
                    case Type::keyword:
                        missingAttr = string.isEmpty() ? QStringLiteral("String") : QString();
                        break;

                    case Type::DetectChar:
                        missingAttr = !char0.unicode() ? QStringLiteral("char") : QString();
                        break;

                    case Type::Detect2Chars:
                    case Type::RangeDetect:
                        missingAttr
                            = !char0.unicode() && !char1.unicode() ? QStringLiteral("char and char1")
                            : !char0.unicode() ? QStringLiteral("char")
                            : !char1.unicode() ? QStringLiteral("char1")
                            : QString();
                        break;

                    case Type::IncludeRules:
                        missingAttr = context.name.isEmpty() ? QStringLiteral("context") : QString();
                        break;

                    case Type::DetectIdentifier:
                    case Type::DetectSpaces:
                    case Type::Float:
                    case Type::HlCChar:
                    case Type::HlCHex:
                    case Type::HlCOct:
                    case Type::HlCStringChar:
                    case Type::Int:
                    case Type::LineContinue:
                        break;
                }

                if (!missingAttr.isEmpty()) {
                    qWarning() << filename << "line" << xml.lineNumber() << "missing attribute:" << missingAttr;
                    return false;
                }

                return true;
            }
        };

        int line;
        QString name;
        QString attribute;
        ContextName lineEndContext;
        ContextName lineEmptyContext;
        ContextName fallthroughContext;
        QVector<Rule> rules;
        XmlBool dynamic {};
        XmlBool fallthrough {};

        bool parseElement(const QString &filename, QXmlStreamReader &xml)
        {
            line = xml.lineNumber();

            bool success = true;

            for (auto& attr : xml.attributes()) {
                Parser parser{filename, xml, attr, success};
                XmlBool noIndentationBasedFolding{};

                const bool isExtracted
                  = parser.extractString(name, QStringLiteral("name"))
                 || parser.extractString(attribute, QStringLiteral("attribute"))
                 || parser.extractString(lineEndContext.name, QStringLiteral("lineEndContext"))
                 || parser.extractString(lineEmptyContext.name, QStringLiteral("lineEmptyContext"))
                 || parser.extractString(fallthroughContext.name, QStringLiteral("fallthroughContext"))
                 || parser.extractXmlBool(dynamic, QStringLiteral("dynamic"))
                 || parser.extractXmlBool(fallthrough, QStringLiteral("fallthrough"))
                 || parser.extractXmlBool(noIndentationBasedFolding, QStringLiteral("noIndentationBasedFolding"))
                ;

                success = parser.checkIfExtracted(isExtracted);
            }

            if (name.isEmpty()) {
                qWarning() << filename << "line" << xml.lineNumber() << "missing attribute: name";
                success = false;
            }

            if (attribute.isEmpty()) {
                qWarning() << filename << "line" << xml.lineNumber() << "missing attribute: attribute";
                success = false;
            }

            if (lineEndContext.name.isEmpty()) {
                qWarning() << filename << "line" << xml.lineNumber() << "missing attribute: lineEndContext";
                success = false;
            }

            return success;
        }
    };

    struct Version
    {
        int majorRevision;
        int minorRevision;

        Version(int majorRevision = 0, int minorRevision = 0)
            : majorRevision(majorRevision)
            , minorRevision(minorRevision)
        {
        }

        bool operator<(const Version &version) const
        {
            return majorRevision < version.majorRevision || (majorRevision == version.majorRevision && minorRevision < version.minorRevision);
        }
    };

    struct ItemDatas
    {
        struct Style
        {
            QString name;
            int line;

            friend uint qHash(const Style &style, uint seed = 0)
            {
                return qHash(style.name, seed);
            }

            friend bool operator==(const Style &style0, const Style &style1)
            {
                return style0.name == style1.name;
            }
        };

        QSet<Style> styleNames;

        bool parseElement(const QString &filename, QXmlStreamReader &xml)
        {
            bool success = true;

            QString name;
            QString defStyleNum;
            XmlBool bold {};
            XmlBool italic {};
            XmlBool underline {};
            XmlBool strikeOut {};
            XmlBool spellChecking {};
            qint64 color = -1;
            qint64 selColor = -1;
            qint64 BackgroundColor = -1;
            qint64 selBackgroundColor = -1;

            for (auto& attr : xml.attributes()) {
                Parser parser{filename, xml, attr, success};

                const bool isExtracted
                    = parser.extractString(name, QStringLiteral("name"))
                   || parser.extractString(defStyleNum, QStringLiteral("defStyleNum"))
                   || parser.extractXmlBool(bold, QStringLiteral("bold"), false)
                   || parser.extractXmlBool(italic, QStringLiteral("italic"), false)
                   || parser.extractXmlBool(underline, QStringLiteral("underline"), false)
                   || parser.extractXmlBool(strikeOut, QStringLiteral("strikeOut"), false)
                   || parser.extractXmlBool(spellChecking, QStringLiteral("spellChecking"), false)
                   || parser.extractColor(color, QStringLiteral("color"))
                   || parser.extractColor(selColor, QStringLiteral("selColor"))
                   || parser.extractColor(BackgroundColor, QStringLiteral("BackgroundColor"))
                   || parser.extractColor(selBackgroundColor, QStringLiteral("selBackgroundColor"))
                ;

                success = parser.checkIfExtracted(isExtracted);
            }

            if (!name.isEmpty()) {
                const auto len = styleNames.size();
                styleNames.insert({name, int(xml.lineNumber())});
                if (len == styleNames.size()) {
                    qWarning() << filename << "line" << xml.lineNumber() << "itemData duplicate:" << name;
                    success = false;
                }
            }

            return success;
        }
    };

    struct Definition
    {
        QMap<QString, Keywords> keywordsList;
        QMap<QString, Context> contexts;
        ItemDatas itemDatas;
        QString firstContextName;
        const Context *firstContext = nullptr;
        QString filename;
        WordDelimiters wordDelimiters;
        XmlBool casesensitive {};
        Version kateVersion {};
        QString kateVersionStr;
        QString languageName;
        QSet<const Definition*> referencedDefinitions;

        bool parseKeywords(QXmlStreamReader &xml)
        {
            for (QChar c : xml.attributes().value(QStringLiteral("additionalDeliminator")))
                wordDelimiters.append(c);
            for (QChar c : xml.attributes().value(QStringLiteral("weakDeliminator")))
                wordDelimiters.remove(c);
            return true;
        }
    };

    void processContextElement(QXmlStreamReader &xml)
    {
        Context context;
        m_success = context.parseElement(m_currentDefinition->filename, xml) && m_success;
        if (m_currentDefinition->firstContextName.isEmpty())
            m_currentDefinition->firstContextName = context.name;
        if (m_currentDefinition->contexts.contains(context.name)) {
            qWarning() << m_currentDefinition->filename << "line" << xml.lineNumber() << "duplicate context:" << context.name;
            m_success = false;
        }
        m_currentContext = &*m_currentDefinition->contexts.insert(context.name, context);
    }

    void processListElement(QXmlStreamReader &xml)
    {
        Keywords keywords;
        m_success = keywords.parseElement(m_currentDefinition->filename, xml) && m_success;
        if (m_currentDefinition->keywordsList.contains(keywords.name)) {
            qWarning() << m_currentDefinition->filename << "line" << xml.lineNumber() << "duplicate list:" << keywords.name;
            m_success = false;
        }
        m_currentKeywords = &*m_currentDefinition->keywordsList.insert(keywords.name, keywords);
    }

    bool checkKateVersion(const Definition &definition) const
    {
        auto* maxDef = &definition;
        for (const auto &referencedDef : definition.referencedDefinitions) {
            if (maxDef->kateVersion < referencedDef->kateVersion) {
                maxDef = referencedDef;
            }
        }

        if (maxDef != &definition) {
            qWarning() << definition.filename << "depends on a language" << maxDef->languageName << "in version" << maxDef->kateVersionStr << ". Please, increase kateversion.";
            return false;
        }
        return true;
    }

    void resolveIncludeRules()
    {
        QSet<const Context*> usedContexts;
        QVector<const Context*> contexts;

        QMutableMapIterator<QString, Definition> def(m_definitions);
        while (def.hasNext()) {
            def.next();
            auto &definition = def.value();
            QMutableMapIterator<QString, Context> contextIt(definition.contexts);
            while (contextIt.hasNext()) {
                contextIt.next();
                for (auto &rule : contextIt.value().rules) {
                    if (rule.type != Context::Rule::Type::IncludeRules) {
                        continue;
                    }

                    if (rule.context.stay) {
                        qWarning() << definition.filename << "line" << rule.line << "IncludeRules refers to himself";
                        m_success = false;
                        continue;
                    }
                    if (rule.context.popCount) {
                        qWarning() << definition.filename << "line" << rule.line << "IncludeRules with #pop prefix";
                        m_success = false;
                    }

                    if (!rule.context.context) {
                        m_success = false;
                        continue;
                    }

                    usedContexts.clear();
                    usedContexts.insert(rule.context.context);
                    contexts.clear();
                    contexts.append(rule.context.context);

                    for (int i = 0; i < contexts.size(); ++i) {
                        for (const auto &includedRule : contexts[i]->rules) {
                            if (includedRule.type != Context::Rule::Type::IncludeRules) {
                                rule.includedRules.append(&includedRule);
                            }
                            else if (&rule == &includedRule) {
                                qWarning() << definition.filename << "line" << rule.line << "IncludeRules refers to himself by recursivity";
                                m_success = false;
                            }
                            else if (includedRule.includedRules.isEmpty()) {
                                const auto &context = includedRule.context.context;
                                if (context && !usedContexts.contains(context)) {
                                    contexts.append(context);
                                    usedContexts.insert(context);
                                }
                            }
                            else {
                                rule.includedRules.append(includedRule.includedRules);
                            }
                        }
                    }
                }
            }
        }
    }

    //! Recursively extracts the contexts used from the first context of the definitions
    QSet<const Context*> extractUsedContexts() const
    {
        QSet<const Context*> usedContexts;
        QVector<const Context*> contexts;

        QMapIterator<QString, Definition> def(m_definitions);
        while (def.hasNext()) {
            def.next();
            const auto &definition = def.value();

            if (definition.firstContext) {
                usedContexts.insert(definition.firstContext);
                contexts.clear();
                contexts.append(definition.firstContext);

                for (int i = 0; i < contexts.size(); ++i) {
                    auto appendContext = [&](const Context *context){
                        if (context && !usedContexts.contains(context)) {
                            contexts.append(context);
                            usedContexts.insert(context);
                        }
                    };

                    const auto *context = contexts[i];
                    appendContext(context->lineEndContext.context);
                    appendContext(context->lineEmptyContext.context);
                    appendContext(context->fallthroughContext.context);

                    for (auto &rule : context->rules) {
                        appendContext(rule.context.context);
                    }
                }
            }
        }

        return usedContexts;
    }

    bool checkContexts(const Definition &definition, QSet<const Keywords*> &referencedKeywords, QSet<ItemDatas::Style> &usedAttributeNames, const QSet<const Context*> &usedContexts) const
    {
        bool success = true;

        QMapIterator<QString, Context> contextIt(definition.contexts);
        while (contextIt.hasNext()) {
            contextIt.next();

            const auto &context = contextIt.value();
            const auto &filename = definition.filename;

            if (!usedContexts.contains(&context)) {
                qWarning() << filename << "line" << context.line << "unused context:" << context.name;
                success = false;
                continue;
            }

            if (!context.attribute.isEmpty())
                usedAttributeNames.insert({context.attribute, context.line});

            success = checkfallthrough(definition, context) && success;

            for (const auto& rule : context.rules) {
                if (!rule.attribute.isEmpty())
                    usedAttributeNames.insert({rule.attribute, rule.line});
                success = checkLookAhead(filename, rule) && success;
                success = checkStringDetect(filename, rule) && success;
                success = checkKeyword(definition, rule, referencedKeywords) && success;
                success = checkRegExpr(filename, rule) && success;
                success = checkWordDetect(definition, rule) && success;
            }
        }

        return success;
    }

    bool checkWordDetect(const Definition &definition, const Context::Rule &rule) const
    {
        if (rule.type == Context::Rule::Type::WordDetect && !rule.string.isEmpty()) {
            bool isDelimiter = definition.wordDelimiters.contains(rule.string[0]);
            if (isDelimiter || definition.wordDelimiters.contains(rule.string[rule.string.size()-1])) {
                qWarning() << definition.filename << "line" << rule.line << "WordDetect with delimiter:" << rule.string[(isDelimiter ? 0 : rule.string.size()-1)];
                return false;
            }
        }
        return true;
    }

    //! Check that a regular expression in a RegExpr rule:
    //! - isValid()
    //! - character ranges such as [A-Z] are valid and not accidentally e.g. [A-z].
    //! - dynamic=true but no place holder used?
    //! - is not . with lookAhead="1"
    //! - is not equivalent to DetectSpaces or StringDetect
    bool checkRegExpr(const QString &filename, const Context::Rule &rule) const
    {
        if (rule.type == Context::Rule::Type::RegExpr) {
            if (!checkRegularExpression(filename, rule.string, rule.line)) {
                return false;
            }

            // remove "(?:" and ")" to simplify the following checks
            static const QRegularExpression sanitize(QStringLiteral(R"(\(\?:|\))"));
            auto reg = rule.string;
            reg.replace(sanitize, QString());

            // dynamic == true and no place holder?
            if (rule.dynamic == XmlBool::True) {
                static const QRegularExpression placeHolder(QStringLiteral("%\\d+"));
                if (!rule.string.contains(placeHolder)) {
                    qWarning() << filename << "line" << rule.line << "broken regex:" << rule.string << "problem: dynamic=true but no %\\d+ placeholder";
                    return false;
                }
            }

            // . + lookAhead should be fallthroughContext="..."
            static const QRegularExpression isDot(QStringLiteral(
                R"(^\.[*+]?\$?$)"));
            if (rule.lookhAhead == XmlBool::True
                && reg.contains(isDot)
                && rule.beginRegion.isEmpty()
                && rule.endRegion.isEmpty()
                && rule.column == -1
                && rule.firstNonSpace != XmlBool::True
            ) {
                qWarning() << filename << "line" << rule.line << "regex should be replaced by fallthroughContext:" << rule.string;
                return false;
            }

            // is DetectSpaces
            static const QRegularExpression isDetectSpaces(QStringLiteral(
                R"(^\^?(\\s|\[ \\t\]|\[\\t \])[*+]$)"));
            if (reg.contains(isDetectSpaces)) {
                qWarning() << filename << "line" << rule.line << "regex should be replaced by DetectSpaces:" << rule.string;
                return false;
            }

            // is StringDetect
            static const QRegularExpression isStringDetect(QStringLiteral(
                R"(^\^?(?:\\[^0BDPSWbdpswoux]|\\x[0-9a-fA-F]{2}|\\x\{[0-9a-fA-F]+\}|\\0\d\d|\\o\{[0-7]+\}|\\u[0-9a-fA-F]{4}|\[[^\\]{2}\]|\[(?:\\[*+?.|^$[\]{}()\\]|.)\]|\(\?:|[^*+?.|^$[{(\\]+?)*$)"));
            // check [eE][tT][cC]
            static const QRegularExpression isInsensitive(QStringLiteral(
                R"(^\^?(?:\\.|\[(.)\1\]|[^[\\]+?)*$)"));
            if (reg.contains(isStringDetect) && reg.toUpper().contains(isInsensitive)) {
                qWarning() << filename << "line" << rule.line << "regex should be replaced by StringDetect / Detect2Chars / DetectChar:" << rule.string;
                return false;
            }

            // column="0" or firstNonSpace="1"
            if (rule.column == -1 && rule.firstNonSpace != XmlBool::True
             && reg.startsWith(QLatin1Char('^')) && !reg.contains(QLatin1Char('|'))
            ) {
                qWarning() << filename << "line" << rule.line << "column=\"0\" or firstNonSpace=\"1\" missing with RegExpr:" << rule.string;
                return false;
            }

            // capture does not refer to any dynamic rules
            static const QRegularExpression hasCapture(QStringLiteral(
                R"(^\^?(?:\\.|\[(?:\\.|\[(?:[^]\\]++|\\.)*\]|[^[\]\\]++)\]|\((?=[?])|[^[\\(]+?)*$)"));
            if (rule.context.context && !reg.contains(hasCapture)) {
                bool isDynamic = false;
                for (const auto &nextRule : rule.context.context->rules) {
                    if (nextRule.dynamic == XmlBool::True) {
                        isDynamic = true;
                        break;
                    }
                    else {
                        for (const auto &includedRule : nextRule.includedRules) {
                            if (includedRule->dynamic == XmlBool::True) {
                                isDynamic = true;
                                break;
                            }
                        }
                        if (isDynamic) {
                            break;
                        }
                    }
                }

                if (!isDynamic) {
                    qWarning() << filename << "line" << rule.line << "regex with a capture, but does not refer to any dynamic rules. Please, replace '(...)' with '(?:...)':" << rule.string;
                    return false;
                }
            }
        }

        return true;
    }

    bool parseEmptyLine(const QString &filename, QXmlStreamReader &xml)
    {
        bool success = true;

        QString regexpr;
        XmlBool casesensitive {};

        for (auto& attr : xml.attributes()) {
            Parser parser{filename, xml, attr, success};

            const bool isExtracted
                = parser.extractString(regexpr, QStringLiteral("regexpr"))
               || parser.extractXmlBool(casesensitive, QStringLiteral("casesensitive"))
            ;

            success = parser.checkIfExtracted(isExtracted);
        }

        if (regexpr.isEmpty()) {
            qWarning() << filename << "line" << xml.lineNumber() << "missing attribute: regexpr";
            success = false;
        }
        else {
            success = checkRegularExpression(filename, regexpr, xml.lineNumber());
        }

        return success;
    }

    //! Check that a regular expression:
    //! - isValid()
    //! - character ranges such as [A-Z] are valid and not accidentally e.g. [A-z].
    bool checkRegularExpression(const QString &filename, const QString& string, int line) const
    {
        // validate regexp
        const QRegularExpression regexp(string);
        if (!regexp.isValid()) {
            qWarning() << filename << "line" << line << "broken regex:" << string << "problem:" << regexp.errorString() << "at offset" << regexp.patternErrorOffset();
            return false;
        }

        // catch possible case typos: [A-z] or [a-Z]
        const int azOffset = std::max(string.indexOf(QStringLiteral("A-z")), string.indexOf(QStringLiteral("a-Z")));
        if (azOffset >= 0) {
            qWarning() << filename << "line" << line << "broken regex:" << string << "problem: [a-Z] or [A-z] at offset" << azOffset;
            return false;
        }

        return true;
    }

    //! Search for rules with lookAhead="true" and context="#stay".
    //! This would cause an infinite loop.
    bool checkfallthrough(const Definition &definition, const Context &context) const
    {
        bool success = true;

        if (!context.fallthroughContext.name.isEmpty()) {
            if (context.fallthroughContext.stay) {
                qWarning() << definition.filename << "line" << context.line << "possible infinite loop due to fallthroughContext=\"#stay\" in context " << context.name;
                success = false;
            }

            if (context.fallthrough != XmlBool::True && definition.kateVersion < Version{5, 62}) {
                qWarning() << definition.filename << "line" << context.line << "fallthroughContext attribute without fallthrough attribute is only valid with kateversion >= 5.62 in context" << context.name;
                success = false;
            }
        }

        return success;
    }

    //! Search for rules with lookAhead="true" and context="#stay".
    //! This would cause an infinite loop.
    bool checkKeyword(const Definition &definition, const Context::Rule &rule, QSet<const Keywords*> &referencedKeywords) const
    {
        if (rule.type == Context::Rule::Type::keyword) {
            auto it = definition.keywordsList.find(rule.string);
            if (it != definition.keywordsList.end()) {
                referencedKeywords.insert(&*it);
            }
            else {
                qWarning() << definition.filename << "line" << rule.line << "reference of non-existing keyword list:" << rule.string;
                return false;
            }
        }
        return true;
    }

    //! Search for rules with lookAhead="true" and context="#stay".
    //! This would cause an infinite loop.
    bool checkLookAhead(const QString &filename, const Context::Rule &rule) const
    {
        if (rule.lookhAhead == XmlBool::True && rule.context.stay) {
            qWarning() << filename << "line" << rule.line << "infinite loop: lookAhead with context #stay";
        }
        return true;
    }

    //! Check that StringDetect contains more that 2 characters
    //! Fix with following command:
    //! \code
    //!   sed -E '/StringDetect/{/dynamic="(1|true)|insensitive="(1|true)/!{s/StringDetect(.*)String="(.|&lt;|&gt;|&quot;|&amp;)(.|&lt;|&gt;|&quot;|&amp;)"/Detect2Chars\1char="\2" char1="\3"/;t;s/StringDetect(.*)String="(.|&lt;|&gt;|&quot;|&amp;)"/DetectChar\1char="\2"/}}' -i file.xml...
    //! \endcode
    bool checkStringDetect(const QString &filename, const Context::Rule &rule) const
    {
        if (rule.type == Context::Rule::Type::StringDetect
         && rule.string.size() <= 2
         && rule.dynamic != XmlBool::True
         && rule.insensitive != XmlBool::True
        ) {
            qWarning() << filename << "line" << rule.line << "StringDetect should be replaced by" << (rule.string.size() == 2 ? "Detect2Chars" : "DetectChar");
            return false;
        }
        return true;
    }

    bool checkKeywordsList(const Definition &definition, QSet<const Keywords*> &referencedKeywords) const
    {
        bool success = true;

        bool includeSupport = !(definition.kateVersion < Version{5, 53});
        QMapIterator<QString, Keywords> keywordsIt(definition.keywordsList);
        while (keywordsIt.hasNext()) {
            keywordsIt.next();

            for (const auto& include : keywordsIt.value().items.includes) {
                if (includeSupport) {
                    qWarning() << definition.filename << "line" << include.line << "<include> is only available since version \"5.53\". Please, increase kateversion.";
                    success = false;
                }
                success = checkKeywordInclude(definition, include, referencedKeywords) && success;
            }

            // Check that keyword list items do not have deliminator character
            for (const auto& keyword : keywordsIt.value().items.keywords) {
                for (QChar c : keyword.content) {
                    if (definition.wordDelimiters.contains(c)) {
                        qWarning() << definition.filename << "line" << keyword.line << "keyword with delimiter:" << c;
                        success = false;
                    }
                }
            }
        }

        return success;
    }

    //! search for non-existing keyword include.
    bool checkKeywordInclude(const Definition &definition, const Keywords::Items::Item &include, QSet<const Keywords*> &referencedKeywords) const
    {
        bool containsKeywordName = true;
        int const idx = include.content.indexOf(QStringLiteral("##"));
        if (idx == -1) {
            auto it = definition.keywordsList.find(include.content);
            containsKeywordName = (it != definition.keywordsList.end());
            if (containsKeywordName) {
                referencedKeywords.insert(&*it);
            }
        } else {
            auto defName = include.content.mid(idx + 2);
            auto listName = include.content.left(idx);
            auto it = m_definitions.find(defName);
            if (it == m_definitions.end()) {
                qWarning() << definition.filename << "line" << include.line << "unknown definition in" << include.content;
                return false;
            }
            containsKeywordName = it->keywordsList.contains(listName);
        }

        if (!containsKeywordName) {
            qWarning() << definition.filename << "line" << include.line << "unknown keyword name in" << include.content;
        }

        return containsKeywordName;
    }

    //! Initialize the referenced context (ContextName::context)
    //! Some input / output examples are:
    //! - "#stay"         -> ""
    //! - "#pop"          -> ""
    //! - "Comment"       -> "Comment"
    //! - "#pop!Comment"  -> "Comment"
    //! - "##ISO C++"     -> ""
    //! - "Comment##ISO C++"-> "Comment" in ISO C++
    void resolveContextName(Definition &definition, ContextName &context, int line)
    {
        QStringView name = context.name;
        if (name.isEmpty()) {
            context.stay = true;
        }
        else if (name.startsWith(QStringLiteral("#stay"))) {
            name = name.mid(5);
            context.stay = true;
            if (!name.isEmpty()) {
                qWarning() << definition.filename << "line" << line << "invalid context in" << context.name;
                m_success = false;
            }
        }
        else {
            while (name.startsWith(QStringLiteral("#pop"))) {
                name = name.mid(4);
                ++context.popCount;
            }

            if (context.popCount && !name.isEmpty()) {
                if (name.startsWith(QLatin1Char('!')) && name.size() > 1) {
                    name = name.mid(1);
                }
                else {
                    qWarning() << definition.filename << "line" << line << "invalid context in" << context.name;
                    m_success = false;
                }
            }

            if (!name.isEmpty()) {
                const int idx = name.indexOf(QStringLiteral("##"));
                if (idx == -1) {
                    auto it = definition.contexts.find(name.toString());
                    if (it != definition.contexts.end())
                        context.context = &*it;
                } else {
                    auto defName = name.mid(idx + 2);
                    auto listName = name.left(idx);
                    auto it = m_definitions.find(defName.toString());
                    if (it != m_definitions.end()) {
                        definition.referencedDefinitions.insert(&*it);
                        auto ctxIt = it->contexts.find(listName.isEmpty() ? it->firstContextName : listName.toString());
                        if (ctxIt != it->contexts.end()) {
                            context.context = &*ctxIt;
                        }
                    } else {
                        qWarning() << definition.filename << "line" << line << "unknown definition in" << context.name;
                        m_success = false;
                    }
                }

                if (!context.context) {
                    qWarning() << definition.filename << "line" << line << "unknown context in" << context.name;
                    m_success = false;
                }
            }
        }
    }

    QMap<QString, Definition> m_definitions;
    Definition *m_currentDefinition = nullptr;
    Keywords *m_currentKeywords = nullptr;
    Context *m_currentContext = nullptr;
    bool m_success = true;
};

namespace
{
QStringList readListing(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        return QStringList();
    }

    QXmlStreamReader xml(&file);
    QStringList listing;
    while (!xml.atEnd()) {
        xml.readNext();

        // add only .xml files, no .json or stuff
        if (xml.isCharacters() && xml.text().toString().contains(QLatin1String(".xml"))) {
            listing.append(xml.text().toString());
        }
    }

    if (xml.hasError()) {
        qWarning() << "XML error while reading" << fileName << " - " << qPrintable(xml.errorString()) << "@ offset" << xml.characterOffset();
        listing.clear();
    }

    return listing;
}

/**
 * check if the "extensions" attribute have valid wildcards
 * @param extensions extensions string to check
 * @return valid?
 */
bool checkExtensions(const QString &extensions)
{
    // get list of extensions
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    const QStringList extensionParts = extensions.split(QLatin1Char(';'), QString::SkipEmptyParts);
#else
    const QStringList extensionParts = extensions.split(QLatin1Char(';'), Qt::SkipEmptyParts);
#endif

    // ok if empty
    if (extensionParts.isEmpty()) {
        return true;
    }

    // check that only valid wildcard things are inside the parts
    for (const auto &extension : extensionParts) {
        for (const auto c : extension) {
            // eat normal things
            if (c.isDigit() || c.isLetter()) {
                continue;
            }

            // allow some special characters
            if (c == QLatin1Char('.') || c == QLatin1Char('-') || c == QLatin1Char('_') || c == QLatin1Char('+')) {
                continue;
            }

            // only allowed wildcard things: '?' and '*'
            if (c == QLatin1Char('?') || c == QLatin1Char('*')) {
                continue;
            }

            qWarning() << "invalid character" << c << "seen in extensions wildcard";
            return false;
        }
    }

    // all checks passed
    return true;
}

}

int main(int argc, char *argv[])
{
    // get app instance
    QCoreApplication app(argc, argv);

    // ensure enough arguments are passed
    if (app.arguments().size() < 3)
        return 1;

#ifdef QT_XMLPATTERNS_LIB
    // open schema
    QXmlSchema schema;
    if (!schema.load(QUrl::fromLocalFile(app.arguments().at(2))))
        return 2;
#endif

    const QString hlFilenamesListing = app.arguments().value(3);
    if (hlFilenamesListing.isEmpty()) {
        return 1;
    }

    QStringList hlFilenames = readListing(hlFilenamesListing);
    if (hlFilenames.isEmpty()) {
        qWarning("Failed to read %s", qPrintable(hlFilenamesListing));
        return 3;
    }

    // text attributes
    const QStringList textAttributes = QStringList()
        << QStringLiteral("name")
        << QStringLiteral("section")
        << QStringLiteral("mimetype")
        << QStringLiteral("extensions")
        << QStringLiteral("style")
        << QStringLiteral("author")
        << QStringLiteral("license")
        << QStringLiteral("indenter");

    // index all given highlightings
    HlFilesChecker filesChecker;
    QVariantMap hls;
    int anyError = 0;
    for (const QString &hlFilename : qAsConst(hlFilenames)) {
        QFile hlFile(hlFilename);
        if (!hlFile.open(QIODevice::ReadOnly)) {
            qWarning("Failed to open %s", qPrintable(hlFilename));
            anyError = 3;
            continue;
        }

#ifdef QT_XMLPATTERNS_LIB
        // validate against schema
        QXmlSchemaValidator validator(schema);
        if (!validator.validate(&hlFile, QUrl::fromLocalFile(hlFile.fileName()))) {
            anyError = 4;
            continue;
        }
#endif

        // read the needed attributes from toplevel language tag
        hlFile.reset();
        QXmlStreamReader xml(&hlFile);
        if (xml.readNextStartElement()) {
            if (xml.name() != QLatin1String("language")) {
                anyError = 5;
                continue;
            }
        } else {
            anyError = 6;
            continue;
        }

        // map to store hl info
        QVariantMap hl;

        // transfer text attributes
        for (const QString &attribute : qAsConst(textAttributes)) {
            hl[attribute] = xml.attributes().value(attribute).toString();
        }

        // check if extensions have the right format
        if (!checkExtensions(hl[QStringLiteral("extensions")].toString())) {
            qWarning() << hlFilename << "'extensions' wildcards invalid:" << hl[QStringLiteral("extensions")].toString();
            anyError = 23;
        }

        // numerical attributes
        hl[QStringLiteral("version")] = xml.attributes().value(QLatin1String("version")).toInt();
        hl[QStringLiteral("priority")] = xml.attributes().value(QLatin1String("priority")).toInt();

        // add boolean one
        hl[QStringLiteral("hidden")] = attrToBool(xml.attributes().value(QLatin1String("hidden")));

        // remember hl
        hls[QFileInfo(hlFile).fileName()] = hl;

        const QString hlName = hl[QStringLiteral("name")].toString();

        filesChecker.setDefinition(xml.attributes().value(QStringLiteral("kateversion")), hlFilename, hlName);

        // scan for broken regex or keywords with spaces
        while (!xml.atEnd()) {
            xml.readNext();
            filesChecker.processElement(xml);
        }
    }

    filesChecker.resolveContexts();

    if (!filesChecker.check())
        anyError = 7;

    // bail out if any problem was seen
    if (anyError)
        return anyError;

    // create outfile, after all has worked!
    QFile outFile(app.arguments().at(1));
    if (!outFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return 9;

    // write out json
    outFile.write(QCborValue::fromVariant(QVariant(hls)).toCbor());

    // be done
    return 0;
}
