/* This file is part of the KDE libraries
   Copyright (C) 2014 Christoph Cullmann <cullmann@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QVariant>
#include <QXmlStreamReader>
#include <QJsonDocument>
#include <QRegularExpression>
#include <QDebug>

#ifdef QT_XMLPATTERNS_LIB
#include <QXmlSchema>
#include <QXmlSchemaValidator>
#endif

namespace {

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
        qWarning() << "XML error while reading" << fileName << " - "
            << qPrintable(xml.errorString()) << "@ offset" << xml.characterOffset();
    }

    return listing;
}

//! Check that a regular expression in a RegExpr rule:
//! - is not empty
//! - isValid()
//! - character ranges such as [A-Z] are valid and not accidentally e.g. [A-z].
bool checkRegularExpression(const QString &hlFilename, QXmlStreamReader &xml)
{
    if (xml.name() == QLatin1String("RegExpr") || xml.name() == QLatin1String("emptyLine")) {
        // get right attribute
        const QString string (xml.attributes().value((xml.name() == QLatin1String("RegExpr")) ? QLatin1String("String") : QLatin1String("regexpr")).toString());

        // validate regexp
        const QRegularExpression regexp (string);
        if (!regexp.isValid()) {
            qWarning() << hlFilename << "line" << xml.lineNumber() << "broken regex:" << string << "problem:" << regexp.errorString() << "at offset" << regexp.patternErrorOffset();
            return false;
        } else if (string.isEmpty()) {
            qWarning() << hlFilename << "line" << xml.lineNumber() << "empty regex not allowed.";
            return false;
        }

        // catch possible case typos: [A-z] or [a-Z]
        const int azOffset = std::max(string.indexOf(QStringLiteral("A-z")), string.indexOf(QStringLiteral("a-Z")));
        if (azOffset >= 0) {
            qWarning() << hlFilename << "line" << xml.lineNumber() << "broken regex:" << string << "problem: [a-Z] or [A-z] at offset" << azOffset;
            return false;
        }
    }

    return true;
}

//! Check that keyword list items do not have trailing or leading spaces,
//! e.g.: <item> keyword </item>
bool checkItemsTrimmed(const QString &hlFilename, QXmlStreamReader &xml)
{
    if (xml.name() == QLatin1String("item")) {
        const QString keyword = xml.readElementText();
        if (keyword != keyword.trimmed()) {
            qWarning() << hlFilename << "line" << xml.lineNumber() << "keyword with leading/trailing spaces:" << keyword;
            return false;
        }
    }

    return true;
}

//! Checks that DetectChar and Detect2Chars really only have one char
//! in the attributes 'char' and 'char1'.
bool checkSingleChars(const QString &hlFilename, QXmlStreamReader &xml)
{
    const bool testChar1 = xml.name() == QLatin1String("Detect2Chars");
    const bool testChar = testChar1 || xml.name() == QLatin1String("DetectChar");

    if (testChar) {
        const QString c = xml.attributes().value(QLatin1String("char")).toString();
        if (c.size() != 1) {
            qWarning() << hlFilename << "line" << xml.lineNumber() << "'char' must contain exactly one char:" << c;
        }
    }

    if (testChar1) {
        const QString c = xml.attributes().value(QLatin1String("char1")).toString();
        if (c.size() != 1) {
            qWarning() << hlFilename << "line" << xml.lineNumber() << "'char1' must contain exactly one char:" << c;
        }
    }

    return true;
}

//! Search for rules with lookAhead="true" and context="#stay".
//! This would cause an infinite loop.
bool checkLookAhead(const QString &hlFilename, QXmlStreamReader &xml)
{
    if (xml.attributes().hasAttribute(QStringLiteral("lookAhead"))) {
        auto lookAhead = xml.attributes().value(QStringLiteral("lookAhead"));
        if (lookAhead == QStringLiteral("true")) {
            auto context = xml.attributes().value(QStringLiteral("context"));
            if (context == QStringLiteral("#stay")) {
                qWarning() << hlFilename << "line" << xml.lineNumber() << "Infinite loop: lookAhead with context #stay";
                return false;
            }
        }
    }
    return true;
}

//! Extract the referenced context name.
//! Some input / output examples are:
//! - "#stay"         -> ""
//! - "#pop"          -> ""
//! - "Comment"       -> "Comment"
//! - "#pop!Comment"  -> "Comment"
//! - "##ISO C++"     -> "" (i.e. context cross-language references are currently ignored
QString filterContext(QString context)
{
    // filter out #stay and #pop
    static QRegularExpression stayPop(QStringLiteral("^(#stay|#pop)+"));
    context.remove(stayPop);

    // filter out cross-language context references
    if (context.contains(QStringLiteral("##")))
        return QString();

    // handle #pop!context"
    if (context.startsWith(QLatin1Char('!')))
        context.remove(0, 1);

    return context;
}

/**
 * Helper class to search for non-existing or unreferenced keyword lists.
 */
