pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import UI
import UI.Simulated

Dialog {
    id: root

    required property string typeId
    property string mode: "add"

    property SimulatedSourceDialogModel sessionModel: SimulatedSourceDialogModel {}

    title: qsTr("Simulated Reader Configuration")
    modal: true
    width: 340
    height: 420

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

    onAccepted: AppController.sourceList.configSource(root.typeId, root.sessionModel)

    SimulatedFormDialog {
        id: formEditor

        onFormConstantSubmitted: (variableId, value) => {
            if (variableId >= 0) {
                root.sessionModel.modifyConstantForm(variableId, value);
            } else {
                root.sessionModel.addConstantForm(value);
            }
        }

        onFormSinusoidSubmitted: (variableId, frequency, amplitude) => {
            if (variableId >= 0) {
                root.sessionModel.modifySinusoidForm(variableId, frequency, amplitude);
            } else {
                root.sessionModel.addSinusoidForm(frequency, amplitude);
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: root.sessionModel

            delegate: RowLayout {
                required property int variableId
                required property string label
                width: ListView.view.width

                Label {
                    Layout.fillWidth: true
                    text: label
                }
                Button {
                    text: qsTr("Edit")
                    onClicked: formEditor.openForEdit(variableId, root.sessionModel.formAt(variableId))
                }
                Button {
                    text: qsTr("Delete")
                    onClicked: root.sessionModel.removeForm(variableId)
                }
            }
        }

        Button {
            Layout.fillWidth: true
            text: qsTr("+ Add new form")
            onClicked: formEditor.openForAdd()
        }
    }
}
