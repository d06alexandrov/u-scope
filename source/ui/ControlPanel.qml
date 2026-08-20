import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ColumnLayout {
    id: root
    spacing: 12

    GroupBox {
        id: triggerConfig
        title: "Trigger"
        Layout.fillWidth: true

        GridLayout {
            anchors.fill: parent
            columns: 2
            rowSpacing: 6
            columnSpacing: 6

            Button {
                id: pushButton_StartAll
                text: "Start"
                Layout.fillWidth: true
                onClicked: appController.handle_start_clicked()
            }

            Button {
                id: pushButton_StopAll
                text: "Stop"
                Layout.fillWidth: true
                onClicked: appController.handle_stop_clicked()
            }
        }
    }

    GroupBox {
        id: horizontalConfig
        title: "Horizontal"
        enabled: true
        Layout.fillWidth: true

        RowLayout {
            anchors.fill: parent
            spacing: 12

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 4

                Label {
                    text: "Scale"
                    Layout.alignment: Qt.AlignHCenter
                }

                Dial {
                    id: horizontalScale
                    Layout.preferredWidth: 80
                    Layout.preferredHeight: 80
                    Layout.maximumWidth: 100
                    Layout.maximumHeight: 100
                    Layout.alignment: Qt.AlignHCenter

                    from: -21
                    to: -4

                    value: timebaseModel ? timebaseModel.qDialValue : -4

                    onMoved: {
                        if (timebaseModel) {
                            timebaseModel.qDialValue = value;
                        }
                    }

                    Connections {
                        target: timebaseModel
                        function onQDialValueChanged() {
                            if (timebaseModel) {
                                horizontalScale.value = timebaseModel.qDialValue;
                            }
                        }
                    }
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 4

                Label {
                    text: "Position"
                    Layout.alignment: Qt.AlignHCenter
                }

                Dial {
                    id: dial_2
                    Layout.preferredWidth: 80
                    Layout.preferredHeight: 80
                    Layout.maximumWidth: 100
                    Layout.maximumHeight: 100

                    enabled: false
                    Layout.alignment: Qt.AlignHCenter
                }
            }
        }
    }

    GroupBox {
        id: verticalConfig
        title: "Vertical"
        Layout.fillWidth: true

        enabled: channelModel && (channelModel.selectedChannel >= 0)

        RowLayout {
            anchors.fill: parent
            spacing: 12

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 4

                Label {
                    text: "Scale"
                    Layout.alignment: Qt.AlignHCenter
                }

                Dial {
                    id: verticalScale
                    Layout.preferredWidth: 80
                    Layout.preferredHeight: 80
                    Layout.maximumWidth: 100
                    Layout.maximumHeight: 100
                    Layout.alignment: Qt.AlignHCenter

                    from: -24
                    to: -4

                    value: {
                        if (channelModel && (channelModel.selectedChannel >= 0) && verticalScaleModel) {
                            return verticalScaleModel.qDialValue(channelModel.selectedChannel);
                        } else {
                            return -4;
                        }
                    }

                    onMoved: {
                        if (verticalScaleModel && channelModel && channelModel.selectedChannel >= 0) {
                            verticalScaleModel.qDialValueUpdate(channelModel.selectedChannel, value);
                        }
                    }

                    Connections {
                        target: verticalScaleModel
                        function onVDivisionChanged() {
                            if (channelModel && channelModel.selectedChannel >= 0) {
                                verticalScale.value = verticalScaleModel.qDialValue(channelModel.selectedChannel);
                            }
                        }
                    }

                    Connections {
                        target: channelModel
                        function onSelectedChannelChanged() {
                            if (channelModel && channelModel.selectedChannel >= 0 && verticalScaleModel) {
                                verticalScale.value = verticalScaleModel.qDialValue(channelModel.selectedChannel);
                            }
                        }
                    }
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 4

                enabled: false

                Label {
                    text: "Position"
                    Layout.alignment: Qt.AlignHCenter
                }

                Dial {
                    id: dial_4
                    Layout.preferredWidth: 80
                    Layout.preferredHeight: 80
                    Layout.maximumWidth: 100
                    Layout.maximumHeight: 100
                    Layout.alignment: Qt.AlignHCenter
                }
            }
        }
    }
}