class KeywordChecker
{
public:
    KeywordChecker(const QString &filename)
        : m_filename(filename)
    {}

    void processElement(QXmlStreamReader &xml)
    {
        if (xml.name() == QLatin1String("list")) {
            const QString name = xml.attributes().value(QLatin1String("name")).toString();
            if (m_existingNames.contains(name)) {
                qWarning() << m_filename << "list duplicate:" << name;
            }
            m_existingNames.insert(name);
        } else if (xml.name() == QLatin1String("keyword")) {
            const QString context = xml.attributes().value(QLatin1String("String")).toString();
            if (!context.isEmpty())
                m_usedNames.insert(context);
        }
    }

    bool check() const
    {
        const auto invalidNames = m_usedNames - m_existingNames;
        if (!invalidNames.isEmpty()) {
            qWarning() << m_filename << "Reference of non-existing keyword list:" << invalidNames;
            return false;
        }

        const auto unusedNames = m_existingNames - m_usedNames;
        if (!unusedNames.isEmpty()) {
            qWarning() << m_filename << "Unused keyword lists:" << unusedNames;
        }

        return true;
    }

private:
    QString m_filename;
    QSet<QString> m_usedNames;
    QSet<QString> m_existingNames;
};

/**
 * Helper class to search for non-existing contexts
 */
class ContextChecker
{
public:
    ContextChecker(const QString &filename)
        : m_filename(filename)
    {}

    void processElement(QXmlStreamReader &xml)
    {
        if (xml.name() == QLatin1String("context")) {
            const QString name = xml.attributes().value(QLatin1String("name")).toString();
            if (m_firstContext.isEmpty()) {
                m_firstContext = name;
            }

            if (m_existingContextNames.contains(name)) {
                qWarning() << m_filename << "Duplicate context:" << name;
            } else {
                m_existingContextNames.insert(name);
            }

            const QString lineEndContext = filterContext(xml.attributes().value(QLatin1String("lineEndContext")).toString());
            if (!lineEndContext.isEmpty())
                m_usedContextNames.insert(lineEndContext);

            const QString lineEmptyContext = filterContext(xml.attributes().value(QLatin1String("lineEmptyContext")).toString());
            if (!lineEmptyContext.isEmpty())
                m_usedContextNames.insert(lineEmptyContext);

            const QString fallthroughContext = filterContext(xml.attributes().value(QLatin1String("fallthroughContext")).toString());
            if (!fallthroughContext.isEmpty())
                m_usedContextNames.insert(fallthroughContext);
        } else {
            if (xml.attributes().hasAttribute(QLatin1String("context"))) {
                QString context = xml.attributes().value(QLatin1String("context")).toString();
                if (context.isEmpty()) {
                    qWarning() << m_filename << "Missing context name in line" << xml.lineNumber();
                } else {
                    context = filterContext(xml.attributes().value(QLatin1String("context")).toString());
                    if (!context.isEmpty())
                        m_usedContextNames.insert(context);
                }
            }
        }
    }

    bool check() const
    {
        const auto invalidContextNames = m_usedContextNames - m_existingContextNames;
        if (!invalidContextNames.isEmpty()) {
            qWarning() << m_filename << "Reference of non-existing contexts:" << invalidContextNames;
            return false;
        }

        auto unusedNames = m_existingContextNames - m_usedContextNames;
        unusedNames.remove(m_firstContext);
        if (!unusedNames.isEmpty()) {
            qWarning() << m_filename << "Unused contexts:" << unusedNames;
        }

        return true;
    }

private:
    QString m_filename;
    QString m_firstContext;
    QSet<QString> m_usedContextNames;
    QSet<QString> m_existingContextNames;
};

/**
 * Helper class to search for non-existing itemDatas.
 */
