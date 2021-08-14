/*
    SPDX-FileCopyrightText: 2021 Jonathan Poelen <jonathan.poelen@gmail.com>

    SPDX-License-Identifier: MIT
*/

#ifndef KSYNTAXHIGHLIGHTING_LOAD_HIGHLIGHTING_DATA_P_H
#define KSYNTAXHIGHLIGHTING_LOAD_HIGHLIGHTING_DATA_P_H

#include "highlightingdata_p.hpp"

QT_BEGIN_NAMESPACE
class QXmlStreamReader;
QT_END_NAMESPACE

namespace KSyntaxHighlighting
{
HighlightingContextData loadContextData(const QString &defName, QXmlStreamReader &reader);
}

#endif
