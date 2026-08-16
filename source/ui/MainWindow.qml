import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: mainWindowRoot
    color: "transparent"

    SplitView {
        anchors.fill: parent
        orientation: Qt.Horizontal

        GroupBox {
            id: sourceListBox
            title: "Sources"

            SplitView.preferredWidth: 300
            SplitView.minimumWidth: 0
            SplitView.maximumWidth: 400

            onWidthChanged: {
                // Fully collapse source list
                if (width < 100) {
                    width = 0;
                }
            }

            background: Rectangle {
                y: sourceListBox.topPadding - sourceListBox.bottomPadding
                width: parent.width
                height: parent.height - y
                border.color: "#333333"
                border.width: 1
                radius: 4

                visible: parent.width > 100
            }

            label: Text {
                x: sourceListBox.leftPadding
                text: sourceListBox.title
                font.bold: true

                visible: parent.width > 100
            }

            SourceList {
                anchors.fill: parent
            }
        }

        GroupBox {
            id: outputScreenBox
            title: "Screen"

            SplitView.minimumWidth: 400
            SplitView.fillWidth: true

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
}
