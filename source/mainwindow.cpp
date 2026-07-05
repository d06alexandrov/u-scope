#include "mainwindow.h"

#include "dataprocessor.h"
#include "simulatedreader_dialog.h"
#include "ui_mainwindow.h"

#include <QChart>
#include <QLineSeries>
#include <QThread>
#include <QValueAxis>

namespace {
/**
 * @brief Config an axis
 *
 * @param axis pointer to the axis
 * @param min minimum axis value
 * @param max maximum axis value
 * @param grid_cells amount of the grid cells
 * @param grid_color color of the grid
 */
void config_axis(QValueAxis *axis, int min, int max, int grid_cells, const QColor grid_color);
} // namespace

MainWindow::MainWindow()
    : QMainWindow(nullptr)
    , ui(new Ui::MainWindow)
    , m_source_list_model(new QStandardItemModel(this))
{
    ui->setupUi(this);

    init_graph();

    init_data_processor();

    init_source_list();
}

MainWindow::~MainWindow()
{
    if ((m_data_processor_thread != nullptr) && (m_data_processor_thread->isRunning())) {
        m_data_processor_thread->quit();

        if (!m_data_processor_thread->wait(1000)) {
            m_data_processor_thread->terminate();
            m_data_processor_thread->wait();
        }

        delete m_data_processor_thread;
    }
    delete ui;
}

void MainWindow::init_data_processor()
{
    m_data_processor_thread = new QThread;
    DataProcessor *data_processor =
            new DataProcessor(GraphStyle::left_bottom_corner, GraphStyle::right_top_corner);

    data_processor->moveToThread(m_data_processor_thread);

    connect(m_data_processor_thread, &QThread::started, data_processor, &DataProcessor::setup);

    connect(data_processor, &DataProcessor::send_new_data, this, &MainWindow::receive_new_data);

    connect(this, &MainWindow::configure_reader, data_processor, &DataProcessor::configure_reader);
    connect(this, &MainWindow::remove_reader, data_processor, &DataProcessor::remove_reader);
    connect(this, &MainWindow::assign_channel, data_processor, &DataProcessor::assign_channel);
    connect(this, &MainWindow::set_window_time_width, data_processor,
            &DataProcessor::set_time_width);

    connect(m_data_processor_thread, &QThread::finished, data_processor,
            &DataProcessor::deleteLater);

    connect(ui->pushButton_StartAll, &QPushButton::clicked, data_processor,
            &DataProcessor::start_data_processing);
    connect(ui->pushButton_StopAll, &QPushButton::clicked, data_processor,
            &DataProcessor::stop_data_processing);

    // Register Meta Type which will be used in a communication with Data Processor
    qRegisterMetaType<std::shared_ptr<UniversalReaderDialogConfig>>(
            "std::shared_ptr<UniversalReaderDialogConfig>");

    m_data_processor_thread->start();
}

void MainWindow::init_graph()
{
    // Configure graph
    auto axisX = new QValueAxis;

    config_axis(axisX, GraphStyle::left_bottom_corner.x(), GraphStyle::right_top_corner.x(),
                GraphStyle::horizontal_grid, GraphStyle::grid_line_color);

    auto axisY = new QValueAxis;

    config_axis(axisY, GraphStyle::left_bottom_corner.y(), GraphStyle::right_top_corner.y(),
                GraphStyle::vertical_grid, GraphStyle::grid_line_color);

    auto main_chart = ui->dataPlot->chart();

    main_chart->addAxis(axisX, Qt::AlignBottom);
    main_chart->addAxis(axisY, Qt::AlignLeft);

    main_chart->setBackgroundBrush(QBrush(GraphStyle::background_color));

    main_chart->legend()->hide();

    for (int i = 0; i < this->channels_amount; ++i) {
        m_series[i] = new QLineSeries(this);
        // TODO: check if series was created

        main_chart->addSeries(m_series[i]);

        m_series[i]->attachAxis(axisX);
        m_series[i]->attachAxis(axisY);
        m_series[i]->setPointsVisible(true);
    }
}

void MainWindow::init_source_list()
{
    ui->sourceList->setModel(m_source_list_model);

    connect(ui->sourceList, &QTreeView::customContextMenuRequested, this,
            &MainWindow::source_list_context_menu);
}

