import QtQuick
import QtQuick.Controls

TreeView {
    id: root
    anchors.fill: parent

    model: sourceListController ? sourceListController.model : null

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
                if (sourceListController) {
                    Qt.callLater(() => sourceListController.open_simulated_source_dialog(mainWindow));
                }
            }
        }
        MenuItem {
            text: "Configure new serial port source"
            onTriggered: {
                if (sourceListController) {
                    Qt.callLater(() => sourceListController.open_serial_source_dialog(mainWindow));
                }
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
                    if (sourceListController) {
                        sourceListController.delete_source(readerId);
                    }
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
                            if (sourceListController) {
                                sourceListController.assign_variable_to_channel(readerId, variableId, index);
                            }
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
