import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    id: popup

    anchors.centerIn: Overlay.overlay

    property int editingVariableId: -1

    signal formConstantSubmitted(int variableId, real value)
    signal formSinusoidSubmitted(int variableId, int frequency, real amplitude)

    title: qsTr("Simulated Form")
    modal: true

    function resetToDefault() {
        editingVariableId = -1;
        graphType.currentIndex = 0;
        constantValue.value = 0;
        sinusoidalAmplitude.value = 1;
        sinusoidalFrequency.value = 1;
    }

    function openForAdd() {
        resetToDefault();
        open();
    }

    function openForEdit(variableId, form) {
        resetToDefault();

        editingVariableId = variableId;
        if (form.type === "sinusoid") {
            graphType.currentIndex = 1;
            sinusoidalFrequency.value = form.frequency;
            sinusoidalAmplitude.value = form.amplitude;
        } else {
            graphType.currentIndex = 0;
            constantValue.value = form.value;
        }
        open();
    }

    footer: DialogButtonBox {
        Button {
            text: qsTr("OK")
            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
        }
        Button {
            text: qsTr("Cancel")
            DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
        }
    }

    onAccepted: {
        if (graphType.currentIndex == 0) {
            formConstantSubmitted(editingVariableId, constantValue.value);
        } else {
            formSinusoidSubmitted(editingVariableId, sinusoidalFrequency.value, sinusoidalAmplitude.value);
        }
    }

    ColumnLayout {
        anchors.fill: parent

        ComboBox {
            id: graphType
            Layout.fillWidth: true
            model: [qsTr("Constant Value"), qsTr("Sinusoidal Wave")]
        }

        StackLayout {
            currentIndex: graphType.currentIndex

            GridLayout {
                columns: 2
                Label {
                    text: qsTr("Value")
                }
                SpinBox {
                    id: constantValue
                    from: -1000
                    to: 1000
                }
            }

            GridLayout {
                columns: 2
                Label {
                    text: qsTr("Amplitude")
                }
                SpinBox {
                    id: sinusoidalAmplitude
                    from: 1
                    to: 1000
                    value: 1
                }
                Label {
                    text: qsTr("Frequency (Hz)")
                }
                SpinBox {
                    id: sinusoidalFrequency
                    from: 1
                    to: 1000000
                }
            }
        }
    }
}
