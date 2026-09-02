#include "appcontroller.hpp"

#include "appcontroller_singleton.hpp"

#include <QCoreApplication>
#include <QFile>
#include <QMessageBox>
#include <QQmlContext>
#include <QThread>

AppController::AppController()
    : QObject(nullptr)
    , m_channelbar_model(channel_colors)
    , m_verticalscale_model(channels_amount)
{
    AppControllerForeign::set_controller_instance(this);

    init_data_processor();

    init_ui_elements();
}

AppController::~AppController()
{
    // m_data_processor_thread will destroy underlying object
    m_data_processor.release();

    if (m_data_processor_thread.isRunning()) {
        m_data_processor_thread.quit();
        m_data_processor_thread.wait();
    }

    AppControllerForeign::clear_controller_instance();
}

void AppController::about_menu()
{
    QMessageBox about_box;
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
}

SourceListController *AppController::sourceList()
{
    return &m_sourcelist_controller;
}

MainChartController *AppController::mainChart()
{
    return &m_mainchart_controller;
}

OverviewChartController *AppController::overviewChart()
{
    return &m_overviewchart_controller;
}

ChannelBarModel *AppController::channelModel()
{
    return &m_channelbar_model;
}

TimebaseModel *AppController::timebaseModel()
{
    return &m_timebase_model;
}

VerticalScaleModel *AppController::verticalScaleModel()
{
    return &m_verticalscale_model;
}

QVariantList AppController::channelColors() const
{
    QVariantList color_list;
    color_list.reserve(static_cast<qsizetype>(channel_colors.size()));

    for (const QColor &color : channel_colors) {
        color_list.append(color);
    }

    return color_list;
}

void AppController::handle_start_clicked()
{
    m_current_mode = ScopeMode::Roll;

    emit switch_continuous_mode(true);

    emit start_data_processing();
}

void AppController::handle_stop_clicked()
{
    emit switch_continuous_mode(false);

    emit stop_data_processing();

    m_current_mode = ScopeMode::Stopped;
}

void AppController::channel_selected(int channel_id)
{
    if (channel_id < 0 || channel_id >= this->channels_amount) {
        return;
    }

    auto id = static_cast<ChannelId>(channel_id);

    m_channelbar_model.select_channel(id);
}

void AppController::channel_toggled(int channel_id)
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
        m_channelbar_model.enable_channel(id, m_verticalscale_model.vScaleText(channel_id));

        emit enable_channel(id);
    }
}

void AppController::init_data_processor()
{
    m_data_processor = std::make_unique<DataProcessor>();

    if (!m_data_processor) {
        qFatal() << tr("Failed to initialize Data Processor.");
    }

    m_data_processor->moveToThread(&m_data_processor_thread);

    connect(&m_data_processor_thread, &QThread::started, m_data_processor.get(),
            &DataProcessor::setup);

    // Data Processor commands
    connect(this, &AppController::enable_channel, m_data_processor.get(),
            &DataProcessor::enable_channel);
    connect(this, &AppController::disable_channel, m_data_processor.get(),
            &DataProcessor::disable_channel);
    connect(this, &AppController::update_channel_vertical_scale, m_data_processor.get(),
            &DataProcessor::update_channel_vertical_scale);

    connect(&m_data_processor_thread, &QThread::finished, m_data_processor.get(),
            &DataProcessor::deleteLater);

    connect(this, &AppController::start_data_processing, m_data_processor.get(),
            &DataProcessor::start_data_processing);
    connect(this, &AppController::stop_data_processing, m_data_processor.get(),
            &DataProcessor::stop_data_processing);

    // Register Meta Type which will be used in a communication with Data Processor
    qRegisterMetaType<std::shared_ptr<UniversalReaderDialogConfig>>(
            "std::shared_ptr<UniversalReaderDialogConfig>");

    m_data_processor_thread.start();
}

void AppController::init_ui_elements()
{
    init_source_list();

    init_graph();

    init_input();
}

void AppController::init_graph()
{
    // Main Chart Controller
    connect(this, &AppController::switch_continuous_mode, &m_mainchart_controller,
            &MainChartController::switch_continuous_mode);
    connect(this, &AppController::force_graph_refresh, &m_mainchart_controller,
            &MainChartController::force_graph_refresh);
    connect(m_data_processor.get(), &DataProcessor::send_new_data, &m_mainchart_controller,
            &MainChartController::receive_stored_data);
    connect(&m_mainchart_controller, &MainChartController::request_recent_stored_data,
            m_data_processor.get(), &DataProcessor::handle_recent_data_request);
    connect(&m_mainchart_controller, &MainChartController::request_stored_data,
            m_data_processor.get(), &DataProcessor::handle_data_request);

    // Overview Chart Controller
    connect(this, &AppController::switch_continuous_mode, &m_overviewchart_controller,
            &OverviewChartController::switch_continuous_mode);
    connect(this, &AppController::force_graph_refresh, &m_overviewchart_controller,
            &OverviewChartController::force_graph_refresh);
    connect(m_data_processor.get(), &DataProcessor::send_full_history, &m_overviewchart_controller,
            &OverviewChartController::receive_full_history);
    connect(&m_overviewchart_controller, &OverviewChartController::request_full_history,
            m_data_processor.get(), &DataProcessor::handle_full_history_request);
    connect(&m_overviewchart_controller, &OverviewChartController::selected_time_frame,
            &m_mainchart_controller, &MainChartController::set_time_frame);
}

void AppController::init_input()
{
    // Initialize timebase model (horizontal scaler)
    connect(&m_timebase_model, &TimebaseModel::hDivisionChanged, this, [this]() {
        m_mainchart_controller.set_horizontal_div(
                UData::duration_from_microseconds(m_timebase_model.divisionUs()));
        m_overviewchart_controller.set_horizontal_div(
                UData::duration_from_microseconds(m_timebase_model.divisionUs()));
        m_overviewchart_controller.set_sliding_window_width(
                UData::duration_from_microseconds(m_timebase_model.frameWidthUs()));
    });

    m_mainchart_controller.set_horizontal_div(
            UData::duration_from_microseconds(m_timebase_model.divisionUs()));
    m_overviewchart_controller.set_horizontal_div(
            UData::duration_from_microseconds(m_timebase_model.divisionUs()));
    m_overviewchart_controller.set_sliding_window_width(
            UData::duration_from_microseconds(m_timebase_model.frameWidthUs()));

    // Initialize vertical scaler model
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
}

void AppController::init_source_list()
{
    connect(&m_sourcelist_controller, &SourceListController::configure_reader,
            m_data_processor.get(), &DataProcessor::configure_reader);
    connect(&m_sourcelist_controller, &SourceListController::request_channel_assignment,
            m_data_processor.get(), &DataProcessor::assign_channel);
    connect(&m_sourcelist_controller, &SourceListController::request_channel_assignment, this,
            [this](ReaderId reader_id, VariableId variable_id, ChannelId channel_id) {
                m_verticalscale_model.reset_channel(channel_id);
                m_channelbar_model.connect_channel(channel_id);
                m_channelbar_model.enable_channel(channel_id,
                                                  m_verticalscale_model.vScaleText(channel_id));
            });
    connect(&m_sourcelist_controller, &SourceListController::request_reader_remove,
            m_data_processor.get(), &DataProcessor::remove_reader);
}
