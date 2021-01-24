/*
    SPDX-FileCopyrightText: 2020 Aleix Pol Gonzalez <aleixpol@kde.org>

    SPDX-License-Identifier: MIT
*/

#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QPlainTextEdit>

#include <definition.h>
#include <foldingregion.h>
#include <repository.h>
#include <syntaxhighlighter.h>
#include <theme.h>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addPositionalArgument(QStringLiteral("source"), QStringLiteral("The source file to highlight."));
    parser.process(app);

    KSyntaxHighlighting::Repository repository;
    for (const QString &file : parser.positionalArguments()) {
        const auto url = QUrl::fromUserInput(file, {}, QUrl::UserInputResolutionOption::AssumeLocalFile);
        QFile f(url.toLocalFile());
        if (!f.open(QIODevice::ReadOnly))
            continue;

        auto view = new QPlainTextEdit();
        view->setPlainText(QString::fromUtf8(f.readAll()));
        view->resize(500, 500);

        auto highlighter = new KSyntaxHighlighting::SyntaxHighlighter(view->document());
        highlighter->setTheme((view->palette().color(QPalette::Base).lightness() < 128) ? repository.defaultTheme(KSyntaxHighlighting::Repository::DarkTheme)
                                                                                        : repository.defaultTheme(KSyntaxHighlighting::Repository::LightTheme));

        const auto def = repository.definitionForFileName(url.toLocalFile());
        highlighter->setDefinition(def);

        view->show();
    }

    return app.exec();
}
