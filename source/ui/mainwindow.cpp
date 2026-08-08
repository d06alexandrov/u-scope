#include "mainwindow.h"

#include "dataprocessor.h"
#include "serialreader_dialog.h"
#include "simulatedreader_dialog.h"
#include "sliding_window.hpp"
#include "ui_mainwindow.h"

#include <QAction>
#include <QChart>
#include <QFile>
#include <QGraphicsLayout>
#include <QLineSeries>
#include <QMessageBox>
#include <QObject>
#include <QQmlContext>
#include <QQuickItem>
#include <QThread>
#include <QValueAxis>
#include <QtCharts/QAbstractAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QXYSeries>
#include <algorithm>

namespace {
/**
 * @brief Powers of 10.
 */
constexpr std::array<int64_t, 7> powers_of_10 = {
    1LL, 10LL, 100LL, 1000LL, 10000LL, 100000LL, 1000000LL,
};

/**
 * @brief Config an axis
 *
 * @param axis Pointer to the axis.
 * @param min Minimum axis value.
 * @param max Maximum axis value.
 * @param grid_cells Amount of the grid cells.
 * @param grid_color Color of the grid.
 */
void config_axis(QValueAxis *axis, int min, int max, int grid_cells, const QColor grid_color);

/**
 * @brief Convert horizontal division in us to QDial value.
 *
 * @param division_us Horizontal division in us.
 * @return QDial value.
 */
int horizontal_div_us_to_qdial_value(int64_t division_us);

/**
 * @brief Convert QDial value to horizontal division in us.
 *
 * @param value QDial value.
 * @return Horizontal division in us.
 */
int64_t horizontal_qdial_value_to_div_us(int value);

/**
 * @brief Convert horizontal division in us to a string representation.
 *
 * @param us Horizontal division in us.
 * @return String representation of the horizontal division.
 */
QString scale_to_string(int64_t us);
} // namespace

MainWindow::MainWindow()
    : QMainWindow(nullptr)
    , ui(new Ui::MainWindow)
    , m_source_list_model(new QStandardItemModel(this))
    , m_channelbar_model(channel_colors)
{
    ui->setupUi(this);

    init_data_processor();

    init_ui_elements();

    init_graph_rendering();
}

MainWindow::~MainWindow()
{
    if (m_data_processor_thread.isRunning()) {
        m_data_processor_thread.quit();

        if (!m_data_processor_thread.wait(1000)) {
            m_data_processor_thread.terminate();
            m_data_processor_thread.wait();
        }
    }
    delete ui;
}

void MainWindow::init_data_processor()
{
    DataProcessor *data_processor = new DataProcessor;

    data_processor->moveToThread(&m_data_processor_thread);

    connect(&m_data_processor_thread, &QThread::started, data_processor, &DataProcessor::setup);

    connect(this, &MainWindow::request_stored_data, data_processor,
            &DataProcessor::handle_data_request);
    connect(this, &MainWindow::request_recent_stored_data, data_processor,
            &DataProcessor::handle_recent_data_request);
    connect(this, &MainWindow::request_full_history, data_processor,
            &DataProcessor::handle_full_history_request);
    connect(data_processor, &DataProcessor::send_new_data, this, &MainWindow::receive_stored_data);
    connect(data_processor, &DataProcessor::send_full_history, this,
            &MainWindow::receive_full_history);

    connect(this, &MainWindow::configure_reader, data_processor, &DataProcessor::configure_reader);
    connect(this, &MainWindow::remove_reader, data_processor, &DataProcessor::remove_reader);
    connect(this, &MainWindow::assign_channel, data_processor, &DataProcessor::assign_channel);

    connect(&m_data_processor_thread, &QThread::finished, data_processor,
            &DataProcessor::deleteLater);

    connect(this, &MainWindow::start_data_processing, data_processor,
            &DataProcessor::start_data_processing);
    connect(this, &MainWindow::stop_data_processing, data_processor,
            &DataProcessor::stop_data_processing);

    // Register Meta Type which will be used in a communication with Data Processor
    qRegisterMetaType<std::shared_ptr<UniversalReaderDialogConfig>>(
            "std::shared_ptr<UniversalReaderDialogConfig>");

    m_data_processor_thread.start();
}

