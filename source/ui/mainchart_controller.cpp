#include "mainchart_controller.h"

MainChartController::MainChartController(QObject *parent)
    : QObject{ parent }
{
    m_render_timer.setTimerType(Qt::PreciseTimer);

    connect(&m_render_timer, &QTimer::timeout, this, &MainChartController::render_timer_trigger);
}

void MainChartController::registerXAxis(QValueAxis *x_axis)
{
    if (!x_axis) {
        return;
    }

    m_axis_x = x_axis;

    m_axis_div_count = std::max(1, x_axis->tickCount() - 1);
}

void MainChartController::registerSeries(int id, QXYSeries *series)
{
    if (id < 0) {
        return;
    }

    if (id >= m_series.size()) {
        m_series.resize(id + 1, nullptr);
    }

    m_series.at(id) = series;
}

void MainChartController::set_horizontal_div(int64_t div_us)
{
    if (div_us > 0) {
        m_div_horizontal_us = div_us;
    }
}

void MainChartController::set_time_frame(UData::Time start_time, UData::Time end_time)
{
    if (start_time >= end_time) {
        clear_all_series();
        return;
    }

    if (!m_continuous_mode) {
        emit request_stored_data(start_time, end_time, m_graph_width);
    }
}

void MainChartController::receive_stored_data(const QList<GraphData> &new_data,
                                              UData::Time requested_start_time,
                                              UData::Time requested_end_time)
{
    if (m_axis_x == nullptr) {
        return;
    }

    m_axis_x->setRange(0.0, UData::to_double(requested_end_time - requested_start_time));

    m_graph_min_time = requested_start_time;
    m_graph_max_time = requested_end_time;

    // TODO: clear channels that are not present in the new_data list

    for (auto &channel_data : new_data) {
        ChannelId channel_id = channel_data.get_id();

        if (channel_id < m_series.size()) {
            if (auto series = m_series.at(channel_id)) {
                series->replace(channel_data.get_values());
            }
        }
    }
}

void MainChartController::set_chart_width(qreal width)
{
    m_graph_width = std::max(minimum_graph_width, static_cast<int>(width));
}

void MainChartController::switch_continuous_mode(bool on)
{
    if (m_continuous_mode == on) {
        return;
    }

    if (on) {
        m_render_timer.start(default_frame_period_ms);
    } else {
        m_render_timer.stop();
    }

    m_continuous_mode = on;
}

void MainChartController::force_graph_refresh()
{
    if (m_continuous_mode) {
        return;
    }

    emit request_stored_data(m_graph_min_time, m_graph_max_time, m_graph_width);
}

void MainChartController::render_timer_trigger()
{
    if (m_continuous_mode) {
        int64_t time_width_us = m_div_horizontal_us * m_axis_div_count;
        const UData::Time end_time = UData::get_timestamp();
        const int64_t frame_period_us = std::chrono::duration_cast<std::chrono::microseconds>(
                                                std::chrono::milliseconds(default_frame_period_ms))
                                                .count();

        emit request_recent_stored_data(end_time, UData::duration_from_microseconds(time_width_us),
                                        UData::duration_from_microseconds(frame_period_us),
                                        m_graph_width);
    }
}

void MainChartController::clear_all_series()
{
    for (auto &series : m_series) {
        series->clear();
    }
}
