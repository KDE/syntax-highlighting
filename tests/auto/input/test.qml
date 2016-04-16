/*
 * multi line comment
 */

// single line comment

import QtQuick 2.0

Rectangle {
    property int myIntProp = 0;
    property alias sub.color;
    color: "lightsteelblue"
    width: 320
    height: width/2

    Rectangle {
        id: sub
        width: 0x10
        height: 007
        objectName: 'single quote'
    }
}
