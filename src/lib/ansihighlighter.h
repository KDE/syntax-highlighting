/*
    SPDX-FileCopyrightText: 2020 Jonathan Poelen <jonathan.poelen@gmail.com>

    SPDX-License-Identifier: MIT
*/

#ifndef KSYNTAXHIGHLIGHTING_ANSIHIGHLIGHTER_H
#define KSYNTAXHIGHLIGHTING_ANSIHIGHLIGHTER_H

#include "abstracthighlighter.h"
#include "ksyntaxhighlighting_export.h"

#include <QIODevice>
#include <QString>

#include <memory>

namespace KSyntaxHighlighting
{
class AnsiHighlighterPrivate;

class KSYNTAXHIGHLIGHTING_EXPORT AnsiHighlighter : public AbstractHighlighter
{
public:
    enum class AnsiFormat
    {
        TrueColor,
        XTerm256Color,
    };

    AnsiHighlighter();
    ~AnsiHighlighter() override;

    void highlightFile(const QString &fileName, AnsiFormat format = AnsiFormat::TrueColor);
    void highlightData(QIODevice *device, AnsiFormat format = AnsiFormat::TrueColor);

    void setOutputFile(const QString &fileName);
    void setOutputFile(FILE *fileHandle);

    /**
     * Add a format name to each highlighted part
     */
    void enableFormatNameTrace(bool enabled = true);

protected:
    void applyFormat(int offset, int length, const Format &format) override;

private:
    std::unique_ptr<AnsiHighlighterPrivate> d;
};
}

#endif // KSYNTAXHIGHLIGHTING_ANSIHIGHLIGHTER_H
