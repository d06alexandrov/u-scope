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
            Layout.fillWidth: true
            Layout.margins: 5
            channelBarModel: channelModel
        }
    }
}
