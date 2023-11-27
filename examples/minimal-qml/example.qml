/*
    SPDX-FileCopyrightText: 2018 Eike Hein <hein@kde.org>

    SPDX-License-Identifier: MIT
*/

import QtQuick
import QtQuick.Controls as QQC2
import org.kde.syntaxhighlighting

QQC2.ScrollView {
    width: 250
    height: 250

    QQC2.TextArea {
        id: myText

        text: "int foo = 0;"
        wrapMode: TextEdit.Wrap
        focus: true

        Kirigami.SpellCheck.enabled: false

        SyntaxHighlighter {
            textEdit: myText
            definition: "C++"
        }
    }
}
