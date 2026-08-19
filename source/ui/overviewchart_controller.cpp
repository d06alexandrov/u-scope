#include "overviewchart_controller.h"

OverviewChartController::OverviewChartController(QObject *parent)
    : QObject{ parent }
{
}

void OverviewChartController::registerXAxis(QValueAxis *x_axis)
{
    if (!x_axis) {
        return;
    }

    m_axis_x = x_axis;

    m_axis_div_count = std::max(1, x_axis->tickCount() - 1);
}

void OverviewChartController::registerSeries(int id, QXYSeries *series)
{
    if (id >= m_series.size()) {
        m_series.resize(id + 1, nullptr);
    }

    m_series.at(id) = series;
}

qreal OverviewChartController::xPos() const
{
    return m_x_pos;
}
qreal OverviewChartController::rectWidth() const
{
    return m_window_pixel_width;
}

void OverviewChartController::receive_full_history(const QList<GraphData> &new_data,
                                                   UData::Time min_time, UData::Time max_time)
{
    if (m_axis_x == nullptr) {
        return;
    }

    if (m_continuous_mode) {
        return;
    }

    m_axis_x->setRange(min_time, max_time);

    m_graph_min_time = min_time;
    m_graph_max_time = max_time;

    // TODO: clear channels that are not present in the new_data list and update sliding window
    // accordingly.

    for (auto &channel_data : new_data) {
        ChannelId channel_id = channel_data.get_id();

        if (channel_id < m_series.size()) {
            if (auto series = m_series.at(channel_id)) {
                series->replace(channel_data.get_values());
            }
        }
    }

    // Put sliding window to the end
    if (UData::get_timestamp_diff_us(m_graph_min_time, m_graph_max_time) <= m_sliding_window_us) {
        m_sliding_window_start = m_graph_min_time;
    } else {
        m_sliding_window_start =
                UData::timestamp_sub_us_rounddown(m_graph_max_time, m_sliding_window_us);
    }

    update_sliding_window_on_graph();

    auto [window_min_time, window_max_time] = get_window_boundaries();
    emit selected_time_frame(window_min_time, window_max_time);
}

void OverviewChartController::set_chart_width(qreal width)
{
    if (m_graph_width == width) {
        return;
    }

    m_graph_width = width;

    if (!m_continuous_mode) {
        update_sliding_window_on_graph();
    }
}

void OverviewChartController::set_sliding_window_width(int64_t window_width_us)
{
    if (m_sliding_window_us == window_width_us) {
        return;
    }

    if (m_continuous_mode) {
        m_sliding_window_us = window_width_us;
        return;
    }

    // Keep the center of the window in a fixed position
    m_sliding_window_start = UData::timestamp_add_us_roundup(
            m_sliding_window_start, (m_sliding_window_us - window_width_us) / 2);

    m_sliding_window_us = window_width_us;

    // Fix the position if it hits the border
    if (UData::get_timestamp_diff_us(m_sliding_window_start, m_graph_max_time)
        < m_sliding_window_us) {
        m_sliding_window_start =
                UData::timestamp_sub_us_rounddown(m_graph_max_time, m_sliding_window_us);
    }

    if (m_sliding_window_start < m_graph_min_time) {
        m_sliding_window_start = m_graph_min_time;
    }

    update_sliding_window_on_graph();

    auto [window_min_time, window_max_time] = get_window_boundaries();
    emit selected_time_frame(window_min_time, window_max_time);
}

void OverviewChartController::switch_continuous_mode(bool on)
{
    if (m_continuous_mode == on) {
        return;
    }

    // TODO: hide graph in continuous mode

    if (!on) {
        emit request_full_history(std::max(minimum_graph_points, static_cast<int>(m_graph_width)));
    }

    m_continuous_mode = on;
}

void OverviewChartController::force_graph_refresh()
{
    if (m_continuous_mode) {
        return;
    }

    emit request_full_history(std::max(minimum_graph_points, static_cast<int>(m_graph_width)));
}

void OverviewChartController::updateDragPosition(qreal new_x)
{
    qreal max_x = std::max(0.0, m_graph_width - m_window_pixel_width);
    new_x = std::clamp(new_x, 0.0, max_x);

    if (m_x_pos != new_x) {
        m_x_pos = new_x;

        UData::Time new_sliding_window_start = m_graph_min_time;

        if (m_graph_width > std::numeric_limits<qreal>::epsilon()) {
            auto new_window_offset = static_cast<int64_t>(
                    UData::get_timestamp_diff_us(m_graph_min_time, m_graph_max_time) * m_x_pos
                    / m_graph_width);

            new_sliding_window_start =
                    UData::timestamp_add_us_roundup(m_graph_min_time, new_window_offset);
        }

        if (new_sliding_window_start != m_sliding_window_start) {
            m_sliding_window_start = new_sliding_window_start;

            auto [window_min_time, window_max_time] = get_window_boundaries();
            emit selected_time_frame(window_min_time, window_max_time);
        }
    }
}

void OverviewChartController::clear_all_series()
{
    for (auto &series : m_series) {
        series->clear();
    }
}

void OverviewChartController::update_sliding_window_on_graph()
{
    qreal x_pos = 0.0;
    qreal window_width = 0.0;

    if (UData::get_timestamp_diff_us(m_graph_min_time, m_graph_max_time) <= 0) {
        x_pos = 0.0;
        window_width = m_graph_width;
    } else {
        x_pos = m_graph_width
                * static_cast<qreal>(
                        UData::get_timestamp_diff_us(m_graph_min_time, m_sliding_window_start))
                / static_cast<qreal>(
                        UData::get_timestamp_diff_us(m_graph_min_time, m_graph_max_time));
        window_width = std::clamp(m_graph_width * static_cast<qreal>(m_sliding_window_us)
                                          / static_cast<qreal>(UData::get_timestamp_diff_us(
                                                  m_graph_min_time, m_graph_max_time)),
                                  0.0, m_graph_width - x_pos);
    }

    if ((x_pos != m_x_pos) || (window_width != m_window_pixel_width)) {
        m_x_pos = x_pos;
        m_window_pixel_width = window_width;

        emit geometryChanged();
    }
}

std::tuple<UData::Time, UData::Time> OverviewChartController::get_window_boundaries()
{
    // Return the time frame aligned to the right, if the whole history is shorter than sliding
    // window.
    if (UData::get_timestamp_diff_us(m_graph_min_time, m_graph_max_time) <= m_sliding_window_us) {
        return std::tuple{ UData::timestamp_sub_us_rounddown(m_graph_max_time, m_sliding_window_us),
                           m_graph_max_time };
    }

    UData::Time left_boundary = m_sliding_window_start;
    UData::Time right_boundary =
            std::clamp(UData::timestamp_add_us_roundup(m_sliding_window_start, m_sliding_window_us),
                       m_graph_min_time, m_graph_max_time);

    return std::tuple{ left_boundary, right_boundary };
}
