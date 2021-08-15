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
                textRole: "translatedName"
                onCurrentIndexChanged: {
                    console.log(highlighter.definition, Repository.definitions[currentIndex].name);
                    highlighter.definition = Repository.definitions[currentIndex];
                }
            }
        }
        RowLayout {
            Label { text: "Theme" }
            ComboBox {
                Layout.fillWidth: true
                model: Repository.themes
                textRole: "translatedName"
                onCurrentIndexChanged: {
                    console.log(Repository.themes[currentIndex].name);
                }
            }
            Button {
                text: "Light"
                onClicked: console.log(Repository.defaultTheme(Repository.LightTheme))
            }
            Button {
                text: "Dark"
                onClicked: console.log(Repository.defaultTheme(Repository.DarkTheme).name)
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
            }
        }
    }

    Component.onCompleted: {
        highlighter.definition = Repository.definitionForFileName("example.qml")
    }
}
