import QtQuick
import QtQuick.Layouts
import QtQuick.Shapes

Flow {
    id: flowRoot

    required property var channelBarModel

    signal channelSelected(int channelId)
    signal channelToggled(int channelId)

    width: parent ? parent.width : 0
    spacing: 5

    Repeater {
        model: flowRoot.channelBarModel

        ChannelBadge {
            onBadgeClicked: channelId => {
                flowRoot.channelSelected(channelId);
            }
            onBadgeDoubleClicked: channelId => {
                flowRoot.channelToggled(channelId);
            }
        }
    }

    component ChannelBadge: Item {
        id: badgeRoot

        signal badgeClicked(int channelId)
        signal badgeDoubleClicked(int channelId)

        required property int channelId

        // ChannelNumber is a displayed number
        required property int channelNumber
        required property string valueText
        required property color badgeColor
        required property bool channelConnected
        required property bool channelEnabled
        required property bool channelSelected

        readonly property color currentAccentColor: {
            if (!badgeRoot.channelConnected) {
                return "#646464";
            }
            if (!badgeRoot.channelSelected) {
                return Qt.darker(badgeRoot.badgeColor, 2.5);
            }
            return badgeRoot.badgeColor;
        }

        implicitWidth: Math.max(100, 28 + valueTextItem.implicitWidth + 16)
        implicitHeight: 22

        antialiasing: true

        Timer {
            id: clickTimer
            interval: Qt.styleHints.mouseDoubleClickInterval
            repeat: false
            onTriggered: {
                badgeRoot.badgeClicked(badgeRoot.channelId);
            }
        }

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor

            onClicked: {
                clickTimer.start();
            }

            onDoubleClicked: {
                clickTimer.stop();
                badgeRoot.badgeDoubleClicked(badgeRoot.channelId);
            }
        }

        Shape {
            anchors.fill: parent
            layer.enabled: true
            layer.samples: 4

            ShapePath {
                fillColor: "#1A1A1A"
                strokeColor: badgeRoot.currentAccentColor
                strokeWidth: 2

                PathMove {
                    x: 0
                    y: badgeRoot.height
                }
                PathLine {
                    x: 8
                    y: 0
                }
                PathLine {
                    x: badgeRoot.width
                    y: 0
                }
                PathLine {
                    x: badgeRoot.width - 8
                    y: badgeRoot.height
                }
                PathLine {
                    x: 0
                    y: badgeRoot.height
                }
            }

            ShapePath {
                fillColor: badgeRoot.currentAccentColor
                strokeColor: "transparent"

                PathMove {
                    x: 0
                    y: badgeRoot.height
                }
                PathLine {
                    x: 8
                    y: 0
                }
                PathLine {
                    x: 32
                    y: 0
                }
                PathLine {
                    x: 24
                    y: badgeRoot.height
                }
                PathLine {
                    x: 0
                    y: badgeRoot.height
                }
            }

            RowLayout {
                anchors.fill: parent
                spacing: 0

                Item {
                    Layout.fillHeight: true
                    Layout.preferredWidth: 28

                    Text {
                        anchors.centerIn: parent
                        // Place in the middle of the left trapezoid
                        anchors.horizontalCenterOffset: 2

                        text: badgeRoot.channelNumber
                        font.bold: true
                        font.pixelSize: 11
                        color: "black"
                    }
                }

                Item {
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    Text {
                        id: valueTextItem

                        anchors.centerIn: parent
                        // Place in the middle of the right trapezoid
                        anchors.horizontalCenterOffset: -2

                        text: {
                            if (!badgeRoot.channelConnected) {
                                return "DISCONNECTED";
                            }
                            if (!badgeRoot.channelEnabled) {
                                return "OFF";
                            }
                            return badgeRoot.valueText;
                        }
                        color: badgeRoot.currentAccentColor
                        font.pixelSize: 11
                        font.bold: true
                        font.family: "Monospace"
                    }
                }
            }
        }
    }
}
