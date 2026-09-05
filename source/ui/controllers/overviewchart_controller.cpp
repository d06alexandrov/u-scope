#include "overviewchart_controller.hpp"

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
}

void OverviewChartController::registerSeries(int id, QXYSeries *series)
{
    if (id < 0) {
        return;
    }

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

QBindable<bool> OverviewChartController::bindableVisible()
{
    return &m_visible;
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

void OverviewChartController::updateDragPosition(qreal new_x)
{
    qreal max_x = std::max(0.0, m_graph_width - m_window_pixel_width);
    new_x = std::clamp(new_x, 0.0, max_x);

    if (m_x_pos != new_x) {
        m_x_pos = new_x;

        UData::Time new_sliding_window_start = m_graph_min_time;

        if (m_graph_width > std::numeric_limits<qreal>::epsilon()) {
            const double fraction = m_x_pos / m_graph_width;
            const double offset_seconds =
                    UData::to_double(m_graph_max_time - m_graph_min_time) * fraction;
            const UData::Time::Duration offset = UData::duration_from_seconds(offset_seconds);

            new_sliding_window_start = m_graph_min_time + offset;
        }

        if (new_sliding_window_start != m_sliding_window_start) {
            m_sliding_window_start = new_sliding_window_start;

            auto [window_min_time, window_max_time] = get_window_boundaries();
            emit selected_time_frame(window_min_time, window_max_time);
        }
    }
}

void OverviewChartController::moveSlidingWindow(int division_steps)
{
    if (m_continuous_mode || division_steps == 0) {
        return;
    }

    UData::Time new_start = m_sliding_window_start + m_div_horizontal * division_steps;

    const UData::Time max_start =
            std::max(m_graph_min_time, m_graph_max_time - m_sliding_window_width);
    new_start = std::clamp(new_start, m_graph_min_time, max_start);

    if (new_start != m_sliding_window_start) {
        m_sliding_window_start = new_start;
        update_sliding_window_on_graph();

        auto [window_min_time, window_max_time] = get_window_boundaries();
        emit selected_time_frame(window_min_time, window_max_time);
    }
}

void OverviewChartController::set_sliding_window_width(UData::Time::Duration window_width)
{
    if (m_sliding_window_width == window_width) {
        return;
    }

    if (m_continuous_mode) {
        m_sliding_window_width = window_width;
        return;
    }

    // Keep the center of the window in a fixed position
    m_sliding_window_start = m_sliding_window_start + (m_sliding_window_width - window_width) / 2;

    m_sliding_window_width = window_width;

    // Fix the position if it hits the border
    if ((m_graph_max_time - m_sliding_window_start) < m_sliding_window_width) {
        m_sliding_window_start = m_graph_max_time - m_sliding_window_width;
    }

    if (m_sliding_window_start < m_graph_min_time) {
        m_sliding_window_start = m_graph_min_time;
    }

    update_sliding_window_on_graph();

    auto [window_min_time, window_max_time] = get_window_boundaries();
    emit selected_time_frame(window_min_time, window_max_time);
}
void OverviewChartController::set_horizontal_div(UData::Time::Duration div)
{
    if (div > UData::Time::Duration::zero()) {
        m_div_horizontal = div;
    }
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

    if ((min_time >= max_time) || (new_data.empty())) {
        // History is empty, chart must be hidden
        m_visible = false;
        return;
    }

    m_axis_x->setRange(0.0, UData::to_double(max_time - min_time));

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

    if (m_visible) {
        // Overview graph was refreshed
        const UData::Time max_start = std::max(min_time, max_time - m_sliding_window_width);
        m_sliding_window_start = std::clamp(m_sliding_window_start, min_time, max_start);
    } else {
        // Put sliding window to the end
        if ((max_time - min_time) <= m_sliding_window_width) {
            m_sliding_window_start = min_time;
        } else {
            m_sliding_window_start = max_time - m_sliding_window_width;
        }
    }

    m_graph_min_time = min_time;
    m_graph_max_time = max_time;

    update_sliding_window_on_graph();

    auto [window_min_time, window_max_time] = get_window_boundaries();
    emit selected_time_frame(window_min_time, window_max_time);

    m_visible = true;
}

void OverviewChartController::switch_continuous_mode(bool on)
{
    if (m_continuous_mode == on) {
        return;
    }

    // TODO: hide graph in continuous mode

    if (on) {
        m_visible = false;
        clear_all_series();
    } else {
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

    if (m_graph_min_time >= m_graph_max_time) {
        x_pos = 0.0;
        window_width = m_graph_width;
    } else {
        double start_fraction = UData::to_double(m_sliding_window_start - m_graph_min_time)
                / UData::to_double(m_graph_max_time - m_graph_min_time);
        double window_fraction = UData::to_double(m_sliding_window_width)
                / UData::to_double(m_graph_max_time - m_graph_min_time);

        x_pos = m_graph_width * start_fraction;
        window_width = std::clamp(m_graph_width * window_fraction, 0.0, m_graph_width - x_pos);
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
    if ((m_graph_max_time - m_graph_min_time) <= m_sliding_window_width) {
        return std::tuple{ m_graph_max_time - m_sliding_window_width, m_graph_max_time };
    }

    UData::Time left_boundary = m_sliding_window_start;
    UData::Time right_boundary = std::clamp(m_sliding_window_start + m_sliding_window_width,
                                            m_graph_min_time, m_graph_max_time);

    return std::tuple{ left_boundary, right_boundary };
}
