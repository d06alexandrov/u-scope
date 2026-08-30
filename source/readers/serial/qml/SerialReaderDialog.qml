import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import UI.Readers
import UI.Readers.Serial

ReaderDialog {
    id: root

    title: qsTr("Serial Port Configuration")
    modal: true

    property SerialReaderDialogModel sessionModel: SerialReaderDialogModel {}

    acceptable: (root.sessionModel.portName !== "") && (parseInt(root.sessionModel.baudRate) >= 9600)

    onAccepted: root.configAccepted(root.typeId, root.sessionModel)

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
}
