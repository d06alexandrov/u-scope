pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import UI.Readers
import UI.Readers.Simulated

ReaderDialog {
    id: root

    title: qsTr("Simulated Reader Configuration")
    modal: true
    width: 340
    height: 420

    property SimulatedReaderDialogModel sessionModel: SimulatedReaderDialogModel {}

    acceptable: formsList.count > 0

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
            id: formsList

            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: root.sessionModel

            delegate: RowLayout {
                id: formDelegate
                required property int variableId
                required property string label
                width: ListView.view.width

                Label {
                    Layout.fillWidth: true
                    text: formDelegate.label
                }
                Button {
                    text: qsTr("Edit")
                    onClicked: formEditor.openForEdit(formDelegate.variableId, root.sessionModel.formAt(formDelegate.variableId))
                }
                Button {
                    text: qsTr("Delete")
                    onClicked: root.sessionModel.removeForm(formDelegate.variableId)
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
