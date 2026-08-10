import QtQuick
import QtCharts
import QtQuick.Layouts

ChartView {
    id: root
    objectName: "overviewChart"

    // UI Translation: "dataOverview" has vsizetype="Fixed" and maximumSize.height="40"[cite: 9]
    Layout.fillWidth: true
    Layout.maximumHeight: 40
    Layout.preferredHeight: 40

    // Design properties mirroring MainChart.qml[cite: 8] and UI stylesheet[cite: 9]
    backgroundColor: "transparent"
    legend.visible: false
    antialiasing: true
    margins {
        top: 0
        bottom: 0
        left: 0
        right: 0
    }

    ValueAxis {
        id: plotAxisX
        objectName: "overviewAxisX"
        gridVisible: false
        labelsVisible: false
        lineVisible: false
    }

    ValueAxis {
        id: plotAxisY
        objectName: "overviewAxisY"
        min: -100
        max: 100
        gridVisible: false
        labelsVisible: false
        lineVisible: false
    }

    function getAxisX(): ValueAxis {
        return plotAxisX;
    }

    function getSeries(index: int): AbstractSeries {
        return root.series(index);
    }

    Component.onCompleted: {
        for (var i = 0; i < 12; ++i) {
            var series = root.createSeries(ChartView.SeriesTypeLine, "", plotAxisX, plotAxisY);

            series.objectName = "overview_series_" + i;
            series.color = (cppChannelColors.length > i) ? cppChannelColors[i] : "white";
            series.pointsVisible = false; 
        }
    }
}
