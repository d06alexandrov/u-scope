import QtQuick
import QtQuick.Layouts

Rectangle {
    id: screenRoot
    color: "black"
    radius: 4

    ColumnLayout {
        anchors.fill: parent

        anchors.margins: 4
        spacing: 0

        RowLayout {
            Rectangle {
                id: hScaleDisplay

                color: "black"
                radius: 4

                border.color: "grey"
                border.width: 1

                Layout.preferredHeight: 40
                Layout.preferredWidth: hScaleLayout.implicitWidth + 16

                RowLayout {
                    id: hScaleLayout
                    anchors.centerIn: parent
                    spacing: 4

                    Text {
                        text: "H"
                        color: "white"
                        font.pixelSize: 12
                    }

                    Text {
                        id: hScaleValue
                        objectName: "hScaleValue"
                        text: AppController.timebaseModel.hScaleText
                        color: "white"
                        horizontalAlignment: Text.AlignHCenter

                        Layout.minimumWidth: 50
                        font.pixelSize: 12
                    }
                }
            }

            OverviewChart {
                id: overviewChart
                objectName: "overviewChart"
                Layout.fillWidth: true

                onWidthChanged: {
                    AppController.overviewChart.set_chart_width(overviewChart.plotArea.width);
                }
            }
        }

        MainChart {
            id: mainChart
            objectName: "mainChart"
            Layout.fillWidth: true
            Layout.fillHeight: true

            onWidthChanged: {
                AppController.mainChart.set_chart_width(mainChart.width);
            }
        }

        ChannelBar {
            objectName: "channelBar"
            Layout.fillWidth: true
            Layout.margins: 5
            channelBarModel: AppController.channelModel

            onChannelSelected: channelId => AppController.channel_selected(channelId)
            onChannelToggled: channelId => AppController.channel_toggled(channelId)
        }
    }
}
