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

#ifndef KATESYNTAX_HTMLHIGHLIGHTER_H
#define KATESYNTAX_HTMLHIGHLIGHTER_H

#include "katesyntax_export.h"
#include "abstracthighlighter.h"

#include <QTextStream>

namespace KateSyntax {

class KATESYNTAX_EXPORT HTMLHighlighter : public AbstractHighlighter
{
public:
    HTMLHighlighter();
    void highlightFile(const QString &fileName);
    void setOutputFile(const QString &fileName);

protected:
    void setFormat(int offset, int length, const QString & format) override;

private:
    QTextStream m_out;
    QString m_currentLine;
};
}

#endif // KATESYNTAX_HTMLHIGHLIGHTER_H
