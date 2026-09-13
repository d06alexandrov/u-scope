import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ColumnLayout {
    id: root
    spacing: 12

    property int dialPreferredSize: 80
    property int dialMaximumSize: 100

    GroupBox {
        id: triggerConfig
        title: qsTr("Trigger")
        Layout.fillWidth: true

        GridLayout {
            anchors.fill: parent
            columns: 2
            rowSpacing: 6
            columnSpacing: 6

            Button {
                id: pushButton_StartAll
                text: qsTr("Start")
                Layout.fillWidth: true
                enabled: AppController.stopped
                onClicked: AppController.handle_start_clicked()
            }

            Button {
                id: pushButton_StopAll
                text: qsTr("Stop")
                Layout.fillWidth: true
                enabled: !AppController.stopped
                onClicked: AppController.handle_stop_clicked()
            }
        }
    }

    GroupBox {
        id: horizontalConfig
        title: qsTr("Horizontal")
        enabled: true
        Layout.fillWidth: true

        RowLayout {
            anchors.fill: parent
            spacing: 12

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 4

                Label {
                    text: qsTr("Scale")
                    Layout.alignment: Qt.AlignHCenter
                }

                Dial {
                    id: horizontalScale
                    Layout.preferredWidth: root.dialPreferredSize
                    Layout.preferredHeight: root.dialPreferredSize
                    Layout.maximumWidth: root.dialMaximumSize
                    Layout.maximumHeight: root.dialMaximumSize
                    Layout.alignment: Qt.AlignHCenter

                    from: -21
                    to: -4
                    stepSize: 1.0

                    value: AppController.timebaseModel.qDialValue

                    onMoved: {
                        AppController.timebaseModel.qDialValue = value;
                    }

                    Connections {
                        target: AppController.timebaseModel
                        function onQDialValueChanged() {
                            horizontalScale.value = AppController.timebaseModel.qDialValue;
                        }
                    }
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 4

                enabled: AppController.overviewChart.visible

                Label {
                    text: qsTr("Position")
                    Layout.alignment: Qt.AlignHCenter
                }

                Dial {
                    id: horizontalPosition
                    Layout.preferredWidth: root.dialPreferredSize
                    Layout.preferredHeight: root.dialPreferredSize
                    Layout.maximumWidth: root.dialMaximumSize
                    Layout.maximumHeight: root.dialMaximumSize
                    Layout.alignment: Qt.AlignHCenter

                    startAngle: -180
                    endAngle: 180

                    from: 0.0
                    to: 29.0
                    value: 15.0
                    stepSize: 1.0
                    wrap: true

                    onEnabledChanged: {
                        // Fusion style hack to force background repaint when enabled
                        if (background) {
                            const className = String(background);
                            if (className.includes("Fusion")) {
                                background.update();
                            }
                        }
                    }

                    property real previousValue: value

                    onMoved: {
                        let delta = value - previousValue;
                        let range = to - from + 1.0;

                        if (delta > range / 2.0) {
                            delta -= range;
                        } else if (delta < -range / 2.0) {
                            delta += range;
                        }

                        previousValue = value;

                        AppController.overviewChart.moveSlidingWindow(Math.round(delta));
                    }
                }
            }
        }
    }

    GroupBox {
        id: verticalConfig
        title: qsTr("Vertical")
        Layout.fillWidth: true

        enabled: AppController.channelModel.selectedChannel >= 0

        RowLayout {
            anchors.fill: parent
            spacing: 12

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 4

                Label {
                    text: qsTr("Scale")
                    Layout.alignment: Qt.AlignHCenter
                }

                Dial {
                    id: verticalScale
                    Layout.preferredWidth: root.dialPreferredSize
                    Layout.preferredHeight: root.dialPreferredSize
                    Layout.maximumWidth: root.dialMaximumSize
                    Layout.maximumHeight: root.dialMaximumSize
                    Layout.alignment: Qt.AlignHCenter

                    from: -36
                    to: -4
                    stepSize: 1.0

                    value: {
                        if (AppController.channelModel.selectedChannel >= 0) {
                            return AppController.verticalScaleModel.qDialValue(AppController.channelModel.selectedChannel);
                        } else {
                            return -4;
                        }
                    }

                    onEnabledChanged: {
                        // Fusion style hack to force background repaint when enabled
                        if (background) {
                            const className = String(background);
                            if (className.includes("Fusion")) {
                                background.update();
                            }
                        }
                    }

                    onMoved: {
                        if (AppController.channelModel.selectedChannel >= 0) {
                            AppController.verticalScaleModel.qDialValueUpdate(AppController.channelModel.selectedChannel, value);
                        }
                    }

                    Connections {
                        target: AppController.verticalScaleModel
                        function onVDivisionChanged() {
                            if (AppController.channelModel.selectedChannel >= 0) {
                                verticalScale.value = AppController.verticalScaleModel.qDialValue(AppController.channelModel.selectedChannel);
                            }
                        }
                    }

                    Connections {
                        target: AppController.channelModel
                        function onSelectedChannelChanged() {
                            if (AppController.channelModel.selectedChannel >= 0) {
                                verticalScale.value = AppController.verticalScaleModel.qDialValue(AppController.channelModel.selectedChannel);
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
                    text: qsTr("Position")
                    Layout.alignment: Qt.AlignHCenter
                }

                Dial {
                    id: dial_4
                    Layout.preferredWidth: root.dialPreferredSize
                    Layout.preferredHeight: root.dialPreferredSize
                    Layout.maximumWidth: root.dialMaximumSize
                    Layout.maximumHeight: root.dialMaximumSize
                    Layout.alignment: Qt.AlignHCenter

                    onEnabledChanged: {
                        // Fusion style hack to force background repaint when enabled
                        if (background) {
                            const className = String(background);
                            if (className.includes("Fusion")) {
                                background.update();
                            }
                        }
                    }
                }
            }
        }
    }
}
