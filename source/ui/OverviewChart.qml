import QtQuick
import QtCharts
import QtQuick.Layouts

ChartView {
    id: root
    objectName: "overviewChart"

    Layout.fillWidth: true
    Layout.maximumHeight: 40
    Layout.preferredHeight: 40

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
        return root.series(index);
    }

    Component.onCompleted: {
        for (var i = 0; i < 12; ++i) {
            var series = root.createSeries(ChartView.SeriesTypeLine, "", plotAxisX, plotAxisY);

            series.objectName = "overview_series_" + i;
            series.color = (AppController.channelColors.length > i) ? AppController.channelColors[i] : "white";
            series.pointsVisible = false;

            AppController.overviewChart.registerSeries(i, series);
        }
    }

    Rectangle {
        id: slidingRect

        y: root.plotArea.y
        height: root.plotArea.height

        color: Qt.rgba(0, 120 / 255, 255 / 255, 80 / 255)
        border.color: Qt.rgba(0, 120 / 255, 255 / 255, 1)
        border.width: 2

        onXChanged: {
            if (dragHandler.active) {
                let relativeX = slidingRect.x - root.plotArea.x;
                AppController.overviewChart.updateDragPosition(relativeX);
            }
        }

        HoverHandler {
            cursorShape: Qt.SizeHorCursor
        }

        DragHandler {
            id: dragHandler
            target: slidingRect
            xAxis.minimum: root.plotArea.x
            xAxis.maximum: root.plotArea.x + root.plotArea.width - slidingRect.width
            yAxis.enabled: false
            cursorShape: Qt.SizeHorCursor
        }

        Connections {
            target: AppController.overviewChart

            function onGeometryChanged() {
                if (!dragHandler.active) {
                    slidingRect.x = root.plotArea.x + AppController.overviewChart.xPos;
                    slidingRect.width = AppController.overviewChart.rectWidth;
                }
            }
        }
    }
}
