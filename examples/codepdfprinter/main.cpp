/*
    SPDX-FileCopyrightText: 2019 Friedrich W. H. Kossebau <kossebau@kde.org>

    SPDX-License-Identifier: MIT
*/

#include "codepdfprinter.h"

#include <QApplication>
#include <QCommandLineParser>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addPositionalArgument(QStringLiteral("source"), QStringLiteral("The source file to print."));
    parser.addPositionalArgument(QStringLiteral("pdf"), QStringLiteral("The PDF file to print to."));
    parser.process(app);

    if (parser.positionalArguments().size() < 2) {
        parser.showHelp();
    }

    CodePdfPrinter printer;
    if (printer.openSourceFile(parser.positionalArguments().at(0))) {
        printer.printPdfFile(parser.positionalArguments().at(1));
    }

    return 0;
}
