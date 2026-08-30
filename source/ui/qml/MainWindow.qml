import QtQuick.Controls.Fusion // Use Fusion style by default
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
                onTriggered: Qt.callLater(() => AppController.about_menu())
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

            SourceList {
                anchors.fill: parent
                onOpenSourceDialogRequested: (typeId, dialogUrl) => mainWindowRoot.openSourceDialog(typeId, dialogUrl)
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

                ScreenRoot {
                    anchors.fill: parent
                }
            }

            GroupBox {
                id: controlPanelBox
                title: "Control panel"

                Layout.alignment: Qt.AlignTop

                ControlPanel {
                    anchors.fill: parent
                }
            }
        }
    }

    Loader {
        id: sourceDialogLoader
        onLoaded: {
            item.open();
            item.anchors.centerIn = Overlay.overlay;
        }
    }

    function openSourceDialog(typeId, dialogUrl) {
        sourceDialogLoader.setSource(dialogUrl, {
            typeId: typeId
        });
    }
}
