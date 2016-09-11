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

#include "syntaxhighlighter.h"
#include "format.h"

#include <QDebug>

using namespace SyntaxHighlighting;

namespace SyntaxHighlighting {
class TextBlockUserData : public QTextBlockUserData
{
public:
    explicit TextBlockUserData(const State &s) : state(s) {}
    State state;
};
}

SyntaxHighlighter::SyntaxHighlighter(QObject* parent) :
    QSyntaxHighlighter(parent)
{
}

SyntaxHighlighter::~SyntaxHighlighter()
{
}

void SyntaxHighlighter::highlightBlock(const QString& text)
{
    State state;
    if (currentBlock().position() > 0) {
        const auto prevBlock = currentBlock().previous();
        const auto data = dynamic_cast<TextBlockUserData*>(prevBlock.userData());
        if (data)
            state = data->state;
    }
    state = highlightLine(text, state);
    setCurrentBlockUserData(new TextBlockUserData(state));
}

void SyntaxHighlighter::setFormat(int offset, int length, const SyntaxHighlighting::Format& format)
{
    if (format.isNormal(theme()) || length == 0)
        return;

    QTextCharFormat tf;
    if (format.hasColor(theme()))
        tf.setForeground(format.color(theme()));
    if (format.hasBackgroundColor(theme()))
        tf.setBackground(format.backgroundColor(theme()));

    if (format.isBold(theme()))
        tf.setFontWeight(QFont::Bold);
    if (format.isItalic(theme()))
        tf.setFontItalic(true);
    if (format.isUnderline(theme()))
        tf.setFontUnderline(true);
    if (format.isStrikeThrough(theme()))
        tf.setFontStrikeOut(true);

    QSyntaxHighlighter::setFormat(offset, length, tf);
}
