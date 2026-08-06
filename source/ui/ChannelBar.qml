import QtQuick
import QtQuick.Layouts
import QtQuick.Shapes

Flow {
    id: flowRoot

    required property var channelBarModel

    signal channelSelected(int channelNumber)
    signal channelToggled(int channelNumber)

    width: parent ? parent.width : 0
    spacing: 5

    Repeater {
        model: flowRoot.channelBarModel

        ChannelBadge {
            onBadgeClicked: channelNumber => {
                flowRoot.channelSelected(channelNumber);
            }
            onBadgeDoubleClicked: channelNumber => {
                flowRoot.channelToggled(channelNumber);
            }
        }
    }

    component ChannelBadge: Item {
        id: badgeRoot

        signal badgeClicked(int channelNumber)
        signal badgeDoubleClicked(int channelNumber)

        required property int channelNumber
        required property string valueText
        required property color badgeColor
        required property bool channelEnabled

        implicitWidth: Math.max(100, 28 + valueTextItem.implicitWidth + 16)
        implicitHeight: 22

        antialiasing: true

        Timer {
            id: clickTimer
            interval: Qt.styleHints.mouseDoubleClickInterval
            repeat: false
            onTriggered: {
                badgeRoot.badgeClicked(badgeRoot.channelNumber);
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
                badgeRoot.badgeDoubleClicked(badgeRoot.channelNumber);
            }
        }

        Shape {
            anchors.fill: parent
            layer.enabled: true
            layer.samples: 4

            ShapePath {
                fillColor: "#1A1A1A"
                strokeColor: badgeRoot.channelEnabled ? badgeRoot.badgeColor : "#646464"
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
                fillColor: badgeRoot.channelEnabled ? badgeRoot.badgeColor : "#646464"
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

                        text: badgeRoot.channelEnabled ? badgeRoot.valueText : "OFF"
                        color: badgeRoot.channelEnabled ? badgeRoot.badgeColor : "#969696"
                        font.pixelSize: 11
                        font.bold: true
                        font.family: "Monospace"
                    }
                }
            }
        }
    }
}
