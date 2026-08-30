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
        constantValue.setRealValue(0);
        sinusoidalAmplitude.setRealValue(1);
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
            sinusoidalAmplitude.setRealValue(form.amplitude);
        } else {
            graphType.currentIndex = 0;
            constantValue.setRealValue(form.value);
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
            formConstantSubmitted(editingVariableId, constantValue.realValue);
        } else {
            formSinusoidSubmitted(editingVariableId, sinusoidalFrequency.value, sinusoidalAmplitude.realValue);
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
                DecimalSpinBox {
                    id: constantValue

                    editable: true
                    realFrom: -1000
                    realTo: 1000
                }
            }

            GridLayout {
                columns: 2
                Label {
                    text: qsTr("Amplitude")
                }
                DecimalSpinBox {
                    id: sinusoidalAmplitude

                    editable: true
                    realFrom: 1
                    realTo: 1000
                }
                Label {
                    text: qsTr("Frequency (Hz)")
                }
                SpinBox {
                    id: sinusoidalFrequency

                    editable: true
                    from: 1
                    to: 1000000
                }
            }
        }
    }
}