void MainWindow::init_ui_elements()
{
    init_graph();

    init_source_list();

    // Initialize channel bar with channel badges
    QVariantList color_list;
    for (const QColor &color : channel_colors) {
        color_list.append(color);
    }

    ui->qmlScreenView->setResizeMode(QQuickWidget::SizeRootObjectToView);
    ui->qmlScreenView->rootContext()->setContextProperty("mainWindow", this);
    ui->qmlScreenView->rootContext()->setContextProperty("channelModel", &m_channelbar_model);
    ui->qmlScreenView->rootContext()->setContextProperty("cppChannelColors", color_list);
    ui->qmlScreenView->setSource(QUrl(QStringLiteral("qrc:/qt/qml/UI/ScreenRoot.qml")));

    QQuickItem *root = ui->qmlScreenView->rootObject();

    if (root != nullptr) {
        m_main_chart_item = root->findChild<QQuickItem *>("mainChart");

        if (m_main_chart_item != nullptr) {
            QMetaObject::invokeMethod(m_main_chart_item, "getAxisX", Qt::DirectConnection,
                                      Q_RETURN_ARG(QValueAxis *, m_axis_x));

            for (int i = 0; i < channels_amount; ++i) {
                QAbstractSeries *absSeries = nullptr;
                QMetaObject::invokeMethod(m_main_chart_item, "getSeries", Qt::DirectConnection,
                                          Q_RETURN_ARG(QAbstractSeries *, absSeries),
                                          Q_ARG(int, i));

                m_series[i] = qobject_cast<QXYSeries *>(absSeries);

                if (m_series[i] == nullptr) {
                    qFatal() << tr("Failed to find a main chart series # %1.").arg(i);
                }
            }
        } else {
            qFatal() << tr("Failed to get an access to Main Chart qml.");
        }
    } else {
        qFatal() << tr("Failed to get an access to Screen Root qml.");
    }

    // Connect start and stop buttons
    connect(ui->pushButton_StartAll, &QPushButton::clicked, this,
            &MainWindow::handle_start_clicked);
    connect(ui->pushButton_StopAll, &QPushButton::clicked, this, &MainWindow::handle_stop_clicked);

    // Initialize horizontal scaler
    connect(ui->horizontalScale, &QDial::valueChanged, this, [this](int new_value) {
        m_div_horizontal_us = horizontal_qdial_value_to_div_us(new_value);
        ui->hScaleValue->setText(scale_to_string(m_div_horizontal_us));
        emit window_width_updated(m_div_horizontal_us * GraphStyle::horizontal_grid);
    });
    ui->horizontalScale->setValue(horizontal_div_us_to_qdial_value(default_div_horizontal_us));
    ui->hScaleValue->setText(scale_to_string(default_div_horizontal_us));
    // Explicitly emit the signal, if horizontalScale value was not changed
    emit window_width_updated(m_div_horizontal_us * GraphStyle::horizontal_grid);

    // Initialize elements of the menu
    connect(ui->actionAbout, &QAction::triggered, this, [this](bool checked) {
        QMessageBox about_box(this);
        about_box.setWindowTitle(tr("About %1").arg(QCoreApplication::applicationName()));
        about_box.setText(
                tr("<h3>%1</h3>"
                   "<p>Version %2</p>"
                   "<p>Built with Qt %3</p>"
                   "This program is free software released under the GNU General Public License.")
                        .arg(QCoreApplication::applicationName())
                        .arg(QCoreApplication::applicationVersion())
                        .arg(QT_VERSION_STR));

        QFile license_file(QStringLiteral(":/ui/LICENSE"));
        if (license_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            about_box.setDetailedText(QString::fromUtf8(license_file.readAll()));
        }

        about_box.exec();
    });
}

