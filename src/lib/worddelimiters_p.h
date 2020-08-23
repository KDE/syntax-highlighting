/*
    SPDX-FileCopyrightText: 2020 Jonathan Poelen <jonathan.poelen@gmail.com>

    SPDX-License-Identifier: MIT
*/

#ifndef KSYNTAXHIGHLIGHTING_WORDDELIMITERS_P_H
#define KSYNTAXHIGHLIGHTING_WORDDELIMITERS_P_H

#include <QString>

namespace KSyntaxHighlighting
{
class WordDelimiters
{
public:
    WordDelimiters();

    bool contains(QChar c) const;
    void append(QChar c);
    void remove(QChar c);

private:
    bool asciiDelimiters[128];
    QString notAsciiDelimiters;
};
}

#endif
