/*
    Copyright (C) 2016 Volker Krause <vkrause@kde.org>

    This program is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "syntax_highlighting_version.h"

#include <downloader.h>
#include <htmlhighlighter.h>
#include <syntaxrepository.h>
#include <syntaxdefinition.h>

#include <QCommandLineParser>
#include <QCoreApplication>

#include <iostream>

using namespace SyntaxHighlighting;

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(QStringLiteral("kate-syntax-highlighter"));
    QCoreApplication::setOrganizationDomain(QStringLiteral("kde.org"));
    QCoreApplication::setOrganizationName(QStringLiteral("KDE"));
    QCoreApplication::setApplicationVersion(QStringLiteral(SyntaxHighlighting_VERSION_STRING));

    QCommandLineParser parser;
    parser.setApplicationDescription(app.tr("Command line syntax highlighter using Kate syntax definitions."));
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument(QStringLiteral("source"), app.tr("The source file to highlight."));

    QCommandLineOption listDefs(QStringList() << QStringLiteral("l") << QStringLiteral("list"),
                                app.tr("List all available syntax definitions."));
    parser.addOption(listDefs);

    QCommandLineOption updateDefs(QStringList() << QStringLiteral("u") << QStringLiteral("update"),
                                  app.tr("Download new/updated syntax definitions."));
    parser.addOption(updateDefs);

    QCommandLineOption outputName(QStringList() << QStringLiteral("o") << QStringLiteral("output"),
                                  app.tr("File to write HTML output to (default: stdout)."),
                                  app.tr("output"));
    parser.addOption(outputName);

    QCommandLineOption syntaxName(QStringList() << QStringLiteral("s") << QStringLiteral("syntax"),
                                  app.tr("Highlight using this syntax definition (default: auto-detect based on input file)."),
                                  app.tr("syntax"));
    parser.addOption(syntaxName);

    // TODO select style

    parser.process(app);

    SyntaxRepository repo;
    if (parser.isSet(listDefs)) {
        foreach (const auto &def, repo.definitions()) {
            std::cout << qPrintable(def.name()) << std::endl;
        }
        return 0;
    }

    if (parser.isSet(updateDefs)) {
        Downloader downloader(&repo);
        QObject::connect(&downloader, &Downloader::informationMessage, [](const QString &msg) {
            std::cout << qPrintable(msg) << std::endl;
        });
        QObject::connect(&downloader, &Downloader::done, &app, &QCoreApplication::quit);
        downloader.start();
        return app.exec();
    }

    if (parser.positionalArguments().size() != 1)
        parser.showHelp(1);
    const auto inFileName = parser.positionalArguments().at(0);

    SyntaxDefinition def;
    if (parser.isSet(syntaxName)) {
        def = repo.definitionForName(parser.value(syntaxName));
    } else {
        def = repo.definitionForFileName(inFileName);
    }
    if (!def.isValid()) {
        std::cerr << "Unknown syntax." << std::endl;
        return 1;
    }

    HtmlHighlighter highlighter;
    highlighter.setDefinition(def);
    if (parser.isSet(outputName))
        highlighter.setOutputFile(parser.value(outputName));
    else
        highlighter.setOutputFile(stdout);
    highlighter.highlightFile(inFileName);

    return 0;
}