void MainWindow::init_graph()
{
    // Configure overview graph
    auto overview_chart = ui->dataOverview->chart();

    overview_chart->setBackgroundBrush(QBrush(GraphStyle::background_color));

    overview_chart->legend()->hide();

    overview_chart->setMargins(QMargins(0, 0, 0, 0));
    overview_chart->layout()->setContentsMargins(0, 0, 0, 0);

    m_overview_axis_x = new QValueAxis;
    m_overview_axis_y = new QValueAxis;

    m_overview_axis_x->setRange(0, 1);
    m_overview_axis_y->setRange(-100, 100);

    overview_chart->addAxis(m_overview_axis_x, Qt::AlignBottom);
    overview_chart->addAxis(m_overview_axis_y, Qt::AlignLeft);

    m_overview_axis_x->setVisible(false);
    m_overview_axis_y->setVisible(false);

    for (int i = 0; i < this->channels_amount; ++i) {
        m_series_overview[i] = new QLineSeries(this);

        overview_chart->addSeries(m_series_overview[i]);

        m_series_overview[i]->attachAxis(m_overview_axis_x);
        m_series_overview[i]->attachAxis(m_overview_axis_y);
        m_series_overview[i]->setPen(QPen(channel_colors[i]));
    }

    // Add sliding window to the overview graph
    m_sliding_window = new SlidingWindow();

    overview_chart->scene()->addItem(m_sliding_window);
    m_sliding_window->setRect(0, 0, overview_chart->plotArea().width(),
                              overview_chart->plotArea().height());

    connect(this, &MainWindow::window_width_updated, m_sliding_window,
            &SlidingWindow::set_window_width);
    connect(this, &MainWindow::sliding_window_reset, m_sliding_window,
            &SlidingWindow::reset_window_to_right);

    connect(m_sliding_window, &SlidingWindow::position_changed, this,
            [this](UData::Time window_min_time, UData::Time window_max_time) {
                int pixel_width =
                        std::max(minimum_graph_render_width,
                                 static_cast<int>(m_main_chart_item->viewportItem()->width()));
                emit request_stored_data(window_min_time, window_max_time, pixel_width);
            });

    connect(overview_chart, &QChart::plotAreaChanged, m_sliding_window,
            &SlidingWindow::plot_area_changed);
}

void MainWindow::init_source_list()
{
    ui->sourceList->setModel(m_source_list_model);

    connect(ui->sourceList, &QTreeView::customContextMenuRequested, this,
            &MainWindow::source_list_context_menu);
}

void MainWindow::init_graph_rendering()
{
    m_render_timer.setTimerType(Qt::PreciseTimer);

    connect(&m_render_timer, &QTimer::timeout, this, [this]() {
        if (m_current_mode == ScopeMode::Roll) {
            int64_t time_width_us = m_div_horizontal_us * GraphStyle::horizontal_grid;
            UData::Time end_time = UData::get_timestamp();

            int pixel_width =
                    std::max(minimum_graph_render_width,
                             static_cast<int>(m_main_chart_item->viewportItem()->width()));

            emit request_recent_stored_data(end_time, time_width_us, default_frame_period_ms * 1000,
                                            pixel_width);
        }
    });
}

ReaderId MainWindow::get_available_reader_idx() const
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

