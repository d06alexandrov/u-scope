import QtQuick.Controls

Dialog {
    id: root

    anchors.centerIn: Overlay.overlay

    required property string typeId
    property string mode: "add"
    property bool acceptable: false

    signal configAccepted(string typeId, var sessionModel)

    onAccepted: root.configAccepted(root.typeId, root.sessionModel)

    footer: DialogButtonBox {
        Button {
            enabled: root.acceptable
            text: root.mode === "modify" ? qsTr("Modify") : qsTr("OK")
            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
        }
        Button {
            text: qsTr("Cancel")
            DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
        }
    }
}
