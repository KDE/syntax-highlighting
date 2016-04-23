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

#include "htmlhighlighter.h"
#include "format.h"

#include <QDebug>
#include <QFile>
#include <QTextStream>

using namespace KateSyntax;

HtmlHighlighter::HtmlHighlighter()
{
}

HtmlHighlighter::~HtmlHighlighter()
{
}

void HtmlHighlighter::setOutputFile(const QString& fileName)
{
    m_file.reset(new QFile(fileName));
    if (!m_file->open(QFile::WriteOnly | QFile::Truncate)) {
        qWarning() << "Failed to open output file" << fileName << ":" << m_file->errorString();
        return;
    }
    m_out.reset(new QTextStream(m_file.get()));
}

void HtmlHighlighter::setOutputFile(FILE *fileHandle)
{
    m_out.reset(new QTextStream(fileHandle, QIODevice::WriteOnly));

}

void HtmlHighlighter::highlightFile(const QString& fileName)
{
    if (!m_out) {
        qWarning() << "No output stream defined!";
        return;
    }

    QFile f(fileName);
    if (!f.open(QFile::ReadOnly)) {
        qWarning() << "Failed to open input file" << fileName << ":" << f.errorString();
        return;
    }

    *m_out << "<html><body><pre>\n";

    QTextStream in(&f);
    while (!in.atEnd()) {
        m_currentLine = in.readLine();
        highlightLine(m_currentLine);
        *m_out << "\n";
    }

    *m_out << "</pre></body></html>\n";
    m_out->flush();

    m_out.reset();
    m_file.reset();
}

void HtmlHighlighter::setFormat(int offset, int length, const Format& format)
{
    if (!format.isNormal()) {
        *m_out << "<span style=\"";
        if (format.hasColor())
            *m_out << "color:" << format.color().name() << ";";
        if (format.hasBackgroundColor())
            *m_out << "background-color:" << format.backgroundColor().name() << ";";
        *m_out << "\">";
    }

    *m_out << m_currentLine.mid(offset, length).toHtmlEscaped();

    if (!format.isNormal())
        *m_out << "</span>";
}
