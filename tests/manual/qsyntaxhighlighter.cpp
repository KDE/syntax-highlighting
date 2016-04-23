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
#include <syntaxrepository.h>

#include <QApplication>
#include <QCommandLineParser>
#include <QFile>
#include <QTextEdit>

using namespace KateSyntax;

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addPositionalArgument(QStringLiteral("source"), app.tr("The source file to highlight."));
    QCommandLineOption outputName(QStringList() << QStringLiteral("o") << QStringLiteral("output"),
                                  app.tr("File to write HTML output to (default: stdout)."),
                                  app.tr("output"));
    parser.addOption(outputName);
    QCommandLineOption syntaxName(QStringList() << QStringLiteral("s") << QStringLiteral("syntax"),
                                  app.tr("Highlight using this syntax definition (default: auto-detect based on input file)."),
                                  app.tr("syntax"));
    parser.addOption(syntaxName);
    parser.process(app);

    if (parser.positionalArguments().size() != 1)
        parser.showHelp(1);

    const auto inFileName = parser.positionalArguments().at(0);
    QFile f(inFileName);
    if (!f.open(QFile::ReadOnly))
        return 1;

    KateSyntax::SyntaxRepository repo;

    SyntaxDefinition *def = nullptr;
    if (parser.isSet(syntaxName)) {
        def = repo.definitionForName(parser.value(syntaxName));
    } else {
        def = repo.definitionForFileName(inFileName);
    }
    if (!def)
        qFatal("Unknown syntax.");

    QTextEdit edit;
    edit.setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    edit.setReadOnly(true);
    edit.resize(1024, 1024);
    edit.show();
    auto hl = new KateSyntax::SyntaxHighlighter(&edit);
    hl->setDefinition(def);

    edit.setPlainText(QString::fromUtf8(f.readAll()));
    hl->setDocument(edit.document());

    return app.exec();
}
