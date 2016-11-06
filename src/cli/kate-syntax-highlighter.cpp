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

#include "ksyntaxhighlighting_version.h"

#include <definition.h>
#include <definitiondownloader.h>
#include <htmlhighlighter.h>
#include <repository.h>
#include <theme.h>

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QVector>

#include <iostream>

using namespace KSyntaxHighlighting;

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(QStringLiteral("kate-syntax-highlighter"));
    QCoreApplication::setOrganizationDomain(QStringLiteral("kde.org"));
    QCoreApplication::setOrganizationName(QStringLiteral("KDE"));
    QCoreApplication::setApplicationVersion(QStringLiteral(SyntaxHighlighting_VERSION_STRING));

    QCommandLineParser parser;
    parser.setApplicationDescription(app.translate("SyntaxHighlightingCLI", "Command line syntax highlighter using Kate syntax definitions."));
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument(app.translate("SyntaxHighlightingCLI", "source"),
                                 app.translate("SyntaxHighlightingCLI", "The source file to highlight."));

    QCommandLineOption listDefs(QStringList() << QStringLiteral("l") << QStringLiteral("list"),
                                app.translate("SyntaxHighlightingCLI", "List all available syntax definitions."));
    parser.addOption(listDefs);
    QCommandLineOption listThemes(QStringList() << QStringLiteral("list-themes"),
                                  app.translate("SyntaxHighlightingCLI", "List all available themes."));
    parser.addOption(listThemes);

    QCommandLineOption updateDefs(QStringList() << QStringLiteral("u") << QStringLiteral("update"),
                                  app.translate("SyntaxHighlightingCLI", "Download new/updated syntax definitions."));
    parser.addOption(updateDefs);

    QCommandLineOption outputName(QStringList() << QStringLiteral("o") << QStringLiteral("output"),
                                  app.translate("SyntaxHighlightingCLI", "File to write HTML output to (default: stdout)."),
                                  app.translate("SyntaxHighlightingCLI", "output"));
    parser.addOption(outputName);

    QCommandLineOption syntaxName(QStringList() << QStringLiteral("s") << QStringLiteral("syntax"),
                                  app.translate("SyntaxHighlightingCLI", "Highlight using this syntax definition (default: auto-detect based on input file)."),
                                  app.translate("SyntaxHighlightingCLI", "syntax"));
    parser.addOption(syntaxName);

    QCommandLineOption themeName(QStringList() << QStringLiteral("t") << QStringLiteral("theme"),
                                 app.translate("SyntaxHighlightingCLI", "Color theme to use for highlighting."),
                                 app.translate("SyntaxHighlightingCLI", "theme"), QStringLiteral("Default"));
    parser.addOption(themeName);

    parser.process(app);

    Repository repo;
    if (parser.isSet(listDefs)) {
        foreach (const auto &def, repo.definitions()) {
            std::cout << qPrintable(def.name()) << std::endl;
        }
        return 0;
    }

    if (parser.isSet(listThemes)) {
        foreach (const auto &theme, repo.themes())
            std::cout << qPrintable(theme.name()) << std::endl;
        return 0;
    }

    if (parser.isSet(updateDefs)) {
        DefinitionDownloader downloader(&repo);
        QObject::connect(&downloader, &DefinitionDownloader::informationMessage, [](const QString &msg) {
            std::cout << qPrintable(msg) << std::endl;
        });
        QObject::connect(&downloader, &DefinitionDownloader::done, &app, &QCoreApplication::quit);
        downloader.start();
        return app.exec();
    }

    if (parser.positionalArguments().size() != 1)
        parser.showHelp(1);
    const auto inFileName = parser.positionalArguments().at(0);

    Definition def;
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
    highlighter.setTheme(repo.theme(parser.value(themeName)));
    highlighter.highlightFile(inFileName);

    return 0;
}
