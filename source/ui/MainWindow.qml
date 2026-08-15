import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: mainWindowRoot
    color: "transparent"

    GroupBox {
        id: outputScreenBox
        title: "Screen"
        anchors.fill: parent

        background: Rectangle {
            y: outputScreenBox.topPadding - outputScreenBox.bottomPadding
            width: parent.width
            height: parent.height - y
            color: "black"
            border.color: "#333333"
            border.width: 1
            radius: 4
        }

        label: Text {
            x: outputScreenBox.leftPadding
            text: outputScreenBox.title
            font.bold: true
        }

        ScreenRoot {
            anchors.fill: parent
        }
    }
}
