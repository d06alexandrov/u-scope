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

    acceptable: (root.sessionModel.portName !== "") && (parseInt(root.sessionModel.baudRate) >= 9600)

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
                text: qsTr("Data format:")
            }
            ComboBox {
                id: formatModeBox
                Layout.fillWidth: true
                model: [qsTr("Single byte"), qsTr("Packet")]
                currentIndex: root.sessionModel.formatMode === "packet" ? 1 : 0
                onActivated: root.sessionModel.formatMode = (currentIndex === 1) ? "packet" : "singleByte"
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
                            onToggled: if (checked)
                                root.sessionModel.signedByte = true
                        }
                        RadioButton {
                            text: qsTr("Unsigned")
                            checked: !root.sessionModel.signedByte
                            onToggled: if (checked)
                                root.sessionModel.signedByte = false
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
