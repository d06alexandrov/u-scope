// SPDX-FileCopyrightText: 2026 Dmitriy Alexandrov <d.alexandrov@dalogik.com>
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick.Controls

Dialog {
    id: root

    anchors.centerIn: Overlay.overlay

    required property string typeId
    property string mode: "add"
    property bool acceptable: false

    signal configAccepted(string typeId, var sessionModel)

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
