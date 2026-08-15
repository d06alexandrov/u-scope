#include "mainwindow.h"

#include "dataprocessor.h"
#include "serialreader_dialog.h"
#include "simulatedreader_dialog.h"
#include "ui_mainwindow.h"

#include <QAction>
#include <QFile>
#include <QMessageBox>
#include <QObject>
#include <QQmlContext>
#include <QQuickItem>
#include <QSignalBlocker>
#include <QThread>
#include <QtCharts/QValueAxis>
#include <QtCharts/QXYSeries>

namespace {

/**
 * @brief Extracts QValueAxis and QXYSeries from a QQuickItem representing a graph.
 *
 * @param root The root QQuickItem containing the graph.
 * @param graph_name The name of the graph to extract.
 * @param channels_amount The expected number of QXYSeries in the graph.
 * @return The QValueAxis and a vector of QXYSeries if successful, or std::nullopt if fails.
 */
std::optional<std::tuple<QValueAxis *, std::vector<QXYSeries *>>>
extract_graph_series(QQuickItem *root, QStringView graph_name, size_t channels_amount);

} // namespace

MainWindow::MainWindow()
    : QMainWindow(nullptr)
    , ui(new Ui::MainWindow)
    , m_source_list_model(new QStandardItemModel(this))
    , m_channelbar_model(channel_colors)
    , m_verticalscale_model(channels_amount)
{
    ui->setupUi(this);

    init_data_processor();

    init_ui_elements();
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

void MainWindow::handle_start_clicked()
{
    m_current_mode = ScopeMode::Roll;

    emit switch_continuous_mode(true);

    emit start_data_processing();
}

void MainWindow::handle_stop_clicked()
{
    emit switch_continuous_mode(false);

    emit stop_data_processing();

    m_current_mode = ScopeMode::Stopped;
}

void MainWindow::channel_selected(int channel_id)
{
    if (channel_id < 0 || channel_id >= this->channels_amount) {
        return;
    }

    auto id = static_cast<ChannelId>(channel_id);

    m_channelbar_model.select_channel(id);

    if (m_channelbar_model.is_selected(id)) {
        ui->verticalScale->setEnabled(true);
        ui->verticalScale->setValue(m_verticalscale_model.qDialValue(id));
    }
}

void MainWindow::channel_toggled(int channel_id)
{
    if (channel_id < 0 || channel_id >= this->channels_amount) {
        return;
    }

    auto id = static_cast<ChannelId>(channel_id);

    // TODO: enable or disable channel readings
    if (m_channelbar_model.is_enabled(id)) {
        m_channelbar_model.disable_channel(id);

        emit disable_channel(id);

        if (m_current_mode == ScopeMode::Stopped) {
            emit force_graph_refresh();
        }
    } else {
        m_channelbar_model.enable_channel(id);

        emit enable_channel(id);
    }
}

void MainWindow::init_data_processor()
{
    m_data_processor = new DataProcessor;

    m_data_processor->moveToThread(&m_data_processor_thread);

    connect(&m_data_processor_thread, &QThread::started, m_data_processor, &DataProcessor::setup);

    // Data Processor commands
    connect(this, &MainWindow::configure_reader, m_data_processor,
            &DataProcessor::configure_reader);
    connect(this, &MainWindow::remove_reader, m_data_processor, &DataProcessor::remove_reader);
    connect(this, &MainWindow::assign_channel, m_data_processor, &DataProcessor::assign_channel);
    connect(this, &MainWindow::enable_channel, m_data_processor, &DataProcessor::enable_channel);
    connect(this, &MainWindow::disable_channel, m_data_processor, &DataProcessor::disable_channel);
    connect(this, &MainWindow::update_channel_vertical_scale, m_data_processor,
            &DataProcessor::update_channel_vertical_scale);

    connect(&m_data_processor_thread, &QThread::finished, m_data_processor,
            &DataProcessor::deleteLater);

    connect(this, &MainWindow::start_data_processing, m_data_processor,
            &DataProcessor::start_data_processing);
    connect(this, &MainWindow::stop_data_processing, m_data_processor,
            &DataProcessor::stop_data_processing);

    // Register Meta Type which will be used in a communication with Data Processor
    qRegisterMetaType<std::shared_ptr<UniversalReaderDialogConfig>>(
            "std::shared_ptr<UniversalReaderDialogConfig>");

    m_data_processor_thread.start();
}

void MainWindow::init_ui_elements()
{
    init_qml();

    init_source_list();

    init_graph();

    init_input();

    init_menu();
}

void MainWindow::init_qml()
{
    QVariantList color_list;
    for (const QColor &color : channel_colors) {
        color_list.append(color);
    }

    ui->qmlScreenView->setResizeMode(QQuickWidget::SizeRootObjectToView);
    ui->qmlScreenView->rootContext()->setContextProperties({
            { "mainWindow", QVariant::fromValue(this) },
            { "mainChartController", QVariant::fromValue(&m_mainchart_controller) },
            { "overviewChartController", QVariant::fromValue(&m_overviewchart_controller) },
            { "channelModel", QVariant::fromValue(&m_channelbar_model) },
            { "timebaseModel", QVariant::fromValue(&m_timebase_model) },
            { "verticalScaleModel", QVariant::fromValue(&m_verticalscale_model) },
            { "cppChannelColors", color_list },
    });
    ui->qmlScreenView->setSource(QUrl(QStringLiteral("qrc:/qt/qml/UI/MainWindow.qml")));
}

void MainWindow::init_graph()
{
    QQuickItem *root = ui->qmlScreenView->rootObject();

    if (root != nullptr) {
        if (auto extract_result = extract_graph_series(root, u"mainChart", channels_amount);
            extract_result.has_value()) {
            auto [extracted_axis, extracted_series] = extract_result.value();

            m_mainchart_controller.attach_ui(extracted_axis, extracted_series);
        } else {
            qFatal() << tr("Failed to extract Main Chart series.");
        }

        if (auto extract_result = extract_graph_series(root, u"overviewChart", channels_amount);
            extract_result.has_value()) {
            auto [extracted_axis, extracted_series] = extract_result.value();

            m_overviewchart_controller.attach_ui(extracted_axis, extracted_series);

        } else {
            qFatal() << tr("Failed to extract Overview Chart series.");
        }

    } else {
        qFatal() << tr("Failed to get an access to Screen Root qml.");
    }

    // Main Chart Controller
    connect(this, &MainWindow::switch_continuous_mode, &m_mainchart_controller,
            &MainChartController::switch_continuous_mode);
    connect(this, &MainWindow::set_horizontal_div, &m_mainchart_controller,
            &MainChartController::set_horizontal_div);
    connect(this, &MainWindow::force_graph_refresh, &m_mainchart_controller,
            &MainChartController::force_graph_refresh);
    connect(m_data_processor, &DataProcessor::send_new_data, &m_mainchart_controller,
            &MainChartController::receive_stored_data);
    connect(&m_mainchart_controller, &MainChartController::request_recent_stored_data,
            m_data_processor, &DataProcessor::handle_recent_data_request);
    connect(&m_mainchart_controller, &MainChartController::request_stored_data, m_data_processor,
            &DataProcessor::handle_data_request);

    // Overview Chart Controller
    connect(this, &MainWindow::switch_continuous_mode, &m_overviewchart_controller,
            &OverviewChartController::switch_continuous_mode);
    connect(this, &MainWindow::force_graph_refresh, &m_overviewchart_controller,
            &OverviewChartController::force_graph_refresh);
    connect(this, &MainWindow::window_width_updated, &m_overviewchart_controller,
            &OverviewChartController::set_sliding_window_width);
    connect(m_data_processor, &DataProcessor::send_full_history, &m_overviewchart_controller,
            &OverviewChartController::receive_full_history);
    connect(&m_overviewchart_controller, &OverviewChartController::request_full_history,
            m_data_processor, &DataProcessor::handle_full_history_request);
    connect(&m_overviewchart_controller, &OverviewChartController::selected_time_frame,
            &m_mainchart_controller, &MainChartController::set_time_frame);
}

void MainWindow::init_input()
{
    // Initialize timebase model (horizontal scaler)
    connect(ui->horizontalScale, &QDial::valueChanged, &m_timebase_model,
            &TimebaseModel::dial_value_updated);

    m_timebase_sync_division =
            m_timebase_model.bindableDivisionUs().subscribe(std::function<void()>([this]() {
                m_mainchart_controller.set_horizontal_div(m_timebase_model.divisionUs());
                m_overviewchart_controller.set_sliding_window_width(
                        m_timebase_model.frameWidthUs());

                QSignalBlocker blocker(ui->horizontalScale);
                ui->horizontalScale->setValue(m_timebase_model.qDialValue());
            }));

    // Initialize vertical scaler model
    connect(ui->verticalScale, &QDial::valueChanged, this, [this](int new_value) {
        const auto selected_channel = m_channelbar_model.get_selected();

        if (selected_channel.has_value()) {
            m_verticalscale_model.dial_value_updated(selected_channel.value(), new_value);
        }
    });

    connect(&m_verticalscale_model, &VerticalScaleModel::vDivisionChanged, this, [this]() {
        const auto selected_channel = m_channelbar_model.get_selected();

        if (selected_channel.has_value()) {
            m_channelbar_model.set_channel_text(
                    selected_channel.value(),
                    m_verticalscale_model.vScaleText(selected_channel.value()));

            emit update_channel_vertical_scale(
                    selected_channel.value(),
                    m_verticalscale_model.vScaleFactor(selected_channel.value()));

            if (m_current_mode == ScopeMode::Stopped) {
                emit force_graph_refresh();
            }
        }
    });

    // Connect start and stop buttons
    connect(ui->pushButton_StartAll, &QPushButton::clicked, this,
            &MainWindow::handle_start_clicked);
    connect(ui->pushButton_StopAll, &QPushButton::clicked, this, &MainWindow::handle_stop_clicked);
}

void MainWindow::init_menu()
{
    // Initialize elements of the menu
    connect(ui->actionAbout, &QAction::triggered, this, [this](bool checked) {
        QMessageBox about_box(this);
        about_box.setWindowTitle(tr("About %1").arg(QCoreApplication::applicationName()));
        about_box.setText(tr("<h3>%1</h3>"
                             "<p>Version %2</p>"
                             "<p>Built with Qt %3</p>"
                             "This program is free software released under the GNU General "
                             "Public License.")
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

void MainWindow::init_source_list()
{
    ui->sourceList->setModel(m_source_list_model);

    connect(ui->sourceList, &QTreeView::customContextMenuRequested, this,
            &MainWindow::source_list_context_menu);
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

            for (ChannelId ch_num = 0; ch_num < this->channels_amount; ch_num++) {
                QAction *channel_assign_action =
                        assign_channel_submenu->addAction(tr("Channel %1").arg(ch_num + 1));

                connect(channel_assign_action, &QAction::triggered, this,
                        [this, reader_id, variable_id, ch_num]() {
                            m_verticalscale_model.reset_channel(ch_num);
                            m_channelbar_model.connect_channel(ch_num);
                            m_channelbar_model.enable_channel(
                                    ch_num, m_verticalscale_model.vScaleText(ch_num));
                            emit assign_channel(qMakePair(reader_id, variable_id), ch_num);
                            emit enable_channel(ch_num);
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

                // TODO: gracefully remove config to prevent any issues during the data
                // transmit from data processor to main window

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

namespace {

std::optional<std::tuple<QValueAxis *, std::vector<QXYSeries *>>>
extract_graph_series(QQuickItem *root, QStringView graph_name, size_t channels_amount)
{
    if (root == nullptr) {
        return std::nullopt;
    }

    auto graph_item = root->findChild<QQuickItem *>(graph_name);

    if (graph_item == nullptr) {
        return std::nullopt;
    }

    QValueAxis *extracted_axis = nullptr;

    QMetaObject::invokeMethod(graph_item, "getAxisX", Qt::DirectConnection,
                              Q_RETURN_ARG(QValueAxis *, extracted_axis));

    if (extracted_axis == nullptr) {
        return std::nullopt;
    }

    std::vector<QXYSeries *> extracted_series;
    extracted_series.reserve(channels_amount);

    for (size_t i = 0; i < channels_amount; ++i) {
        QAbstractSeries *absSeries = nullptr;
        QMetaObject::invokeMethod(graph_item, "getSeries", Qt::DirectConnection,
                                  Q_RETURN_ARG(QAbstractSeries *, absSeries),
                                  Q_ARG(int, static_cast<int>(i)));

        auto *xy_series = qobject_cast<QXYSeries *>(absSeries);

        if (xy_series == nullptr) {
            return std::nullopt;
        }

        extracted_series.push_back(xy_series);
    }

    return std::tuple{ extracted_axis, extracted_series };
}

} // namespace