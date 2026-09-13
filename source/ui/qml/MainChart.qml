pragma ComponentBehavior: Bound

import QtQuick
import QtGraphs
import QtQuick.Controls

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

            required property int channel_id

            pointDelegate: Item {
                id: pointItem
                width: 6
                height: 6

                property int pointIndex

                Rectangle {
                    anchors.centerIn: parent
                    width: 6
                    height: 6
                    radius: 3
                    color: seriesItem.color
                }

                HoverHandler {
                    id: hover
                    onHoveredChanged: {
                        if (hovered && AppController.stopped) {
                            const meta_data = AppController.mainChart.getMeta(seriesItem.channel_id, pointItem.pointIndex);

                            if (!meta_data || meta_data.count === undefined) {
                                pointToolTip.text = "";
                                return;
                            }

                            if (meta_data.count == 1) {
                                pointToolTip.text = qsTr("Channel %1\nValue: %2").arg(seriesItem.channel_id + 1).arg(meta_data.value);
                            } else {
                                pointToolTip.text = qsTr("Channel %1\nMultiple values collapsed (%2)\n Min value: %3\n Max value: %4").arg(seriesItem.channel_id + 1).arg(meta_data.count).arg(meta_data.min_value).arg(meta_data.max_value);
                            }
                        }
                    }
                }

                ToolTip {
                    id: pointToolTip
                    visible: hover.hovered && AppController.stopped
                    delay: 0
                }
            }
        }
    }

    Component.onCompleted: {
        for (var i = 0; i < 12; ++i) {
            var series = lineSeriesComponent.createObject(root, {
                objectName: "plot_series_" + i,
                color: (AppController.channelColors.length > i) ? AppController.channelColors[i] : "white",
                channel_id: i
            });

            root.addSeries(series);

            AppController.mainChart.registerSeries(i, series);
        }
    }
}
