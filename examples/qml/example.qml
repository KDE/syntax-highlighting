/*
    SPDX-FileCopyrightText: 2021 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: MIT
*/

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.syntaxhighlighting

ApplicationWindow {
    visible: true
    width: 1024
    height: 720

    ColumnLayout {
        anchors.fill: parent
        spacing: Kirigami.Units.smallSpacing
        RowLayout {
            spacing: Kirigami.Units.smallSpacing
            Layout.fillWidth: true
            Layout.topMargin: Kirigami.Units.smallSpacing
            Layout.leftMargin: Kirigami.Units.smallSpacing
            Layout.rightMargin: Kirigami.Units.smallSpacing
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
            spacing: Kirigami.Units.smallSpacing
            Layout.fillWidth: true
            Layout.leftMargin: Kirigami.Units.smallSpacing
            Layout.rightMargin: Kirigami.Units.smallSpacing
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
        ScrollView {
            Layout.fillHeight: true
            Layout.fillWidth: true

            TextArea {
                id: myText

                font.family: "monospace"
                wrapMode: TextEdit.Wrap
                text: `\
Text {
    text: "Hello World!"
    width: 42
}\
`
                Kirigami.SpellCheck.enabled: false

                SyntaxHighlighter {
                    id: highlighter
                    textEdit: myText
                    repository: Repository
                }
            }
        }
        Label {
            Layout.fillWidth: true
            Layout.margins: Kirigami.Units.smallSpacing
            Layout.topMargin: 0
            elide: Text.ElideRight
            text: `Syntax: ${highlighter.definition.translatedSection}/${highlighter.definition.translatedName}. Theme: ${highlighter.theme.translatedName}`
        }
    }

    Component.onCompleted: {
        highlighter.definition = Repository.definitionForFileName("example.qml")
    }
}
