#pragma once

#include "channelbar_model.h"
#include "dataprocessor.h"
#include "mainchart_controller.h"
#include "overviewchart_controller.h"
#include "sourcelist_controller.h"
#include "timebase_model.h"
#include "universalreader.h"
#include "universalreader_dialog.h"
#include "verticalscale_model.h"

#include <QMainWindow>
#include <QPropertyChangeHandler>

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
    /**
     * @brief Constructor of MainWindow class.
     */
    MainWindow();

    /**
     * @brief Destructor of MainWindow class.
     */
    ~MainWindow() override;

public slots:

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

private:
    static constexpr size_t channels_amount = 12; /**< Amount of channels. */
    static constexpr int64_t default_div_vertical_uval =
            1000000; /**< Default Size of one vertical division in 10^-6. */
    static constexpr int maximum_overview_points =
            2000; /**< Maximum amount of points of the overview chart. */

    /**
     * @brief Current state of the scope.
     */
    enum ScopeMode {
        Stopped, /**< Data gathering is stopped. */
        Roll, /**< Receive and display data in a continuous mode. */
        Normal, /**< Receive and display data in an oscilloscope normal mode. */
        OneShot, /**< One-shot trigger mode. */
    };

    const std::vector<QColor> channel_colors = {
        QColor("#FFFF00"), QColor("#00FFFF"), QColor("#FF00FF"), QColor("#00FF00"),
        QColor("#FF8000"), QColor("#0080FF"), QColor("#FF0080"), QColor("#80FF00"),
        QColor("#A060FF"), QColor("#FFD700"), QColor("#00F5FF"), QColor("#FF4500"),
    };

    Ui::MainWindow *ui = nullptr; /**< Pointer to the Main Window user interface. */

    DataProcessor *m_data_processor = nullptr; /**< Main Data Processor. */
    QThread m_data_processor_thread; /**< Thread with a running Data Processor. */

    ScopeMode m_current_mode = ScopeMode::Stopped; /**< Current display mode. */

    SourceListController m_sourcelist_controller; /**< Controller of the source list. */
    MainChartController m_mainchart_controller; /**< Controller of the main chart. */
    OverviewChartController m_overviewchart_controller; /**< Controller of the overview chart. */
    ChannelBarModel m_channelbar_model; /**< Model for the channel bar. */
    TimebaseModel m_timebase_model; /**< Model for horizontal timebase. */
    VerticalScaleModel m_verticalscale_model; /**< Model for vertical scale. */

    std::optional<QPropertyChangeHandler<std::function<void()>>>
            m_timebase_sync_division; /**< Property change handler for horizontal division. */

    /**
     * @brief Initialize and run Data Processor.
     */
    void init_data_processor();

    /**
     * @brief Initialize graph and other ui elements.
     */
    void init_ui_elements();

    /**
     * @brief Initialize qml elements.
     */
    void init_qml();

    /**
     * @brief Initialize graph.
     */
    void init_graph();

    /**
     * @brief Initialize ui input elements.
     */
    void init_input();

    /**
     * @brief Initialize menu.
     */
    void init_menu();

    /**
     * @brief Initialize source list.
     */
    void init_source_list();
};
