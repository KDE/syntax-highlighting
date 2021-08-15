/*
    SPDX-FileCopyrightText: 2018 Eike Hein <hein@kde.org>

    SPDX-License-Identifier: MIT
*/

#include "kquicksyntaxhighlightingplugin.h"
#include "kquicksyntaxhighlighter.h"

#include <QQmlEngine>

void KQuickSyntaxHighlightingPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(QLatin1String(uri) == QLatin1String("org.kde.syntaxhighlighting"));
    qmlRegisterType<KQuickSyntaxHighlighter>(uri, 1, 0, "SyntaxHighlighter");
}
