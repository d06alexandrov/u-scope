#pragma once

#include "dataprocessor.h"
#include "universalreader.h"
#include "universalreader_dialog.h"

#include <QChart>
#include <QLineSeries>
#include <QMainWindow>
#include <QStandardItemModel>

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

    /**
     * @brief Roles of the items in the source list.
     */
    enum ItemRoles {
        ReaderIdRole = Qt::UserRole + 1, /**< Role for the reader id. */
        VariableIdRole = Qt::UserRole + 2, /**< Role for the variable id. */
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
    QThread m_data_processor_thread; /**< Thread with a running Data Processor. */
    QMap<ReaderId, std::shared_ptr<UniversalReaderDialogConfig>>
            m_readers_config; /**< Readers configuration. */

    QStandardItemModel *m_source_list_model = nullptr; /**< Source List model. */

    void init_data_processor(); /**< Initialize and run Data Processor. */
    void init_graph(); /**< Initialize graph. */
    void init_source_list(); /**< Initialize source list. */

    ReaderId get_available_reader_idx(); /**< Get first available reader index. */

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
     * @brief Set width of the displayed window in microseconds
     *
     * @param us window width in microseconds
     */
    void set_window_time_width(int64_t us);

private slots:
    void source_list_context_menu(const QPoint &pos);

public slots:
    void receive_new_data(const QList<GraphData> &new_data); /**< Slot to receive graph data
                                                                from Data Processor */
};