ReaderId MainWindow::get_available_reader_idx()
{
    ReaderId reader_id = 0;

    for (auto it = m_readers_config.lowerBound(reader_id); it != m_readers_config.end(); it++) {
        if (it.key() == reader_id) {
            reader_id++;
        } else if (it.key() > reader_id) {
            break;
        }
    }

    if (reader_id >= this->readers_amount) {
        throw std::range_error("Reader amount limit exceeded");
    }

    return reader_id;
}

void MainWindow::source_list_context_menu(const QPoint &pos)
{
    QModelIndex index = ui->sourceList->indexAt(pos);

    QMenu contextMenu(this);

    if (index.isValid()) {
        auto existing_item = m_source_list_model->itemFromIndex(index);

        if (existing_item == nullptr) {
            return;
        }

        const ReaderId reader_id = existing_item->data(ItemRoles::ReaderIdRole).value<ReaderId>();

        if (existing_item->data(ItemRoles::VariableIdRole).isValid()) {
            // Variable item was clicked
            const VariableId variable_id =
                    existing_item->data(ItemRoles::VariableIdRole).value<VariableId>();
            QMenu *assign_channel_submenu = contextMenu.addMenu("Assign to channel");

            for (size_t ch_num = 0; ch_num < this->channels_amount; ch_num++) {
                QAction *channel_assign_action =
                        assign_channel_submenu->addAction(tr("Channel %1").arg(ch_num + 1));

                connect(channel_assign_action, &QAction::triggered, this,
                        [this, reader_id, variable_id, ch_num]() {
                            emit assign_channel(qMakePair(reader_id, variable_id), ch_num);
                        });
            }
        } else {
            // Reader item was clicked
            QAction *modify_source_action = contextMenu.addAction("Modify existing source");
            QAction *delete_source_action = contextMenu.addAction("Delete existing source");

            modify_source_action->setEnabled(false);
            connect(delete_source_action, &QAction::triggered, this, [this, index, reader_id]() {
                emit remove_reader(reader_id);
                // TODO: remove correspondence between reader variables and channels

                // TODO: gracefully remove config to prevent any issues during the data transmit
                // from data processor to main window

                m_readers_config.remove(reader_id);
                m_source_list_model->removeRow(index.row(), index.parent());
            });
        }
    } else {
        if (m_readers_config.size() >= this->readers_amount) {
            QAction *new_source_action = contextMenu.addAction("Source amount limit was achieved");
            new_source_action->setEnabled(false);
        } else {
            QAction *new_source_action = contextMenu.addAction("Configure new simulated source");

            connect(new_source_action, &QAction::triggered, this, [this, index]() {
                SimulatedReaderDialog dialog(this);

                if (dialog.exec() == QDialog::Accepted) {
                    ReaderId new_reader_id = get_available_reader_idx();

                    const auto config = dialog.get_config();

                    m_readers_config.insert(new_reader_id, config);

                    QStandardItem *new_item = new QStandardItem(tr("Source %1").arg(new_reader_id));
                    new_item->setData(QVariant::fromValue(new_reader_id), ItemRoles::ReaderIdRole);
                    m_source_list_model->appendRow(new_item);

                    if (!config->variable_names.isEmpty()) {
                        for (const auto [id, name] : config->variable_names.asKeyValueRange()) {
                            QStandardItem *new_variable_item =
                                    new QStandardItem(tr("#%1 %2").arg(id).arg(name));
                            new_variable_item->setData(QVariant::fromValue(new_reader_id),
                                                       ItemRoles::ReaderIdRole);
                            new_variable_item->setData(QVariant::fromValue(id),
                                                       ItemRoles::VariableIdRole);
                            new_item->appendRow(new_variable_item);
                        }
                    }

                    emit configure_reader(new_reader_id, config);
                }
            });
        }
    }

    contextMenu.exec(ui->sourceList->viewport()->mapToGlobal(pos));
}

void MainWindow::receive_new_data(const QList<GraphData> &new_data)
{
    for (auto &channel_data : new_data) {
        if (channel_data.get_id() < this->channels_amount) {
            m_series[channel_data.get_id()]->replace(channel_data.get_values());
        }
    }
}

namespace {
void config_axis(QValueAxis *axis, int min, int max, int grid_cells, const QColor grid_color)
{
    axis->setRange(min, max);

    // Define grid line style
    QPen gridPen;
    gridPen.setColor(grid_color);
    gridPen.setWidth(1);
    axis->setGridLinePen(gridPen);
    axis->setGridLineVisible(true);

    // Hide axis labels and main line
    axis->setLabelsVisible(false);
    axis->setLineVisible(false);

    axis->setTickCount(grid_cells + 1);
}
} // namespace
