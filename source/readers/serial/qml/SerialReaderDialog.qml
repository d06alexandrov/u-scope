import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import UI.Readers
import UI.Readers.Serial

ReaderDialog {
    id: root

    property SerialReaderDialogModel sessionModel: SerialReaderDialogModel {}

    title: qsTr("Serial Port Configuration")
    modal: true

    footer: DialogButtonBox {
        Button {
            text: root.mode === "modify" ? qsTr("Modify") : qsTr("OK")
            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
        }
        Button {
            text: qsTr("Cancel")
            DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
        }
    }

    ColumnLayout {
        anchors.fill: parent

        RowLayout {
            Label {
                text: qsTr("Device:")
            }
            ComboBox {
                Layout.fillWidth: true
                model: root.sessionModel.availablePorts
                currentIndex: indexOfValue(root.sessionModel.portName)
                onActivated: root.sessionModel.portName = currentText
            }
        }

        RowLayout {
            Label {
                text: qsTr("Baudrate:")
            }
            ComboBox {
                id: baudRateBox
                Layout.fillWidth: true
                editable: true
                model: [9600, 19200, 38400, 57600, 115200]
                editText: root.sessionModel.baudRate
                onEditTextChanged: {
                    const parsed = parseInt(editText);
                    if (!isNaN(parsed))
                        root.sessionModel.baudRate = parsed;
                }
            }
        }
    }

    onAccepted: root.configAccepted(root.typeId, root.sessionModel)
}
