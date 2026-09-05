pragma ComponentBehavior: Bound

import QtQuick
import QtGraphs

GraphsView {
    id: root

    marginTop: 5
    marginBottom: 5
    marginLeft: 5
    marginRight: 5

    theme: GraphsTheme {
        colorScheme: GraphsTheme.ColorScheme.Dark
        backgroundVisible: false
        plotAreaBackgroundVisible: false
        gridVisible: true
        grid.mainColor: Qt.rgba(0, 1, 0, 100 / 255)
        labelsVisible: false
    }

    axisX: ValueAxis {
        id: plotAxisX
        objectName: "plotAxisX"

        visible: false
        labelsVisible: false
        lineVisible: false

        tickInterval: (plotAxisX.max - plotAxisX.min) / Math.max(1, AppController.timebaseModel.hGridCells)

        Component.onCompleted: AppController.mainChart.registerXAxis(plotAxisX)
    }

    axisY: ValueAxis {
        id: plotAxisY
        objectName: "plotAxisY"

        visible: false
        labelsVisible: false
        lineVisible: false

        min: -plotAxisY.max
        max: AppController.verticalScaleModel.vGridCells / 2

        tickInterval: (plotAxisY.max - plotAxisY.min) / Math.max(1, AppController.verticalScaleModel.vGridCells)
    }

    Component {
        id: lineSeriesComponent
        LineSeries {
            id: seriesItem

            pointDelegate: Rectangle {
                width: 6
                height: 6
                radius: 3

                color: seriesItem.color
            }
        }
    }

    Component.onCompleted: {
        for (var i = 0; i < 12; ++i) {
            var series = lineSeriesComponent.createObject(root, {
                objectName: "plot_series_" + i,
                color: (AppController.channelColors.length > i) ? AppController.channelColors[i] : "white"
            });

            root.addSeries(series);

            AppController.mainChart.registerSeries(i, series);
        }
    }
}
