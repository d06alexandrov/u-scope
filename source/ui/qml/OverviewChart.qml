import QtQuick
import QtGraphs

Rectangle {
    id: root
    height: 40

    color: "transparent"

    Text {
        // Replacement if the Overview Chart is not visible

        anchors.centerIn: parent
        color: "white"
        visible: !AppController.overviewChart.visible
        text: qsTr("Measurements history is empty.")
    }

    GraphsView {
        id: overviewChartView
        objectName: "overviewChart"

        anchors.fill: parent

        marginTop: 0
        marginBottom: 0
        marginLeft: 5
        marginRight: 5

        theme: GraphsTheme {
            colorScheme: GraphsTheme.ColorScheme.Dark
            backgroundVisible: false
            plotAreaBackgroundVisible: false
            gridVisible: false
            labelsVisible: false
        }

        visible: AppController.overviewChart.visible

        axisX: ValueAxis {
            id: plotAxisX
            objectName: "overviewAxisX"

            visible: false
            labelsVisible: false
            lineVisible: false

            Component.onCompleted: AppController.overviewChart.registerXAxis(plotAxisX)
        }

        axisY: ValueAxis {
            id: plotAxisY
            objectName: "overviewAxisY"

            visible: false
            labelsVisible: false
            lineVisible: false

            min: -plotAxisY.max
            max: AppController.verticalScaleModel.vGridCells / 2
        }

        Component {
            id: lineSeriesComponent
            LineSeries {
                id: seriesItem
            }
        }

        Component.onCompleted: {
            for (var i = 0; i < 12; ++i) {
                var series = lineSeriesComponent.createObject(root, {
                    objectName: "overview_series_" + i,
                    color: (AppController.channelColors.length > i) ? AppController.channelColors[i] : "white"
                });

                overviewChartView.addSeries(series);

                AppController.overviewChart.registerSeries(i, series);
            }
        }

        onPlotAreaChanged: {
            AppController.overviewChart.set_chart_width(overviewChartView.plotArea.width);
        }
    }

    Rectangle {
        id: slidingRect

        y: overviewChartView.plotArea.y
        height: overviewChartView.plotArea.height

        color: Qt.rgba(0, 120 / 255, 255 / 255, 80 / 255)
        border.color: Qt.rgba(0, 120 / 255, 255 / 255, 1)
        border.width: 2

        visible: AppController.overviewChart.visible

        onXChanged: {
            if (dragHandler.active) {
                let relativeX = slidingRect.x - overviewChartView.plotArea.x;
                AppController.overviewChart.updateDragPosition(relativeX);
            }
        }

        HoverHandler {
            cursorShape: Qt.SizeHorCursor
        }

        DragHandler {
            id: dragHandler
            target: slidingRect
            xAxis.minimum: overviewChartView.plotArea.x
            xAxis.maximum: overviewChartView.plotArea.x + overviewChartView.plotArea.width - slidingRect.width
            yAxis.enabled: false
            cursorShape: Qt.SizeHorCursor
        }

        Connections {
            target: AppController.overviewChart

            function onGeometryChanged() {
                if (!dragHandler.active) {
                    slidingRect.x = overviewChartView.plotArea.x + AppController.overviewChart.xPos;
                    slidingRect.width = AppController.overviewChart.rectWidth;
                }
            }
        }
    }
}
