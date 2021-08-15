/*
    SPDX-FileCopyrightText: 2021 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: MIT
*/

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.kde.syntaxhighlighting 1.0

ApplicationWindow {
    visible: true
    width: 1024
    height: 720

    ColumnLayout {
        anchors.fill: parent
        RowLayout {
            Label { text: "Syntax" }
            ComboBox {
                Layout.fillWidth: true
                model: Repository.definitions
                displayText: currentValue.translatedName + " (" + currentValue.translatedSection + ")"
                textRole: "translatedName"
                onCurrentIndexChanged: highlighter.definition = currentValue
            }
        }
        RowLayout {
            Label { text: "Theme" }
            ComboBox {
                Layout.fillWidth: true
                model: Repository.themes
                displayText: currentValue.translatedName
                textRole: "translatedName"
                onCurrentIndexChanged: highlighter.theme = currentValue
            }
            Button {
                text: "Light"
                onClicked: highlighter.theme = Repository.defaultTheme(Repository.LightTheme).name
            }
            Button {
                text: "Dark"
                onClicked: highlighter.theme = Repository.DarkTheme
            }
        }
        TextArea {
            Layout.fillHeight: true
            Layout.fillWidth: true
            id: myText

            text: "Text {\n text: \"Hello World!\"\n width: 42\n}"

            SyntaxHighlighter {
                id: highlighter
                textEdit: myText
                repository: Repository
                // work around for QML not repainting a re-highlighted document...
                onDefinitionChanged: { myText.selectAll(); myText.deselect(); }
                onThemeChanged: { myText.selectAll(); myText.deselect(); }
            }
        }
        Label {
            text: "Syntax: " + highlighter.definition.translatedSection + "/" + highlighter.definition.translatedName + "  Theme: " + highlighter.theme.translatedName
        }
    }

    Component.onCompleted: {
        highlighter.definition = Repository.definitionForFileName("example.qml")
    }
}
