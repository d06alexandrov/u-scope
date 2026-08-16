#pragma once

#include "commontypes.hpp"
#include "dataprocessor.h"

#include <QList>
#include <QObject>
#include <QValueAxis>
#include <QXYSeries>
#include <vector>

/**
 * @brief The class responsible for controlling the overview chart in the application.
 */
class OverviewChartController : public QObject
{
    Q_OBJECT

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    Q_PROPERTY(qreal xPos READ xPos NOTIFY geometryChanged)
    Q_PROPERTY(qreal rectWidth READ rectWidth NOTIFY geometryChanged)
#endif // DOXYGEN_SHOULD_SKIP_THIS

public:
    /**
     * @brief Constructor of OverviewChartController.
     *
     * @param parent Parent QObject, default is nullptr.
     */
    explicit OverviewChartController(QObject *parent = nullptr);

    /**
     * @brief Attach actual ui to the controller.
     *
     * @param x_axis The x-axis of the chart.
     * @param series_array An array of series to be displayed on the chart.
     */
    void attach_ui(QValueAxis *x_axis, std::span<QXYSeries *> series_array);

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

public slots:

    /**
     * @brief Receive full data history from Data Processor.
     *
     * @param new_data List of data history values.
     * @param min_time Minimum timestamp of the data history.
     * @param max_time Maximum timestamp of the data history.
     */
    void receive_full_history(const QList<GraphData> &new_data, UData::Time min_time,
                              UData::Time max_time);

    /**
     * @brief Set the width of the chart in pixels.
     *
     * @param width Width of the chart in pixels.
     */
    void set_chart_width(qreal width);

    /**
     * @brief Sets the width of the sliding window in microseconds.
     *
     * @param window_width_us The width of the sliding window in microseconds.
     */
    void set_sliding_window_width(int64_t window_width_us);

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

    /**
     * @brief Update the position of the sliding window on the overview chart.
     *
     * @param new_x The new x position of the sliding window in pixels.
     */
    void updateDragPosition(qreal new_x);

signals:

    /**
     * @brief Signal emitted when the geometry of the sliding window changes.
     */
    void geometryChanged();

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

    QValueAxis *m_axis_x = nullptr; /**< X axis of the graph. */
    std::vector<QXYSeries *> m_series; /**< Data series of the graph. */

    bool m_continuous_mode = false; /**< Continuous or stopped mode. */

    int m_axis_div_count{ }; /**< Amount of divisions. */
    int64_t m_sliding_window_us{ }; /**< Size of one sliding window in us. */
    UData::Time m_sliding_window_start{ }; /**< Sliding window left position. */
    UData::Time m_graph_min_time{ }; /**< Min graph time. */
    UData::Time m_graph_max_time{ }; /**< Max graph time. */

    qreal m_graph_width{ }; /**< Graph width in pixels. */
    qreal m_x_pos{ }; /**< Sliding window position in pixels. */
    qreal m_window_pixel_width{ }; /**< Sliding window size in pixels. */

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
