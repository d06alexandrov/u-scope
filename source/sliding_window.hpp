#pragma once

#include <QBrush>
#include <QChartView>
#include <QCursor>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QObject>
#include <QPen>
#include <limits>
#include <optional>

/**
 * @brief Sliding window for the overview graph.
 */
class SlidingWindow : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
public:
    /**
     * @brief Constructs a SlidingWindow object.
     *
     * @param parent The parent overview graph.
     */
    explicit SlidingWindow(QGraphicsItem *parent = nullptr)
        : QObject()
        , QGraphicsRectItem(parent)
    {
        // Enable dragging and tracking position changes
        setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemSendsGeometryChanges);

        setBrush(QBrush(QColor(0, 120, 255, 80)));
        setPen(QPen(QColor(0, 120, 255, 255), 2));
        setCursor(Qt::SizeAllCursor);
    }

private:
    UData::Time m_overview_min_time{ }; /**< Min time of the overview graph. */
    UData::Time m_overview_max_time{ }; /**< Max time of the overview graph. */
    UData::Time m_window_start{ }; /**< Left border of the window. */
    int64_t m_window_width_us{ }; /**< Width of the window in us. */

    /**
     * @brief Calculates the width of the overview graph.
     *
     * @return The width of the overview graph.
     */
    qreal get_overview_graph_width()
    {
        QGraphicsScene *current_scene = this->scene();
        if (!current_scene) {
            return 0.0;
        }

        auto views = current_scene->views();
        if (views.isEmpty()) {
            return 0.0;
        }

        QGraphicsView *view = views.first();
        if (auto chart_view = qobject_cast<QChartView *>(view)) {
            qreal exact_plot_width = chart_view->chart()->plotArea().width();
            return exact_plot_width;
        }

        return 0.0;
    }

    /**
     * @brief Calculates the time boundaries of the sliding window.
     *
     * @return A tuple containing the minimum and maximum time of the sliding window.
     */
    std::tuple<UData::Time, UData::Time> get_window_boundaries()
    {
        UData::Time left_boundary = m_window_start;
        UData::Time right_boundary =
                std::clamp(UData::timestamp_add_us_roundup(m_window_start, m_window_width_us),
                           m_overview_min_time, m_overview_max_time);

        return std::tuple{ left_boundary, right_boundary };
    }

    /**
     * @brief Update sliding window rectangle on the overview graph.
     *
     * @param chart_width Width of the overview graph.
     * @param chart_height Optional height of the overview graph. If not provided, the current
     * height.
     */
    void update_rectangle_position(qreal chart_width,
                                   std::optional<qreal> chart_height = std::nullopt)
    {
        qreal x_pos = 0.0;
        qreal window_width = 0.0;
        qreal window_height = chart_height.value_or(rect().height());

        if (UData::get_timestamp_diff_us(m_overview_min_time, m_overview_max_time) <= 0) {
            x_pos = 0.0;
            window_width = chart_width;
        } else {
            x_pos = chart_width
                    * static_cast<qreal>(
                            UData::get_timestamp_diff_us(m_overview_min_time, m_window_start))
                    / static_cast<qreal>(
                            UData::get_timestamp_diff_us(m_overview_min_time, m_overview_max_time));
            window_width = chart_width * static_cast<qreal>(m_window_width_us)
                    / static_cast<qreal>(UData::get_timestamp_diff_us(m_overview_min_time,
                                                                      m_overview_max_time));
        }

        blockSignals(true);

        setRect(0, 0, window_width, window_height);
        setPos(x_pos, 0);

        blockSignals(false);
    }

signals:
    /**
     * @brief Signal emitted when the position of the sliding window changes.
     *
     * @param window_min_time The minimum time of the sliding window.
     * @param window_max_time The maximum time of the sliding window.
     */
    void position_changed(UData::Time window_min_time, UData::Time window_max_time);

