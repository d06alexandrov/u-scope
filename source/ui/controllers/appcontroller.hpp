#pragma once

#include "channelbar_model.hpp"
#include "dataprocessor.hpp"
#include "mainchart_controller.hpp"
#include "overviewchart_controller.hpp"
#include "sourcelist_controller.hpp"
#include "timebase_model.hpp"
#include "verticalscale_model.hpp"

#include <QPropertyChangeHandler>
#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include <memory>

/**
 * @brief Class that implements main controller
 */
class AppController : public QObject
{
    Q_OBJECT

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    Q_PROPERTY(SourceListController *sourceList READ sourceList CONSTANT)
    Q_PROPERTY(MainChartController *mainChart READ mainChart CONSTANT)
    Q_PROPERTY(OverviewChartController *overviewChart READ overviewChart CONSTANT)
    Q_PROPERTY(ChannelBarModel *channelModel READ channelModel CONSTANT)
    Q_PROPERTY(TimebaseModel *timebaseModel READ timebaseModel CONSTANT)
    Q_PROPERTY(VerticalScaleModel *verticalScaleModel READ verticalScaleModel CONSTANT)
    Q_PROPERTY(QVariantList channelColors READ channelColors CONSTANT)
#endif // DOXYGEN_SHOULD_SKIP_THIS

public:
    /**
     * @brief Constructor of AppController class.
     */
    AppController();

    AppController(const AppController &other) = delete;
    AppController(AppController &&other) = delete;

    /**
     * @brief Destructor of AppController class.
     */
    ~AppController() override;

    AppController &operator=(const AppController &other) = delete;
    AppController &operator=(AppController &&other) = delete;

    /**
     * @brief Show about dialog with information about the application.
     */
    Q_INVOKABLE void about_menu();

    /**
     * @brief Get pointer to the SourceListController.
     *
     * @return Pointer to the SourceListController.
     */
    [[nodiscard]] SourceListController *sourceList();
    /**
     * @brief Get pointer to the MainChartController.
     *
     * @return Pointer to the MainChartController.
     */
    [[nodiscard]] MainChartController *mainChart();
    /**
     * @brief Get pointer to the OverviewChartController.
     *
     * @return Pointer to the OverviewChartController.
     */
    [[nodiscard]] OverviewChartController *overviewChart();
    /**
     * @brief Get pointer to the ChannelBarModel.
     *
     * @return Pointer to the ChannelBarModel.
     */
    [[nodiscard]] ChannelBarModel *channelModel();
    /**
     * @brief Get pointer to the TimebaseModel.
     *
     * @return Pointer to the TimebaseModel.
     */
    [[nodiscard]] TimebaseModel *timebaseModel();
    /**
     * @brief Get pointer to the VerticalScaleModel.
     *
     * @return Pointer to the VerticalScaleModel.
     */
    [[nodiscard]] VerticalScaleModel *verticalScaleModel();
    /**
     * @brief Get list of channel colors.
     *
     * @return List of channel colors.
     */
    [[nodiscard]] QVariantList channelColors() const;

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

    std::unique_ptr<DataProcessor> m_data_processor{ }; /**< Main Data Processor. */
    QThread m_data_processor_thread; /**< Thread with a running Data Processor. */

    ScopeMode m_current_mode = ScopeMode::Stopped; /**< Current display mode. */

    SourceListController m_sourcelist_controller; /**< Controller of the source list. */
    MainChartController m_mainchart_controller; /**< Controller of the main chart. */
    OverviewChartController m_overviewchart_controller; /**< Controller of the overview chart. */
    ChannelBarModel m_channelbar_model; /**< Model for the channel bar. */
    TimebaseModel m_timebase_model; /**< Model for horizontal timebase. */
    VerticalScaleModel m_verticalscale_model; /**< Model for vertical scale. */

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
     * @brief Initialize ui input elements.
     */
    void init_input();

    /**
     * @brief Initialize source list.
     */
    void init_source_list();
};
