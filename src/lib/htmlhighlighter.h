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

#ifndef KSYNTAXHIGHLIGHTING_HTMLHIGHLIGHTER_H
#define KSYNTAXHIGHLIGHTING_HTMLHIGHLIGHTER_H

#include "ksyntaxhighlighting_export.h"
#include "abstracthighlighter.h"

#include <QString>

#include <memory>

class QFile;
class QTextStream;

namespace KSyntaxHighlighting {

class HtmlHighlighterPrivate;

class KSYNTAXHIGHLIGHTING_EXPORT HtmlHighlighter : public AbstractHighlighter
{
public:
    HtmlHighlighter();
    ~HtmlHighlighter();

    void highlightFile(const QString &fileName);

    void setOutputFile(const QString &fileName);
    void setOutputFile(FILE *fileHandle);

protected:
    void applyFormat(int offset, int length, const Format &format) Q_DECL_OVERRIDE;

private:
    std::unique_ptr<HtmlHighlighterPrivate> d;
};
}

#endif // KSYNTAXHIGHLIGHTING_HTMLHIGHLIGHTER_H
