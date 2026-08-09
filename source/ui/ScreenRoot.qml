import QtQuick
import QtQuick.Layouts

Rectangle {
    id: screenRoot
    color: "black"

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        MainChart {
            objectName: "mainChart"
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        ChannelBar {
            objectName: "channelBar"
            Layout.fillWidth: true
            Layout.margins: 5
            channelBarModel: channelModel

            onChannelSelected: (channelId) => mainWindow.channel_selected(channelId)
            onChannelToggled: (channelId) => mainWindow.channel_toggled(channelId)
        }
    }
}
