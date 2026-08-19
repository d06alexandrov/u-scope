import QtQuick
import QtCharts

ChartView {
    id: root
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
        objectName: "plotAxisX"
        gridVisible: true
        gridLineColor: Qt.rgba(0, 1, 0, 100 / 255)
        labelsVisible: false
        lineVisible: false
        tickCount: timebaseModel ? timebaseModel.hGridCells + 1 : 2

        Component.onCompleted: mainChartController.registerXAxis(plotAxisX)
    }

    ValueAxis {
        id: plotAxisY
        objectName: "plotAxisY"
        min: -plotAxisY.max
        max: verticalScaleModel ? verticalScaleModel.vGridCells / 2 : 1
        gridVisible: true
        gridLineColor: Qt.rgba(0, 1, 0, 100 / 255)
        labelsVisible: false
        lineVisible: false
        tickCount: verticalScaleModel ? verticalScaleModel.vGridCells + 1 : 3
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

            series.objectName = "plot_series_" + i;
            series.color = (cppChannelColors.length > i) ? cppChannelColors[i] : "white";
            series.pointsVisible = true;

            mainChartController.registerSeries(i, series);
        }
    }
}
