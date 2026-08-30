#pragma once

#include "commontypes.hpp"
#include "dataprocessor.hpp"

#include <QBindable>
#include <QList>
#include <QObject>
#include <QPointer>
#include <QValueAxis>
#include <QXYSeries>
#include <QtQmlIntegration/qqmlintegration.h>
#include <vector>

/**
 * @brief The class responsible for controlling the overview chart in the application.
 */
class OverviewChartController : public QObject
{
    Q_OBJECT
    QML_ANONYMOUS

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    Q_PROPERTY(qreal xPos READ xPos NOTIFY geometryChanged)
    Q_PROPERTY(qreal rectWidth READ rectWidth NOTIFY geometryChanged)
    Q_PROPERTY(bool visible READ default NOTIFY visibleChanged BINDABLE bindableVisible)
#endif // DOXYGEN_SHOULD_SKIP_THIS

public:
    /**
     * @brief Constructor of OverviewChartController.
     *
     * @param parent Parent QObject, default is nullptr.
     */
    explicit OverviewChartController(QObject *parent = nullptr);

    /**
     * @brief Register the x-axis of the chart.
     *
     * @param x_axis Pointer to the QValueAxis representing the x-axis of the chart.
     */
    Q_INVOKABLE void registerXAxis(QValueAxis *x_axis);

    /**
     * @brief Register a data series for the chart.
     *
     * @param id The identifier for the series.
     * @param series Pointer to the QXYSeries to be registered.
     */
    Q_INVOKABLE void registerSeries(int id, QXYSeries *series);

    /**
     * @brief Get the current x position of the sliding window.
     *
     * @return The x position of the sliding window in pixels.
     */
    [[nodiscard]] qreal xPos() const;

    /**
     * @brief Get the current width of the sliding window in pixels.
     *
     * @return The width of the sliding window in pixels.
     */
    [[nodiscard]] qreal rectWidth() const;

    /**
     * @brief Get the bindable property for visibility of the chart.
     *
     * @return A QBindable<bool> representing the visibility of the chart.
     */
    [[nodiscard]] QBindable<bool> bindableVisible();

    /**
     * @brief Set the width of the chart in pixels.
     *
     * @param width Width of the chart in pixels.
     */
    Q_INVOKABLE void set_chart_width(qreal width);

    /**
     * @brief Update the position of the sliding window on the overview chart.
     *
     * @param new_x The new x position of the sliding window in pixels.
     */
    Q_INVOKABLE void updateDragPosition(qreal new_x);

    /**
     * @brief Move the sliding window by a specified number of horizontal division steps.
     *
     * @param division_steps The number of horizontal division steps to move the sliding window.
     */
    Q_INVOKABLE void moveSlidingWindow(int division_steps);

    /**
     * @brief Sets the width of the sliding window in microseconds.
     *
     * @param window_width The width of the sliding window.
     */
    void set_sliding_window_width(UData::Time::Duration window_width);

    /**
     * @brief Set the horizontal division size.
     *
     * @param div Size of one horizontal division.
     */
    void set_horizontal_div(UData::Time::Duration div);

public slots:

    /**
     * @brief Receive full data history from Data Processor.
     *
     * Timestamps in the list of data history values are relative to the minimum timestamp of the
     * data history.
     *
     * @param new_data List of data history values.
     * @param min_time Minimum timestamp of the data history.
     * @param max_time Maximum timestamp of the data history.
     */
    void receive_full_history(const QList<GraphData> &new_data, UData::Time min_time,
                              UData::Time max_time);

    /**
     * @brief Switch between continuous and stopped mode of the chart.
     *
     * @param on True to switch to continuous mode, false to switch to stopped mode.
     */
    void switch_continuous_mode(bool on);

    /**
     * @brief Force refresh of the chart data.
     *
     * Refresh data of the current view in a stopped mode.
     */
    void force_graph_refresh();

signals:

    /**
     * @brief Signal emitted when the geometry of the sliding window changes.
     */
    void geometryChanged();

    /**
     * @brief Signal emitted when the visibility of the chart changes.
     */
    void visibleChanged();

    /**
     * @brief Request full history data from Data Processor to display.
     *
     * @param points_limit Maximum amount of points to be returned.
     */
    void request_full_history(int points_limit);

    /**
     * @brief Signal emitted when a new time frame is selected on the overview chart.
     *
     * @param start_time The start time of the selected time frame.
     * @param end_time The end time of the selected time frame.
     */
    void selected_time_frame(UData::Time start_time, UData::Time end_time);

protected:
private slots:

private:
    static constexpr int minimum_graph_points = 100; /**< Minimum graph points to be drawn. */

    QPointer<QValueAxis> m_axis_x{ }; /**< X axis of the graph. */
    std::vector<QPointer<QXYSeries>> m_series{ }; /**< Data series of the graph. */

    bool m_continuous_mode = false; /**< Continuous or stopped mode. */

    UData::Time::Duration m_div_horizontal{ }; /**< Size of one horizontal division. */
    UData::Time::Duration m_sliding_window_width{ }; /**< Size of one sliding window. */
    UData::Time m_sliding_window_start{ }; /**< Sliding window left position. */
    UData::Time m_graph_min_time{ }; /**< Min graph time. */
    UData::Time m_graph_max_time{ }; /**< Max graph time. */

    qreal m_graph_width{ }; /**< Graph width in pixels. */
    qreal m_x_pos{ }; /**< Sliding window position in pixels. */
    qreal m_window_pixel_width{ }; /**< Sliding window size in pixels. */

    Q_OBJECT_BINDABLE_PROPERTY(
            OverviewChartController, bool, m_visible,
            &OverviewChartController::visibleChanged); /**< Bindable property for visibility of the
                                                          chart. */

    /**
     * @brief Reset all series in the graph.
     */
    void clear_all_series();

    /**
     * @brief Update the sliding window position and size on the overview chart.
     */
    void update_sliding_window_on_graph();

    /**
     * @brief Get the time boundaries of the sliding window.
     *
     * @return A tuple containing the minimum and maximum time of the sliding window.
     */
    std::tuple<UData::Time, UData::Time> get_window_boundaries();
};
