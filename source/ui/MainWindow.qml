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
