/*
    SPDX-FileCopyrightText: 2019 Friedrich W. H. Kossebau <kossebau@kde.org>

    SPDX-License-Identifier: MIT
*/

#include "codepdfprinter.h"

#include <definition.h>
#include <syntaxhighlighter.h>
#include <theme.h>

#include <QDebug>
#include <QFile>
#include <QFontDatabase>
#include <QFontMetrics>
#include <QPrinter>

CodePdfPrinter::CodePdfPrinter()
    : m_highlighter(new KSyntaxHighlighting::SyntaxHighlighter(&m_document))
{
    const auto font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    const QFontMetrics fontMetrics(font);
    m_document.setDefaultFont(font);

    QTextOption textOption(Qt::AlignTop | Qt::AlignLeft);
    textOption.setTabStopDistance(8 * fontMetrics.horizontalAdvance(QLatin1Char(' ')));
    textOption.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    m_document.setDefaultTextOption(textOption);

    // light theme for "printing" on white PDF "paper"
    const auto theme = m_repository.defaultTheme(KSyntaxHighlighting::Repository::LightTheme);
    m_highlighter->setTheme(theme);
}

CodePdfPrinter::~CodePdfPrinter() = default;

bool CodePdfPrinter::openSourceFile(const QString &fileName)
{
    QFile f(fileName);
    if (!f.open(QFile::ReadOnly)) {
        qWarning() << "Failed to open" << fileName << ":" << f.errorString();
        return false;
    }

    const auto def = m_repository.definitionForFileName(fileName);
    m_highlighter->setDefinition(def);

    m_document.setPlainText(QString::fromUtf8(f.readAll()));
    return true;
}

void CodePdfPrinter::printPdfFile(const QString &fileName)
{
    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPageSize(QPageSize(QPageSize::A4));
    printer.setOutputFileName(fileName);

    m_document.print(&printer);
}
