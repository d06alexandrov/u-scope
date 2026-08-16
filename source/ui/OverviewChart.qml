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
    }

    ValueAxis {
        id: plotAxisY
        objectName: "overviewAxisY"
        min: -plotAxisY.max
        max: verticalScaleModel ? verticalScaleModel.vGridCells / 2 : 1
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

    Rectangle {
        id: slidingRect

        y: root.plotArea.y
        height: root.plotArea.height

        color: Qt.rgba(0, 120 / 255, 255 / 255, 80 / 255)
        border.color: Qt.rgba(0, 120 / 255, 255 / 255, 1)
        border.width: 2

        onXChanged: {
            if (dragHandler.active && overviewChartController) {
                let relativeX = slidingRect.x - root.plotArea.x;
                overviewChartController.updateDragPosition(relativeX);
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
            target: overviewChartController

            function onGeometryChanged() {
                if (!dragHandler.active && overviewChartController) {
                    slidingRect.x = root.plotArea.x + overviewChartController.xPos;
                    slidingRect.width = overviewChartController.rectWidth;
                }
            }
        }
    }
}
