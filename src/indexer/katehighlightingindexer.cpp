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

QString filterContext(QString context)
{
    // filter out #stay and #pop
    static QRegularExpression stayPop(QStringLiteral("^(#stay|#pop)+"));
    context.remove(stayPop);

    // filter out cross-language context references
    if (context.contains(QStringLiteral("##")))
        return QString();

    // TODO: handle #pop!context"
    if (context.startsWith(QLatin1Char('!')))
        return QString();

    return context;
}

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
            m_existingContextNames.insert(name);

            const QString lineEndContext = filterContext(xml.attributes().value(QLatin1String("lineEndContext")).toString());
            if (!lineEndContext.isEmpty())
                m_usedContextNames.insert(lineEndContext);
        } else {
            const QString context = filterContext(xml.attributes().value(QLatin1String("context")).toString());
            if (!context.isEmpty())
                m_usedContextNames.insert(context);
        }
    }

    bool check() const
    {
        const auto invalidContextNames = m_usedContextNames - m_existingContextNames;
        if (!invalidContextNames.isEmpty()) {
            qWarning() << m_filename << "Reference of non-existing contexts:" << invalidContextNames;
            return false;
        }

        return true;
    }

private:
    QString m_filename;
    QSet<QString> m_usedContextNames;
    QSet<QString> m_existingContextNames;
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

        // scan for broken regex or keywords with spaces
        while (!xml.atEnd()) {
            xml.readNext();
            if (!xml.isStartElement()) {
                continue;
            }

            // search for used/existing contexts if applicable
            contextChecker.processElement(xml);

            // scan for bad regex
            if (xml.name() == QLatin1String("RegExpr") || xml.name() == QLatin1String("emptyLine")) {
                // get right attribute
                const QString string (xml.attributes().value((xml.name() == QLatin1String("RegExpr")) ? QLatin1String("String") : QLatin1String("regexpr")).toString());

                // validate regexp
                const QRegularExpression regexp (string);
                if (!regexp.isValid()) {
                    qWarning() << hlFilename << "line" << xml.lineNumber() << "broken regex:" << string << "problem:" << regexp.errorString() << "at offset" << regexp.patternErrorOffset();
                    anyError = 7;
                }

                // catch possible case typos: [A-z] or [a-Z]
                const int azOffset = std::max(string.indexOf(QStringLiteral("A-z")), string.indexOf(QStringLiteral("a-Z")));
                if (azOffset >= 0) {
                    qWarning() << hlFilename << "line" << xml.lineNumber() << "broken regex:" << string << "problem: [a-Z] or [A-z] at offset" << azOffset;
                    anyError = 7;
                }

                continue;
            }

            // scan for bogus <item>     lala    </item> spaces
            if (xml.name() == QLatin1String("item")) {
                const QString keyword = xml.readElementText();
                if (keyword != keyword.trimmed()) {
                    qWarning() << hlFilename << "line" << xml.lineNumber() << "keyword with leading/trailing spaces:" << keyword;
                    anyError = 8;
                }
                continue;
            }
        }

        if (!contextChecker.check())
            anyError = 7;
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
