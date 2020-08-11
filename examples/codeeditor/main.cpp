/*
    SPDX-FileCopyrightText: 2016 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: MIT
*/

#include "codeeditor.h"

#include <QApplication>
#include <QCommandLineParser>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addPositionalArgument(QStringLiteral("source"), QStringLiteral("The source file to highlight."));
    parser.process(app);

    CodeEditor edit;
    edit.resize(1024, 1024);
    edit.show();
    if (parser.positionalArguments().size() == 1)
        edit.openFile(parser.positionalArguments().at(0));
    return app.exec();
}
