#pragma once

#include "channelbar_model.h"
#include "dataprocessor.h"
#include "mainchart_controller.h"
#include "universalreader.h"
#include "universalreader_dialog.h"

#include <QChart>
#include <QLineSeries>
#include <QMainWindow>
#include <QStandardItemModel>
#include <QValueAxis>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class SlidingWindow;
class QQuickItem;

/**
 * @brief Class that implements main window of the application
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

private:
    static constexpr ReaderId readers_amount = 10; /**< Maximum amount of readers. */
    static constexpr size_t channels_amount = 12; /**< Amount of channels. */
    static constexpr int64_t default_div_vertical_uval =
            1000000; /**< Default Size of one vertical division in 10^-6. */
    static constexpr int maximum_overview_points =
            2000; /**< Maximum amount of points of the overview chart. */

    /**
     * @brief Roles of the items in the source list.
     */
    enum ItemRoles {
        ReaderIdRole = Qt::UserRole + 1, /**< Role for the reader id. */
        VariableIdRole = Qt::UserRole + 2, /**< Role for the variable id. */
    };

    /**
     * @brief Current state of the scope.
     */
    enum ScopeMode {
        Stopped, /**< Data gathering is stopped. */
        Roll, /**< Receive and display data in a continuous mode. */
        Normal, /**< Receive and display data in an oscilloscope normal mode. */
        OneShot, /**< One-shot trigger mode. */
    };

    /**
     * @brief Configuration of the Graph
     */
    struct GraphStyle
    {
        static constexpr QPoint left_bottom_corner = {
            -100, -100
        }; /**< Value in the bottom left corner of the graph. */
        static constexpr QPoint right_top_corner = {
            100, 100
        }; /**< Value in the top right corner of the graph. */
        static constexpr int horizontal_grid = 10; /**< Amount of horizontal cells in a grid. */
        static constexpr int vertical_grid = 8; /**< Amount of vertical cells in a grid. */
        static constexpr QColor grid_line_color = QColor(0, 255, 0, 100); /**< Color of a grid. */
        static constexpr QColor background_color = QColorConstants::Black; /**< Background color. */
    };

    const std::vector<QColor> channel_colors = {
        QColor("#FFFF00"), QColor("#00FFFF"), QColor("#FF00FF"), QColor("#00FF00"),
        QColor("#FF8000"), QColor("#0080FF"), QColor("#FF0080"), QColor("#80FF00"),
        QColor("#A060FF"), QColor("#FFD700"), QColor("#00F5FF"), QColor("#FF4500"),
    };

    Ui::MainWindow *ui = nullptr; /**< Pointer to the Main Window user interface. */

    QLineSeries
            *m_series_overview[channels_amount]; /**< Pointer to Chart's series' for overview. */
    QValueAxis *m_overview_axis_x = nullptr; /**< Overview's x axis. */
    QValueAxis *m_overview_axis_y = nullptr; /**< Overview's y axis. */
    SlidingWindow *m_sliding_window = nullptr; /**< Sliding window on the overview graph. */

    DataProcessor *m_data_processor = nullptr; /**< Main Data Processor. */
    QThread m_data_processor_thread; /**< Thread with a running Data Processor. */
    QMap<ReaderId, std::shared_ptr<UniversalReaderDialogConfig>>
            m_readers_config; /**< Readers configuration. */

    QStandardItemModel *m_source_list_model = nullptr; /**< Source List model. */

    ScopeMode m_current_mode = ScopeMode::Stopped; /**< Current display mode. */
    int64_t m_div_horizontal_us{ }; /**< Size of one horizontal division in us. */
    std::vector<int64_t> m_div_vertical_uval = std::vector<int64_t>(
            channels_amount,
            default_div_vertical_uval); /**< Sizes of vertical division in 10^-6. */

    UData::Time m_overview_min_time{ }; /**< Min overview graph time. */
    UData::Time m_overview_max_time{ }; /**< Max overview graph time. */

    MainChartController m_mainchart_controller; /**< Controller of the main chart. */
    ChannelBarModel m_channelbar_model; /**< Model for the channel bar. */

    /**
     * @brief Initialize and run Data Processor.
     */
    void init_data_processor();

    /**
     * @brief Initialize graph and other ui elements.
     */
    void init_ui_elements();

    /**
     * @brief Initialize graph.
     */
    void init_graph();

    /**
     * @brief Initialize source list.
     */
    void init_source_list();

    /**
     * @brief Get available reader index.
     *
     * @return Available reader index.
     */
    ReaderId get_available_reader_idx() const;

    /**
     * @brief Add reader to the source list and data processor.
     *
     * @param config Configuration of the reader.
     */
    void add_reader(const std::shared_ptr<UniversalReaderDialogConfig> &config);

