import QtQuick
import QtCharts // qmllint disable import

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

    ChartView {
        id: overviewChartView
        objectName: "overviewChart"

        anchors.fill: parent
        anchors.margins: -3

        backgroundColor: "transparent"
        legend.visible: false
        antialiasing: true
        margins {
            top: 0
            bottom: 0
            left: 0
            right: 0
        }

        visible: AppController.overviewChart.visible

        ValueAxis {
            id: plotAxisX
            objectName: "overviewAxisX"
            gridVisible: false
            labelsVisible: false
            lineVisible: false

            Component.onCompleted: AppController.overviewChart.registerXAxis(plotAxisX)
        }

        ValueAxis {
            id: plotAxisY
            objectName: "overviewAxisY"
            min: -plotAxisY.max
            max: AppController.verticalScaleModel.vGridCells / 2
            gridVisible: false
            labelsVisible: false
            lineVisible: false
        }

        function getAxisX(): ValueAxis {
            return plotAxisX;
        }

        function getSeries(index: int): AbstractSeries {
            return overviewChartView.series(index);
        }

        Component.onCompleted: {
            for (var i = 0; i < 12; ++i) {
                var series = overviewChartView.createSeries(ChartView.SeriesTypeLine, "", plotAxisX, plotAxisY);

                series.objectName = "overview_series_" + i;
                series.color = (AppController.channelColors.length > i) ? AppController.channelColors[i] : "white";
                series.pointsVisible = false;

                AppController.overviewChart.registerSeries(i, series);
            }
        }

        onWidthChanged: {
            AppController.overviewChart.set_chart_width(overviewChartView.plotArea.width);
        }

        Rectangle {
            id: slidingRect

            y: overviewChartView.plotArea.y
            height: overviewChartView.plotArea.height

            color: Qt.rgba(0, 120 / 255, 255 / 255, 80 / 255)
            border.color: Qt.rgba(0, 120 / 255, 255 / 255, 1)
            border.width: 2

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
}
