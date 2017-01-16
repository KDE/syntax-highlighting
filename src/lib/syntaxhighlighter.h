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

#ifndef KSYNTAXHIGHLIGHTING_QSYNTAXHIGHLIGHTER_H
#define KSYNTAXHIGHLIGHTING_QSYNTAXHIGHLIGHTER_H

#include "ksyntaxhighlighting_export.h"

#include "abstracthighlighter.h"

#include <QSyntaxHighlighter>

namespace KSyntaxHighlighting {

class SyntaxHighlighterPrivate;

/** A QSyntaxHighlighter implementation for use with QTextDocument.
 *  This supports partial re-highlighting during editing and
 *  tracks syntax-based code folding regions.
 */
class KSYNTAXHIGHLIGHTING_EXPORT SyntaxHighlighter : public QSyntaxHighlighter, public AbstractHighlighter
{
    Q_OBJECT
public:
    explicit SyntaxHighlighter(QObject *parent = nullptr);
    explicit SyntaxHighlighter(QTextDocument *document);
    ~SyntaxHighlighter();

    void setDefinition(const Definition &def) Q_DECL_OVERRIDE;

    /** Returns whether there is a folding region beginning at @p startBlock.
     *  This only considers syntax-based folding regions,
     *  not indention-based ones as e.g. found in Python.
     *
     *  @see findFoldingRegionEnd
     */
    bool startsFoldingRegion(const QTextBlock &startBlock) const;

    /** Finds the end of the folding region starting at @p startBlock.
     *  If multiple folding regions begin at @p startBlock, the end of
     *  the last/innermost one is returned.
     *  This returns an invalid block if no folding region end is found,
     *  which typically indicates an unterminated region and thus folding
     *  until the document end.
     *  This method performs a sequential search starting at @p startBlock
     *  for the matching folding region end, which is a potentially expensive
     *  operation.
     *
     *  @see startsFoldingRegion
     */
    QTextBlock findFoldingRegionEnd(const QTextBlock &startBlock) const;

protected:
    void highlightBlock(const QString & text) Q_DECL_OVERRIDE;
    void applyFormat(int offset, int length, const Format &format) Q_DECL_OVERRIDE;
    void applyFolding(int offset, int length, FoldingRegion region) Q_DECL_OVERRIDE;

private:
    Q_DECLARE_PRIVATE_D(AbstractHighlighter::d_ptr, SyntaxHighlighter)
};
}

#endif // KSYNTAXHIGHLIGHTING_QSYNTAXHIGHLIGHTER_H