void MainWindow::add_reader(const std::shared_ptr<UniversalReaderDialogConfig> &config)
{
    ReaderId new_reader_id = get_available_reader_idx();

    m_readers_config.insert(new_reader_id, config);

    QStandardItem *new_item = new QStandardItem(tr("Source %1").arg(new_reader_id));
    new_item->setData(QVariant::fromValue(new_reader_id), ItemRoles::ReaderIdRole);
    m_source_list_model->appendRow(new_item);

    if (!config->variable_names.isEmpty()) {
        for (const auto [id, name] : config->variable_names.asKeyValueRange()) {
            QStandardItem *new_variable_item = new QStandardItem(tr("#%1 %2").arg(id).arg(name));
            new_variable_item->setData(QVariant::fromValue(new_reader_id), ItemRoles::ReaderIdRole);
            new_variable_item->setData(QVariant::fromValue(id), ItemRoles::VariableIdRole);
            new_item->appendRow(new_variable_item);
        }
    }

    emit configure_reader(new_reader_id, config);
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

            for (ChannelId ch_num = 1; ch_num <= this->channels_amount; ch_num++) {
                QAction *channel_assign_action =
                        assign_channel_submenu->addAction(tr("Channel %1").arg(ch_num));

                connect(channel_assign_action, &QAction::triggered, this,
                        [this, reader_id, variable_id, ch_num]() {
                            emit assign_channel(qMakePair(reader_id, variable_id), ch_num);
                            m_channelbar_model.connect_channel(ch_num);
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
            QAction *new_simulated_source_action =
                    contextMenu.addAction("Configure new simulated source");

            connect(new_simulated_source_action, &QAction::triggered, this, [this, index]() {
                SimulatedReaderDialog dialog(this);

                if (dialog.exec() == QDialog::Accepted) {
                    add_reader(dialog.get_config());
                }
            });

            QAction *new_serial_source_action =
                    contextMenu.addAction("Configure new serial port source");

            connect(new_serial_source_action, &QAction::triggered, this, [this, index]() {
                SerialReaderDialog dialog(this);

                if (dialog.exec() == QDialog::Accepted) {
                    add_reader(dialog.get_config());
                }
            });
        }
    }

    contextMenu.exec(ui->sourceList->viewport()->mapToGlobal(pos));
}

void MainWindow::receive_stored_data(const QList<GraphData> &new_data,
                                     UData::Time requested_start_time,
                                     UData::Time requested_end_time)
{
    m_axis_x->setRange(requested_start_time, requested_end_time);

    for (auto &channel_data : new_data) {
        if (channel_data.get_id() < this->channels_amount) {
            m_series[channel_data.get_id() - 1]->replace(channel_data.get_values());
        }
    }
}

void MainWindow::receive_full_history(const QList<GraphData> &new_data, UData::Time min_time,
                                      UData::Time max_time)
{
    if (new_data.empty() || (max_time <= min_time)) {
        return;
    }

    m_overview_axis_x->setRange(min_time, max_time);

    m_overview_min_time = min_time;
    m_overview_max_time = max_time;

    for (int i = 0; i < this->channels_amount; ++i) {
        m_series_overview[i]->clear();
    }

    for (auto &channel_data : new_data) {
        if (channel_data.get_id() < this->channels_amount) {
            m_series_overview[channel_data.get_id() - 1]->replace(channel_data.get_values());
        }
    }

    int64_t time_width_us = m_div_horizontal_us * GraphStyle::horizontal_grid;

    emit sliding_window_reset(m_overview_min_time, m_overview_max_time, time_width_us);
}

void MainWindow::handle_start_clicked()
{
    m_current_mode = ScopeMode::Roll;

    m_render_timer.start(default_frame_period_ms);

    emit start_data_processing();
}

void MainWindow::handle_stop_clicked()
{
    m_render_timer.stop();

    emit stop_data_processing();

    emit request_full_history(maximum_overview_points);

    m_current_mode = ScopeMode::Stopped;
}

void MainWindow::channel_selected(int channel_id)
{
    m_channelbar_model.select_channel(channel_id);
}

void MainWindow::channel_toggled(int channel_id)
{
    // TODO: enable or disable channel readings
    if (m_channelbar_model.is_enabled(channel_id)) {
        m_channelbar_model.disable_channel(channel_id);
    } else {
        m_channelbar_model.enable_channel(channel_id);
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

int horizontal_div_us_to_qdial_value(int64_t division_us)
{
    if (division_us <= 0) {
        // It should never happen
        return 0;
    }

    int log10 = 0;

    // Take log10 and round up
    for (int64_t value_left = division_us - 1; value_left > 0; value_left /= 10) {
        log10++;
    }

    return log10;
}

int64_t horizontal_qdial_value_to_div_us(int value)
{
    return powers_of_10.at(std::clamp(value, 0, static_cast<int>(powers_of_10.size() - 1)));
}

QString scale_to_string(int64_t us)
{
    if (us < 1000LL) {
        return QObject::tr("%1 us").arg(us);
    } else if (us < 1000000LL) {
        return QObject::tr("%1 ms").arg(static_cast<double>(us) / 1000.0);
    } else {
        return QObject::tr("%1 s").arg(static_cast<double>(us) / 1000000.0);
    }
}

} // namespace
