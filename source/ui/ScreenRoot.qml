import QtQuick
import QtQuick.Layouts

Rectangle {
    id: screenRoot
    color: "black"

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        OverviewChart {
            id: overviewChart
            objectName: "overviewChart"
            Layout.fillWidth: true

            onWidthChanged: {
                overviewChartController.set_chart_width(overviewChart.plotArea.width);
            }
        }

        MainChart {
            id: mainChart
            objectName: "mainChart"
            Layout.fillWidth: true
            Layout.fillHeight: true

            onWidthChanged: {
                mainChartController.set_chart_width(mainChart.width);
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