public slots:
    /**
     * @brief Reset the sliding window to the right border of the overview graph.
     *
     * @param min_time The minimum time of the overview graph.
     * @param max_time The maximum time of the overview graph.
     * @param window_width_us The width of the sliding window in microseconds.
     */
    void reset_window_to_right(UData::Time min_time, UData::Time max_time, int64_t window_width_us)
    {
        m_overview_min_time = min_time;
        m_overview_max_time = max_time;
        m_window_width_us = window_width_us;

        if (UData::get_timestamp_diff_us(m_overview_min_time, m_overview_max_time)
            <= m_window_width_us) {
            m_window_start = m_overview_min_time;
        } else {
            m_window_start =
                    UData::timestamp_sub_us_rounddown(m_overview_max_time, m_window_width_us);
        }

        update_rectangle_position(get_overview_graph_width());

        emit position_changed(m_window_start, m_overview_max_time);
    }

    /**
     * @brief Sets the width of the sliding window in microseconds.
     *
     * @param window_width_us The width of the sliding window in microseconds.
     */
    void set_window_width(int64_t window_width_us)
    {
        if (m_window_width_us == window_width_us) {
            return;
        } else if (m_window_width_us > window_width_us) {
            // Keep the center of the window in a fixed position
            m_window_start = UData::timestamp_add_us_roundup(
                    m_window_start, (m_window_width_us - window_width_us) / 2);
            m_window_width_us = window_width_us;
        } else {
            // Keep the center of the window in a fixed position if possible
            if (window_width_us
                >= UData::get_timestamp_diff_us(m_overview_min_time, m_overview_max_time)) {
                m_window_start = m_overview_min_time;
                m_window_width_us =
                        UData::get_timestamp_diff_us(m_overview_min_time, m_overview_max_time);
            } else {
                if (UData::get_timestamp_diff_us(m_overview_min_time, m_window_start)
                    <= ((window_width_us - m_window_width_us) / 2)) {
                    // Left border does not fit if the center is fixed
                    m_window_start = m_overview_min_time;
                } else if (UData::get_timestamp_diff_us(m_window_start, m_overview_max_time)
                           <= ((window_width_us - m_window_width_us) / 2)) {
                    // Right border does not fit if the center is fixed
                    m_window_start =
                            UData::timestamp_sub_us_rounddown(m_overview_max_time, window_width_us);
                } else {
                    // It is safe to move the window with a fixed center
                    m_window_start = UData::timestamp_sub_us_rounddown(
                            m_window_start, (window_width_us - m_window_width_us) / 2);
                }

                m_window_width_us = window_width_us;
            }
        }

        // Update rectangle position on the overview graph
        update_rectangle_position(get_overview_graph_width());

        // Update the main graph
        auto [window_min_time, window_max_time] = get_window_boundaries();
        if (window_min_time != window_max_time) {
            emit position_changed(window_min_time, window_max_time);
        }
    }

    /**
     * @brief Slot to handle dimension changes of the overview graph's plot area.
     *
     * @param plotArea The new dimensions of the plot area.
     */
    void plot_area_changed(const QRectF &plotArea)
    {
        update_rectangle_position(plotArea.width(), plotArea.height());
    }

protected:
    /**
     * @brief Handles item changes to constrain movement.
     *
     * @param change The type of change.
     * @param value The new value associated with the change.
     * @return The adjusted sliding window position after applying constraints.
     */
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override
    {
        if (change == ItemPositionChange && scene()) {
            qreal overview_graph_width = get_overview_graph_width();
            QPointF newPos = value.toPointF();

            newPos.setY(0);

            if (overview_graph_width < std::numeric_limits<qreal>::epsilon()) {
                newPos.setX(0);
                m_window_start = m_overview_min_time;
            } else {
                qreal max_x = std::max(0.0, overview_graph_width - rect().width());

                if (newPos.x() < 0) {
                    newPos.setX(0);
                } else if (newPos.x() > max_x) {
                    newPos.setX(max_x);
                }

                int64_t new_window_offset = static_cast<int64_t>(
                        UData::get_timestamp_diff_us(m_overview_min_time, m_overview_max_time)
                        * newPos.x() / overview_graph_width);

                m_window_start =
                        UData::timestamp_add_us_roundup(m_overview_min_time, new_window_offset);
            }

            return newPos;
        } else if (change == ItemPositionHasChanged) {
            auto [window_min_time, window_max_time] = get_window_boundaries();
            if (window_min_time != window_max_time) {
                emit position_changed(window_min_time, window_max_time);
            }
        }

        return QGraphicsRectItem::itemChange(change, value);
    }
};
