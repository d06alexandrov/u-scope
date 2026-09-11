pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import UI.Readers
import UI.Readers.Serial

ReaderDialog {
    id: root

    title: qsTr("Serial Port Configuration")
    modal: true
    width: 420
    height: 480

    property SerialReaderDialogModel sessionModel: SerialReaderDialogModel {}

    acceptable: {
        if (root.sessionModel.portName == "") {
            // Port must be set
            return false;
        }
        if (parseInt(root.sessionModel.baudRate) < 9600) {
            // Minimum baudrate is 9600
            return false;
        }
        if (formatModeBox.currentIndex != 0) {
            // Packet mode
            if (root.sessionModel.packetLength <= 0) {
                // Packet length must be positive
                return false;
            }
            if (startMagicField.text == "") {
                // Start Magic HEX must be set
                return false;
            }
            if (fieldsList.count <= 0) {
                // At least one field must be set
                return false;
            }
        }

        return true;
    }

    onAccepted: root.configAccepted(root.typeId, root.sessionModel)

    PacketFieldDialog {
        id: fieldEditor

        packetLength: root.sessionModel.packetLength

        onFieldSubmitted: (variableId, name, offset, type, endianness) => {
            if (variableId >= 0) {
                root.sessionModel.fields.modifyField(variableId, name, offset, type, endianness);
            } else {
                root.sessionModel.fields.addField(name, offset, type, endianness);
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 10

        RowLayout {
            Layout.fillWidth: true

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
            Layout.fillWidth: true

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

        RowLayout {
            Layout.fillWidth: true

            Label {
                text: qsTr("Data Bits:")
            }
            ComboBox {
                id: dataBitsBox
                Layout.fillWidth: true
                model: [
                    {
                        value: SerialPort.Data5,
                        text: "5"
                    },
                    {
                        value: SerialPort.Data6,
                        text: "6"
                    },
                    {
                        value: SerialPort.Data7,
                        text: "7"
                    },
                    {
                        value: SerialPort.Data8,
                        text: "8"
                    }
                ]
                textRole: "text"
                valueRole: "value"
                currentValue: root.sessionModel.dataBits
                onActivated: root.sessionModel.dataBits = currentValue
            }
        }

        RowLayout {
            Layout.fillWidth: true

            Label {
                text: qsTr("Parity:")
            }
            ComboBox {
                id: parityBox
                Layout.fillWidth: true
                model: [
                    {
                        value: SerialPort.NoParity,
                        text: qsTr("None")
                    },
                    {
                        value: SerialPort.EvenParity,
                        text: qsTr("Even")
                    },
                    {
                        value: SerialPort.OddParity,
                        text: qsTr("Odd")
                    },
                    {
                        value: SerialPort.SpaceParity,
                        text: qsTr("Space")
                    },
                    {
                        value: SerialPort.MarkParity,
                        text: qsTr("Mark")
                    }
                ]
                textRole: "text"
                valueRole: "value"
                currentValue: root.sessionModel.parity
                onActivated: root.sessionModel.parity = currentValue
            }
        }

        RowLayout {
            Layout.fillWidth: true

            Label {
                text: qsTr("Stop bits:")
            }
            ComboBox {
                id: stopBitsBox
                Layout.fillWidth: true
                model: [
                    {
                        value: SerialPort.OneStop,
                        text: "1"
                    },
                    {
                        value: SerialPort.TwoStop,
                        text: "2"
                    }
                ]
                textRole: "text"
                valueRole: "value"
                currentValue: root.sessionModel.stopBits
                onActivated: root.sessionModel.stopBits = currentValue
            }
        }

        RowLayout {
            Layout.fillWidth: true

            Label {
                text: qsTr("Flow control:")
            }
            ComboBox {
                id: flowControlBox
                Layout.fillWidth: true
                model: [
                    {
                        value: SerialPort.NoFlowControl,
                        text: qsTr("No Flow Control")
                    },
                    {
                        value: SerialPort.HardwareControl,
                        text: qsTr("Hardware Control")
                    },
                    {
                        value: SerialPort.SoftwareControl,
                        text: qsTr("Software Control")
                    }
                ]
                textRole: "text"
                valueRole: "value"
                currentValue: root.sessionModel.flowControl
                onActivated: root.sessionModel.flowControl = currentValue
            }
        }

        RowLayout {
            Layout.fillWidth: true

            Label {
                text: qsTr("Data format:")
            }
            ComboBox {
                id: formatModeBox
                Layout.fillWidth: true
                model: [qsTr("Single byte"), qsTr("Packet")]
                currentIndex: root.sessionModel.formatMode == "packet" ? 1 : 0
                onActivated: root.sessionModel.formatMode = (currentIndex == 1) ? "packet" : "singleByte"
            }
        }

        StackLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true

            currentIndex: formatModeBox.currentIndex

            GroupBox {
                title: qsTr("Single Byte")

                ColumnLayout {
                    anchors.fill: parent

                    Label {
                        text: qsTr("Interprets each received byte as one sample.")
                        wrapMode: Text.WordWrap
                        Layout.fillWidth: true
                        opacity: 0.7
                    }

                    RowLayout {
                        Label {
                            text: qsTr("Signedness:")
                        }
                        RadioButton {
                            text: qsTr("Signed")
                            checked: root.sessionModel.signedByte
                            onToggled: {
                                if (checked) {
                                    root.sessionModel.signedByte = true;
                                }
                            }
                        }
                        RadioButton {
                            text: qsTr("Unsigned")
                            checked: !root.sessionModel.signedByte
                            onToggled: {
                                if (checked) {
                                    root.sessionModel.signedByte = false;
                                }
                            }
                        }
                    }

                    Item {
                        Layout.fillHeight: true
                    }
                }
            }

            GroupBox {
                title: qsTr("Packet")

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 8

                    GridLayout {
                        Layout.fillWidth: true
                        columns: 2
                        rowSpacing: 6
                        columnSpacing: 6

                        Label {
                            text: qsTr("Start magic (hex):")
                        }
                        TextField {
                            id: startMagicField
                            Layout.fillWidth: true
                            placeholderText: qsTr("e.g. AD 57")
                            text: root.sessionModel.startMagicHex
                            validator: RegularExpressionValidator {
                                regularExpression: /^([0-9A-Fa-f]{2}\s*)*$/
                            }
                            onEditingFinished: root.sessionModel.startMagicHex = text
                        }

                        Label {
                            text: qsTr("End magic (hex):")
                        }
                        TextField {
                            id: endMagicField
                            Layout.fillWidth: true
                            placeholderText: qsTr("e.g. 90 EF (optional)")
                            text: root.sessionModel.endMagicHex
                            validator: RegularExpressionValidator {
                                regularExpression: /^([0-9A-Fa-f]{2}\s*)*$/
                            }
                            onEditingFinished: root.sessionModel.endMagicHex = text
                        }

                        Label {
                            text: qsTr("Packet length (bytes):")
                        }
                        SpinBox {
                            id: packetLengthBox
                            Layout.fillWidth: true
                            editable: true
                            from: 1
                            to: 1024
                            value: root.sessionModel.packetLength
                            onValueModified: root.sessionModel.packetLength = value
                        }
                    }

                    Label {
                        text: qsTr("Fields:")
                    }

                    ListView {
                        id: fieldsList

                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        clip: true
                        model: root.sessionModel.fields

                        delegate: RowLayout {
                            id: fieldDelegate
                            required property int variableId
                            required property string label
                            width: ListView.view.width

                            Label {
                                Layout.fillWidth: true
                                text: fieldDelegate.label
                                elide: Text.ElideRight
                            }
                            Button {
                                text: qsTr("Edit")
                                onClicked: fieldEditor.openForEdit(fieldDelegate.variableId, root.sessionModel.fields.fieldAt(fieldDelegate.variableId))
                            }
                            Button {
                                text: qsTr("Delete")
                                onClicked: root.sessionModel.fields.removeField(fieldDelegate.variableId)
                            }
                        }
                    }

                    Button {
                        Layout.fillWidth: true
                        text: qsTr("+ Add field")
                        onClicked: fieldEditor.openForAdd()
                    }
                }
            }
        }
    }
}
