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
    return listing;
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
            qWarning ("Failed to open %s.", qPrintable(hlFilename));
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

        // scan for broken regex
        while (!xml.atEnd()) {
            xml.readNext();
            if (!xml.isStartElement() || (xml.name() != QLatin1String("RegExpr") && xml.name() != QLatin1String("emptyLine"))) {
                continue;
            }

            // get right attribute
            const QString string (xml.attributes().value((xml.name() == QLatin1String("RegExpr")) ? QLatin1String("String") : QLatin1String("regexpr")).toString());

            // validate regexp
            const QRegularExpression regexp (string);
            if (!regexp.isValid()) {
                qDebug() << hlFilename << "line" << xml.lineNumber() << "broken regex:" << string << "problem:" << regexp.errorString() << "at offset" << regexp.patternErrorOffset();
                anyError = 7;
            }
        }
    }

    // bail out if any problem was seen
    if (anyError)
        return anyError;

    // create outfile, after all has worked!
    QFile outFile(app.arguments().at(1));
    if (!outFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return 7;

    // write out json
    outFile.write(QJsonDocument::fromVariant(QVariant(hls)).toBinaryData());

    // be done
    return 0;
}
