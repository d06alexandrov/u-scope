// Copyright (C) 2026 Dmitriy Alexandrov
// SPDX-License-Identifier: GPL-3.0-only

pragma ComponentBehavior: Bound

import QtQuick
import QtGraphs
import QtQuick.Controls
import QtQuick.Shapes

Item {
    id: root

    property real markersWidth: 20

    GraphsView {
        id: graphsView

        anchors {
            top: root.top
            bottom: root.bottom
            left: root.left
            right: root.right
            leftMargin: root.markersWidth
        }

        marginTop: 5
        marginBottom: 5
        marginLeft: 0
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

                graphsView.addSeries(series);

                AppController.mainChart.registerSeries(i, series);
            }
        }
    }

    Repeater {
        model: AppController.channelModel

        delegate: Shape {
            id: shapeDelegate

            layer.enabled: true
            layer.samples: 4

            width: root.markersWidth
            height: 12
            x: 0
            y: yPos - height / 2
            z: channelSelected ? 100 : 0

            required property int channelId
            required property color badgeColor
            required property bool channelConnected
            required property bool channelEnabled
            required property bool channelSelected

            property real channelOffset: AppController.verticalScaleModel.vOffset(channelId)

            readonly property real yPos: {
                if ((plotAxisY.max - plotAxisY.min) > 0) {
                    return graphsView.plotArea.y + graphsView.plotArea.height * (plotAxisY.max - channelOffset) / (plotAxisY.max - plotAxisY.min);
                } else {
                    return 0;
                }
            }

            visible: {
                if (!channelConnected || !channelEnabled) {
                    // Channels are not shown
                    return false;
                }
                if ((channelOffset < plotAxisY.min) || (channelOffset > plotAxisY.max)) {
                    // Offset is out of bounds
                    return false;
                }
                if ((yPos < height / 2) || (yPos > root.height - height / 2)) {
                    // Marker is out of chart borders
                    return false;
                }
                return true;
            }
            ShapePath {
                fillColor: shapeDelegate.badgeColor
                strokeWidth: 1
                strokeColor: shapeDelegate.channelSelected ? "white" : "transparent"
                startX: 0
                startY: 0
                PathLine {
                    x: 0
                    y: shapeDelegate.height
                }
                PathLine {
                    x: shapeDelegate.width
                    y: shapeDelegate.height / 2
                }
                PathLine {
                    x: 0
                    y: 0
                }
            }

            Connections {
                target: AppController.verticalScaleModel
                function onVOffsetChanged() {
                    shapeDelegate.channelOffset = AppController.verticalScaleModel.vOffset(shapeDelegate.channelId);
                }
            }
        }
    }
}
