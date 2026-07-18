#pragma once

#include "dataprocessor.h"
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
    static constexpr int64_t default_time_width = 1000000; /**< Default window width in us. */
    static constexpr int default_frame_period = 33; /**< Default graph frame update period in ms. */

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

    Ui::MainWindow *ui = nullptr; /**< Pointer to the Main Window user interface. */
    QLineSeries *m_series[channels_amount]; /**< Pointer to Chart's series'. */
    QValueAxis *m_axis_x = nullptr;
    QValueAxis *m_axis_y = nullptr;

    QThread m_data_processor_thread; /**< Thread with a running Data Processor. */
    QMap<ReaderId, std::shared_ptr<UniversalReaderDialogConfig>>
            m_readers_config; /**< Readers configuration. */
    QTimer m_render_timer;

    QStandardItemModel *m_source_list_model = nullptr; /**< Source List model. */

    ScopeMode m_current_mode = ScopeMode::Stopped; /**< Current display mode. */
    int64_t m_time_width_us = default_time_width; /**< Window width in us. */

    /**
     * @brief Initialize and run Data Processor.
     */
    void init_data_processor();

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
    ReaderId get_available_reader_idx();

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
     * @brief Reuqest stored data from Data Processor to display.
     *
     * @param start_time Start time of the requested data.
     * @param end_time End time of the requested data.
     * @param points_limit Maximum amount of points to be displayed on the graph.
     */
    void request_stored_data(UData::Time start_time, UData::Time end_time, int points_limit);

    /**
     * @brief Start data processing in the Data Processor.
     */
    void start_data_processing();

    /**
     * @brief Stop data processing in the Data Processor.
     */
    void stop_data_processing();

private slots:
    /**
     * @brief Show context menu for the source list.
     *
     * @param pos Position of the mouse cursor.
     */
    void source_list_context_menu(const QPoint &pos);

public slots:
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
     * @brief Handle click on the start button.
     */
    void handle_start_clicked();

    /**
     * @brief Handle click on the stop button.
     */
    void handle_stop_clicked();
};
