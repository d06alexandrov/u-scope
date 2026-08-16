#pragma once

#include "channelbar_model.h"
#include "dataprocessor.h"
#include "mainchart_controller.h"
#include "overviewchart_controller.h"
#include "sourcelist_controller.h"
#include "timebase_model.h"
#include "verticalscale_model.h"

#include <QMainWindow>
#include <QPropertyChangeHandler>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

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
     * @brief Switch between continuous and stopped mode of the chart.
     *
     * @param on True to switch to continuous mode, false to switch to stopped mode.
     */
    void switch_continuous_mode(bool on);

    /**
     * @brief Force refresh of the chart data.
     */
    void force_graph_refresh();

private slots:

private:
    static constexpr size_t channels_amount = 12; /**< Amount of channels. */

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
