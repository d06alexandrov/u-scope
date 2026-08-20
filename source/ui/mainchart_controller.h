#pragma once

#include "dataprocessor.h"

#include <QList>
#include <QObject>
#include <QPointer>
#include <QValueAxis>
#include <QXYSeries>

/**
 * @brief The class responsible for controlling the main chart in the application.
 */
class MainChartController : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructor of MainChartController.
     *
     * @param parent Parent QObject, default is nullptr.
     */
    explicit MainChartController(QObject *parent = nullptr);

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

public slots:

    /**
     * @brief Set the horizontal division size in microseconds.
     *
     * @param div_us Size of one horizontal division in microseconds.
     */
    void set_horizontal_div(int64_t div_us);

    /**
     * @brief Set the time frame of the chart.
     *
     * @param start_time Start time of the time frame.
     * @param end_time End time of the time frame.
     */
    void set_time_frame(UData::Time start_time, UData::Time end_time);

    /**
     * @brief Receive requested data from Data Processor and display it on the graph.
     *
     * @param new_data List of new data to be displayed on the graph.
     * @param requested_start_time Start time of the requested data.
     * @param requested_end_time End time of the requested data.
     */
    void receive_stored_data(const QList<GraphData> &new_data, UData::Time requested_start_time,
                             UData::Time requested_end_time);

    /**
     * @brief Set the width of the chart in pixels.
     *
     * @param width Width of the chart in pixels.
     */
    void set_chart_width(qreal width);

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
     * @brief Request most recent stored data from Data Processor to display.
     *
     * Request data, where the latest data is not less than end_time - max_drift_us. The returned
     * range is [newest data - data_windows_us, newest data].
     *
     * @param end_time Maximum time of the requested data.
     * @param data_window_us Time window of the requested data in microseconds.
     * @param max_drift_us Maximum difference between requested and returned end time.
     * @param points_limit Maximum amount of points to be displayed on the graph.
     */
    void request_recent_stored_data(UData::Time end_time, int64_t data_window_us,
                                    int64_t max_drift_us, int points_limit);

    /**
     * @brief Request stored data from Data Processor to display.
     *
     * @param start_time Start time of the requested data.
     * @param end_time End time of the requested data.
     * @param points_limit Maximum amount of points to be displayed on the graph.
     */
    void request_stored_data(UData::Time start_time, UData::Time end_time, int points_limit);

protected:
private slots:

    /**
     * @brief Render logic invoked by the render timer.
     */
    void render_timer_trigger();

private:
    static constexpr int minimum_graph_width =
            100; /**< Minimum expected pixel width of the graph. */
    static constexpr int default_frame_period_ms =
            33; /**< Default graph frame update period in ms. */

    QPointer<QValueAxis> m_axis_x{ }; /**< X axis of the graph. */
    std::vector<QPointer<QXYSeries>> m_series{ }; /**< Data series of the graph. */

    bool m_continuous_mode = false; /**< Continuous or stopped mode. */
    QTimer m_render_timer; /**< Timer to trigger an update of the graph data. */

    int m_axis_div_count{ }; /**< Amount of divisions. */
    int64_t m_div_horizontal_us{ }; /**< Size of one horizontal division in us. */

    UData::Time m_graph_min_time{ }; /**< Min graph time. */
    UData::Time m_graph_max_time{ }; /**< Max graph time. */

    int m_graph_width = minimum_graph_width; /**< Graph width in pixels. */

    /**
     * @brief Reset all series in the graph.
     */
    void clear_all_series();
};
