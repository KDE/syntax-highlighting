/*
    SPDX-FileCopyrightText: 2018 Eike Hein <hein@kde.org>

    SPDX-License-Identifier: MIT
*/

import QtQuick 2.6
import QtQuick.Controls 2.0
import org.kde.syntaxhighlighting 1.0

TextArea {
    id: myText

    width: 250
    height: 250

    text: "int foo = 0;"

    SyntaxHighlighter {
        textEdit: myText
        definition: "C++"
    }
}