class AttributeChecker
{
public:
    AttributeChecker(const QString &filename)
        : m_filename(filename)
    {}

    void processElement(QXmlStreamReader &xml)
    {
        if (xml.name() == QLatin1String("itemData")) {
            const QString name = xml.attributes().value(QLatin1String("name")).toString();
            if (!name.isEmpty()) {
                if (m_existingAttributeNames.contains(name)) {
                    qWarning() << m_filename << "itemData duplicate:" << name;
                } else {
                    m_existingAttributeNames.insert(name);
                }
            }
        } else {
            const QString name = xml.attributes().value(QLatin1String("attribute")).toString();
            if (!name.isEmpty())
                m_usedAttributeNames.insert(name);
        }
    }

    bool check() const
    {
        const auto invalidNames = m_usedAttributeNames - m_existingAttributeNames;
        if (!invalidNames.isEmpty()) {
            qWarning() << m_filename << "Reference of non-existing itemData attributes:" << invalidNames;
            return false;
        }

        auto unusedNames = m_existingAttributeNames - m_usedAttributeNames;
        if (!unusedNames.isEmpty()) {
            qWarning() << m_filename << "Unused itemData:" << unusedNames;
        }

        return true;
    }

private:
    QString m_filename;
    QSet<QString> m_usedAttributeNames;
    QSet<QString> m_existingAttributeNames;
};

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
    const QStringList textAttributes = QStringList() << QStringLiteral("name") << QStringLiteral("section") << QStringLiteral("mimetype")
            << QStringLiteral("extensions") << QStringLiteral("style")
            << QStringLiteral("author") << QStringLiteral("license") << QStringLiteral("indenter");

    // index all given highlightings
    QVariantMap hls;
    int anyError = 0;
    foreach (const QString &hlFilename, hlFilenames) {
        QFile hlFile(hlFilename);
        if (!hlFile.open(QIODevice::ReadOnly)) {
            qWarning ("Failed to open %s", qPrintable(hlFilename));
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
        Q_FOREACH (const QString &attribute, textAttributes) {
            hl[attribute] = xml.attributes().value(attribute).toString();
        }

        // numerical attributes
        hl[QStringLiteral("version")] = xml.attributes().value(QLatin1String("version")).toInt();
        hl[QStringLiteral("priority")] = xml.attributes().value(QLatin1String("priority")).toInt();

        // add boolean one
        const QString hidden = xml.attributes().value(QLatin1String("hidden")).toString();
        hl[QStringLiteral("hidden")] = (hidden == QLatin1String("true") || hidden == QLatin1String("1"));

        // remember hl
        hls[QFileInfo(hlFile).fileName()] = hl;

        ContextChecker contextChecker(hlFilename);
        AttributeChecker attributeChecker(hlFilename);
        KeywordChecker keywordChecker(hlFilename);

        // scan for broken regex or keywords with spaces
        while (!xml.atEnd()) {
            xml.readNext();
            if (!xml.isStartElement()) {
                continue;
            }

            // search for used/existing contexts if applicable
            contextChecker.processElement(xml);

            // search for used/existing attributes if applicable
            attributeChecker.processElement(xml);

            // search for used/existing keyword lists if applicable
            keywordChecker.processElement(xml);

            // scan for bad regex
            if (!checkRegularExpression(hlFilename, xml)) {
                anyError = 7;
                continue;
            }

            // scan for bogus <item>     lala    </item> spaces
            if (!checkItemsTrimmed(hlFilename, xml)) {
                anyError = 8;
                continue;
            }

            // check single chars in DetectChar and Detect2Chars
            if (!checkSingleChars(hlFilename, xml)) {
//                 anyError = 8;
                continue;
            }

            // scan for lookAhead="true" with context="#stay"
            if (!checkLookAhead(hlFilename, xml)) {
                anyError = 7;
                continue;
            }
        }

        if (!contextChecker.check())
            anyError = 7;

        if (!attributeChecker.check()) {
            //anyError = 7;
        }

        if (!keywordChecker.check()) {
            //anyError = 7;
        }
    }

    // bail out if any problem was seen
    if (anyError)
        return anyError;

    // create outfile, after all has worked!
    QFile outFile(app.arguments().at(1));
    if (!outFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return 9;

    // write out json
    outFile.write(QJsonDocument::fromVariant(QVariant(hls)).toBinaryData());

    // be done
    return 0;
}
