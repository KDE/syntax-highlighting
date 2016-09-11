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

#ifndef SYNTAXHIGHLIGHTING_QSYNTAXHIGHLIGHTER_H
#define SYNTAXHIGHLIGHTING_QSYNTAXHIGHLIGHTER_H

#include "kf5syntaxhighlighting_export.h"

#include "abstracthighlighter.h"
#include "state.h"

#include <QSyntaxHighlighter>

namespace SyntaxHighlighting {

class KF5SYNTAXHIGHLIGHTING_EXPORT SyntaxHighlighter : public QSyntaxHighlighter, public AbstractHighlighter
{
    Q_OBJECT
public:
    explicit SyntaxHighlighter(QObject *parent = Q_NULLPTR);
    ~SyntaxHighlighter();

protected:
    void highlightBlock(const QString & text) Q_DECL_OVERRIDE;
    void setFormat(int offset, int length, const Format &format) Q_DECL_OVERRIDE;
};
}

#endif // SYNTAXHIGHLIGHTING_QSYNTAXHIGHLIGHTER_H
