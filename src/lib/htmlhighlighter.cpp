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
#include "definition.h"
#include "format.h"
#include "state.h"
#include "theme.h"
#include "ksyntaxhighlighting_logging.h"

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

using namespace KSyntaxHighlighting;

class KSyntaxHighlighting::HtmlHighlighterPrivate
{
public:
    std::unique_ptr<QTextStream> out;
    std::unique_ptr<QFile> file;
    QString currentLine;
};

HtmlHighlighter::HtmlHighlighter()
    : d(new HtmlHighlighterPrivate())
{
}

HtmlHighlighter::~HtmlHighlighter()
{
}

void HtmlHighlighter::setOutputFile(const QString& fileName)
{
    d->file.reset(new QFile(fileName));
    if (!d->file->open(QFile::WriteOnly | QFile::Truncate)) {
        qCWarning(Log) << "Failed to open output file" << fileName << ":" << d->file->errorString();
        return;
    }
    d->out.reset(new QTextStream(d->file.get()));
    d->out->setCodec("UTF-8");
}

void HtmlHighlighter::setOutputFile(FILE *fileHandle)
{
    d->out.reset(new QTextStream(fileHandle, QIODevice::WriteOnly));
    d->out->setCodec("UTF-8");
}

void HtmlHighlighter::highlightFile(const QString& fileName)
{
    if (!d->out) {
        qCWarning(Log) << "No output stream defined!";
        return;
    }

    QFile f(fileName);
    if (!f.open(QFile::ReadOnly)) {
        qCWarning(Log) << "Failed to open input file" << fileName << ":" << f.errorString();
        return;
    }

    State state;
    *d->out << "<!DOCTYPE html>\n";
    *d->out << "<html><head>\n";
    *d->out << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\"/>\n";
    QFileInfo fi(fileName);
    *d->out << "<title>" << fi.fileName() << "</title>\n";
    *d->out << "<meta name=\"generator\" content=\"KF5::SyntaxHighlighting (" << definition().name() << ")\"/>\n";
    *d->out << "</head><body";
    if (theme().textColor(Theme::Normal))
        *d->out << " style=\"color:" << QColor(theme().textColor(Theme::Normal)).name() << "\"";
    *d->out << "><pre>\n";

    QTextStream in(&f);
    in.setCodec("UTF-8");
    while (!in.atEnd()) {
        d->currentLine = in.readLine();
        state = highlightLine(d->currentLine, state);
        *d->out << "\n";
    }

    *d->out << "</pre></body></html>\n";
    d->out->flush();

    d->out.reset();
    d->file.reset();
}

void HtmlHighlighter::applyFormat(int offset, int length, const Format& format)
{
    if (!format.isDefaultTextStyle(theme())) {
        *d->out << "<span style=\"";
        if (format.hasTextColor(theme()))
            *d->out << "color:" << format.textColor(theme()).name() << ";";
        if (format.hasBackgroundColor(theme()))
            *d->out << "background-color:" << format.backgroundColor(theme()).name() << ";";
        if (format.isBold(theme()))
            *d->out << "font-weight:bold;";
        if (format.isItalic(theme()))
            *d->out << "font-style:italic;";
        if (format.isUnderline(theme()))
            *d->out << "text-decoration:underline;";
        if (format.isStrikeThrough(theme()))
            *d->out << "text-decoration:line-through;";
        *d->out << "\">";
    }

    *d->out << d->currentLine.mid(offset, length).toHtmlEscaped();

    if (!format.isDefaultTextStyle(theme()))
        *d->out << "</span>";
}
