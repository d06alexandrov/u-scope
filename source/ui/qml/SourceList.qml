pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls

TreeView {
    id: root
    anchors.fill: parent

    model: AppController.sourceList.model

    Menu {
        id: backgroundMenu

        width: {
            var maxWidth = 0;
            for (var i = 0; i < count; ++i) {
                var item = itemAt(i) as MenuItem;
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
        id: sourceDelegate
        implicitWidth: root.width

        required property int readerId
        required property int variableId

        Menu {
            // Menu for reader nodes
            id: readerMenu

            MenuItem {
                text: "Delete existing source"
                onTriggered: {
                    AppController.sourceList.delete_source(sourceDelegate.readerId);
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
                        id: channelMenuItem

                        required property int index

                        text: "Channel " + (channelMenuItem.index + 1)
                        onTriggered: {
                            AppController.sourceList.assign_variable_to_channel(sourceDelegate.readerId, sourceDelegate.variableId, channelMenuItem.index);
                        }
                    }
                }
            }
        }

        TapHandler {
            acceptedButtons: Qt.RightButton
            gesturePolicy: TapHandler.WithinBounds
            onTapped: {
                if (parent.depth == 0) {
                    readerMenu.popup();
                } else {
                    variableMenu.popup();
                }
            }
        }
    }
}
