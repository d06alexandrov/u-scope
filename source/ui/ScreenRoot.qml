import QtQuick
import QtQuick.Layouts

Rectangle {
    id: screenRoot
    color: "black"

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        RowLayout {
            Rectangle {
                id: hScaleDisplay

                color: "black"

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
                        text: timebaseModel ? timebaseModel.hScaleText : ""
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
                    if (overviewChartController) {
                        overviewChartController.set_chart_width(overviewChart.plotArea.width);
                    }
                }
            }
        }

        MainChart {
            id: mainChart
            objectName: "mainChart"
            Layout.fillWidth: true
            Layout.fillHeight: true

            onWidthChanged: {
                if (mainChartController) {
                    mainChartController.set_chart_width(mainChart.width);
                }
            }
        }

        ChannelBar {
            objectName: "channelBar"
            Layout.fillWidth: true
            Layout.margins: 5
            channelBarModel: channelModel

            onChannelSelected: channelId => mainWindow.channel_selected(channelId)
            onChannelToggled: channelId => mainWindow.channel_toggled(channelId)
        }
    }
}
