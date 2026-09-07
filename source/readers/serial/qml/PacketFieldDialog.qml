import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    id: popup

    anchors.centerIn: Overlay.overlay

    required property int packetLength

    property int editingVariableId: -1

    readonly property var typeNames: ["Int8", "UInt8", "Int16", "UInt16", "Int32", "UInt32"]
    readonly property var typeSizes: ({
            "Int8": 1,
            "UInt8": 1,
            "Int16": 2,
            "UInt16": 2,
            "Int32": 4,
            "UInt32": 4
        })

    signal fieldSubmitted(int fieldIndex, string name, int offset, string type, string endianness)

    title: qsTr("Packet Field")
    modal: true

    function resetToDefault() {
        editingVariableId = -1;
        fieldName.text = "";
        fieldOffset.value = 0;
        fieldType.currentIndex = 0;
        fieldEndianness.currentIndex = 0;
    }

    function openForAdd() {
        resetToDefault();
        open();
    }

    function openForEdit(variableId, field) {
        resetToDefault();

        editingVariableId = variableId;
        fieldName.text = field.name;
        fieldOffset.value = field.offset;

        const typeIdx = typeNames.indexOf(field.type);
        fieldType.currentIndex = typeIdx >= 0 ? typeIdx : 0;

        fieldEndianness.currentIndex = field.endianness === "Big" ? 1 : 0;

        open();
    }

    readonly property bool fieldFitsPacket: (fieldOffset.value + typeSizes[fieldType.currentText]) <= popup.packetLength

    footer: DialogButtonBox {
        Button {
            text: qsTr("OK")
            enabled: (fieldName.text !== "") && popup.fieldFitsPacket
            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
        }
        Button {
            text: qsTr("Cancel")
            DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
        }
    }

    onAccepted: {
        popup.fieldSubmitted(editingVariableId, fieldName.text, fieldOffset.value, fieldType.currentText, fieldEndianness.currentText);
    }

    GridLayout {
        anchors.fill: parent
        columns: 2
        rowSpacing: 6
        columnSpacing: 6

        Label {
            text: qsTr("Name")
        }
        TextField {
            id: fieldName
            Layout.fillWidth: true
            placeholderText: qsTr("e.g. Voltage")
        }

        Label {
            text: qsTr("Offset (bytes)")
        }
        SpinBox {
            id: fieldOffset
            Layout.fillWidth: true
            editable: true
            from: 0
            to: Math.max(0, popup.packetLength - 1)
        }

        Label {
            text: qsTr("Type")
        }
        ComboBox {
            id: fieldType
            Layout.fillWidth: true
            model: popup.typeNames
        }

        Label {
            text: qsTr("Endianness")
            visible: popup.typeSizes[fieldType.currentText] > 1
        }
        ComboBox {
            id: fieldEndianness
            Layout.fillWidth: true
            visible: popup.typeSizes[fieldType.currentText] > 1
            model: ["Little", "Big"]
        }

        Label {
            Layout.columnSpan: 2
            visible: !popup.fieldFitsPacket
            color: "red"
            text: qsTr("Field extends past the packet length.")
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }
    }
}
