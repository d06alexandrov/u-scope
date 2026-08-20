import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window

ApplicationWindow {
    id: mainWindowRoot
    title: qsTr("U-Scope")

    width: 1200
    height: 600
    visible: true

    menuBar: MenuBar {
        Menu {
            title: qsTr("&Help")
            Action {
                text: qsTr("&About")
                onTriggered: Qt.callLater(() => appController.about_menu())
            }
        }
    }

    SplitView {
        anchors.fill: parent
        anchors.topMargin: 8
        anchors.leftMargin: 8
        anchors.rightMargin: 8
        anchors.bottomMargin: 8
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

        RowLayout {
            SplitView.minimumWidth: 600
            SplitView.fillWidth: true

            GroupBox {
                id: outputScreenBox
                title: "Screen"

                Layout.fillWidth: true
                Layout.fillHeight: true

                label: Text {
                    x: outputScreenBox.leftPadding
                    text: outputScreenBox.title
                    font.bold: true
                }

                ScreenRoot {
                    anchors.fill: parent
                }
            }

            GroupBox {
                id: controlPanelBox
                title: "Control panel"

                Layout.alignment: Qt.AlignTop

                label: Text {
                    x: controlPanelBox.leftPadding
                    text: controlPanelBox.title
                    font.bold: true
                }

                ControlPanel {
                    anchors.fill: parent
                }
            }
        }
    }
}
