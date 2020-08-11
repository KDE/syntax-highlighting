/*
    SPDX-FileCopyrightText: 2019 Friedrich W. H. Kossebau <kossebau@kde.org>

    SPDX-License-Identifier: MIT
*/

#ifndef CODEPDFPRINTER_H
#define CODEPDFPRINTER_H

#include <repository.h>

#include <QTextDocument>

namespace KSyntaxHighlighting
{
class SyntaxHighlighter;
}

class CodePdfPrinter
{
public:
    explicit CodePdfPrinter();
    ~CodePdfPrinter();

public:
    bool openSourceFile(const QString &fileName);
    void printPdfFile(const QString &fileName);

private:
    QTextDocument m_document;

    KSyntaxHighlighting::Repository m_repository;
    KSyntaxHighlighting::SyntaxHighlighter *m_highlighter;
};

#endif
