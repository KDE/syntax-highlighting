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

#include <syntaxhighlighter.h>
#include <definition.h>
#include <repository.h>
#include <theme.h>

#include <QApplication>
#include <QCommandLineParser>
#include <QFile>
#include <QTextEdit>

using namespace SyntaxHighlighting;

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addPositionalArgument(QStringLiteral("source"), QStringLiteral("The source file to highlight."));
    QCommandLineOption outputName(QStringList() << QStringLiteral("o") << QStringLiteral("output"),
                                  QStringLiteral("File to write HTML output to (default: stdout)."),
                                  QStringLiteral("output"));
    parser.addOption(outputName);
    QCommandLineOption syntaxName(QStringList() << QStringLiteral("s") << QStringLiteral("syntax"),
                                  QStringLiteral("Highlight using this syntax definition (default: auto-detect based on input file)."),
                                  QStringLiteral("syntax"));
    parser.addOption(syntaxName);
    parser.process(app);

    if (parser.positionalArguments().size() != 1)
        parser.showHelp(1);

    const auto inFileName = parser.positionalArguments().at(0);
    QFile f(inFileName);
    if (!f.open(QFile::ReadOnly))
        return 1;

    SyntaxHighlighting::Repository repo;

    Definition def;
    if (parser.isSet(syntaxName)) {
        def = repo.definitionForName(parser.value(syntaxName));
    } else {
        def = repo.definitionForFileName(inFileName);
    }
    if (!def.isValid())
        qFatal("Unknown syntax.");

    QTextEdit edit;
    edit.setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    edit.resize(1024, 1024);
    edit.show();
    auto hl = new SyntaxHighlighting::SyntaxHighlighter(&edit);
    hl->setTheme((app.palette().color(QPalette::Base).lightness() < 128)
        ? repo.defaultTheme(Repository::DarkTheme)
        : repo.defaultTheme(Repository::LightTheme));
    hl->setDefinition(def);

    edit.setPlainText(QString::fromUtf8(f.readAll()));

    return app.exec();
}
