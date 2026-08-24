import QtQuick
import QtQuick.Controls
import UI

TreeView {
    id: root
    anchors.fill: parent

    model: AppController.sourceList.model

    Menu {
        id: backgroundMenu

        width: {
            var maxWidth = 0;
            for (var i = 0; i < count; ++i) {
                var item = itemAt(i);
                if (item && item.contentItem) {
                    maxWidth = Math.max(maxWidth, item.contentItem.implicitWidth);
                }
            }
            return Math.max(implicitWidth, maxWidth + leftPadding + rightPadding + 30);
        }

        MenuItem {
            text: "Configure new simulated source"
            onTriggered: {
                Qt.callLater(() => AppController.sourceList.open_simulated_source_dialog(AppController));
            }
        }
        MenuItem {
            text: "Configure new serial port source"
            onTriggered: {
                Qt.callLater(() => AppController.sourceList.open_serial_source_dialog(AppController));
            }
        }
    }

    TapHandler {
        acceptedButtons: Qt.RightButton
        onTapped: backgroundMenu.popup()
    }

    delegate: TreeViewDelegate {
        implicitWidth: root.width

        Menu {
            // Menu for reader nodes
            id: readerMenu

            MenuItem {
                text: "Delete existing source"
                onTriggered: {
                    AppController.sourceList.delete_source(readerId);
                }
            }
        }

        Menu {
            // Menu for variable nodes
            id: variableMenu

            Menu {
                title: "Assign to channel"

                Repeater {
                    model: 12
                    MenuItem {
                        text: "Channel " + (index + 1)
                        onTriggered: {
                            AppController.sourceList.assign_variable_to_channel(readerId, variableId, index);
                        }
                    }
                }
            }
        }

        TapHandler {
            acceptedButtons: Qt.RightButton
            gesturePolicy: TapHandler.WithinBounds
            onTapped: {
                if (depth == 0) {
                    readerMenu.popup();
                } else {
                    variableMenu.popup();
                }
            }
        }
    }
}
