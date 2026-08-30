import QtQuick.Controls

Dialog {
    id: root

    anchors.centerIn: Overlay.overlay

    required property string typeId
    property string mode: "add"

    signal configAccepted(string typeId, var sessionModel)
}