signals:
    /**
     * @brief Send reader configuration to the Data Processor
     *
     * If the reader exists, it changes the config of it.
     *
     * @param id reader id
     * @param config configuration of the reader
     */
    void configure_reader(ReaderId id, std::shared_ptr<UniversalReaderDialogConfig> config);

    /**
     * @brief Remove reader configuration from the Data Processor
     *
     * @param id reader id
     */
    void remove_reader(ReaderId id);

    /**
     * @brief Send correspondence between a variable and a channel to the Data Processor
     *
     * @param variable uniq identificator of a variable
     * @param channel_id id of the channel
     */
    void assign_channel(QPair<ReaderId, VariableId> variable, ChannelId channel_id);

    /**
     * @brief Enable channel in the Data Processor
     *
     * @param channel_id ID of the channel to be enabled.
     */
    void enable_channel(ChannelId channel_id);

    /**
     * @brief Disable channel in the Data Processor
     *
     * @param channel_id ID of the channel to be disable.
     */
    void disable_channel(ChannelId channel_id);

    /**
     * @brief Update vertical scale of a channel in the Data Processor
     *
     * @param channel_id ID of the channel to update.
     * @param scale New vertical scale for the channel.
     */
    void update_channel_vertical_scale(ChannelId channel_id, double scale);

    /**
     * @brief Request full history data from Data Processor to display.
     *
     * @param points_limit Maximum amount of points to be returned.
     */
    void request_full_history(int points_limit);

    /**
     * @brief Start data processing in the Data Processor.
     */
    void start_data_processing();

    /**
     * @brief Stop data processing in the Data Processor.
     */
    void stop_data_processing();

    /**
     * @brief Update sliding window width on the overview chart.
     *
     * @param window_width_us New width of the sliding window in microseconds.
     */
    void window_width_updated(int64_t window_width_us);

    /**
     * @brief Reset the sliding window on the overview chart.
     */
    void sliding_window_reset(UData::Time min_time, UData::Time max_time, int64_t window_width_us);

    /**
     * @brief Switch between continuous and stopped mode of the chart.
     *
     * @param on True to switch to continuous mode, false to switch to stopped mode.
     */
    void switch_continuous_mode(bool on);

    /**
     * @brief Set the horizontal division of the main chart.
     *
     * @param div_us Size of one horizontal division in microseconds.
     */
    void set_horizontal_div(int64_t div_us);

    /**
     * @brief Force refresh of the chart data.
     */
    void force_graph_refresh();

private slots:
    /**
     * @brief Show context menu for the source list.
     *
     * @param pos Position of the mouse cursor.
     */
    void source_list_context_menu(const QPoint &pos);

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
     * @brief Handle click on the start button.
     */
    void handle_start_clicked();

    /**
     * @brief Handle click on the stop button.
     */
    void handle_stop_clicked();

    /**
     * @brief Handle selection of the channel in the channel bar.
     *
     * @param channel_id ID of the selected channel.
     */
    void channel_selected(int channel_id);

    /**
     * @brief Handle toggling of the channel in the channel bar.
     *
     * @param channel_id ID of the toggled channel.
     */
    void channel_toggled(int channel_id);
};
