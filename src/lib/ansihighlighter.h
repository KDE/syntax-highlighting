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
#include <QFlags>

#include <memory>

namespace KSyntaxHighlighting
{
class AnsiHighlighterPrivate;

class KSYNTAXHIGHLIGHTING_EXPORT AnsiHighlighter final : public AbstractHighlighter
{
public:
    enum class AnsiFormat
    {
        TrueColor,
        XTerm256Color,
    };

    enum class DebugOption
    {
        NoOptions,
        FormatName    = 1 << 0,
        RegionName    = 1 << 1,
        ContextName   = 1 << 2,
    };
    Q_DECLARE_FLAGS(DebugOptions, DebugOption)

    AnsiHighlighter();
    ~AnsiHighlighter() override;

    void highlightFile(const QString &fileName, AnsiFormat format = AnsiFormat::TrueColor, bool useEditorBackground = true, DebugOptions debugOptions = DebugOptions());
    void highlightData(QIODevice *device, AnsiFormat format = AnsiFormat::TrueColor, bool useEditorBackground = true, DebugOptions debugOptions = DebugOptions());

    void setOutputFile(const QString &fileName);
    void setOutputFile(FILE *fileHandle);

protected:
    void applyFormat(int offset, int length, const Format &format) override;

private:
    std::unique_ptr<AnsiHighlighterPrivate> d;
};
}

Q_DECLARE_OPERATORS_FOR_FLAGS(KSyntaxHighlighting::AnsiHighlighter::DebugOptions)

#endif // KSYNTAXHIGHLIGHTING_ANSIHIGHLIGHTER_H
