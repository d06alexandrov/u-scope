#include "dataprocessor.h"

#include "serialreader.h"
#include "simulatedreader.h"
#include "universalreader.h"

#include <QDebug>
#include <QMutexLocker>
#include <QThread>
#include <QVariant>
#include <cmath>

DataProcessor::DataProcessor(QPoint left_bottom_corner, QPoint right_top_corner, QObject *parent)
    : QObject{ parent }
    , m_time_width(default_time_width)
    , m_left_bottom_corner(left_bottom_corner)
    , m_right_top_corner(right_top_corner)
{
}

DataProcessor::~DataProcessor()
{
    for (auto &x : m_senders) {
        if (x.thread != nullptr) {
            if (x.thread->isRunning()) {
                x.thread->quit();

                if (!x.thread->wait(1000)) {
                    x.thread->terminate();
                    x.thread->wait();
                }
            }

            delete x.thread;
        }
    }
}

void DataProcessor::setup(void)
{
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &DataProcessor::process);
    m_timer->start(30);
}

void DataProcessor::process(void)
{
    QList<GraphData> new_data;

    UData::Time current_time = UData::get_timestamp();

    for (auto &&[reader_id, reader_data] : m_buffers.asKeyValueRange()) {
        for (auto &&[variable_id, var_data] : reader_data.asKeyValueRange()) {
            auto it = m_var_to_channel.constFind(qMakePair(reader_id, variable_id));

            if (it != m_var_to_channel.constEnd()) {
                QList<QPointF> processed_values;

                auto cut_it = std::lower_bound(
                        var_data.begin(), var_data.end(), m_time_width,
                        [current_time](const UData::Point &point, int64_t max_distance) {
                            return UData::get_timestamp_diff_us(point.first, current_time)
                                    > max_distance;
                        });

                var_data.erase(var_data.begin(), cut_it);

                for (auto &val_it : var_data) {
                    const auto val = std::visit([](auto &&arg) { return static_cast<qreal>(arg); },
                                                val_it.second);

                    const qreal x_coord = static_cast<qreal>(m_left_bottom_corner.x())
                            + (m_time_width
                               - UData::get_timestamp_diff_us(val_it.first, current_time))
                                    / static_cast<qreal>(m_time_width)
                                    * static_cast<qreal>(m_right_top_corner.x()
                                                         - m_left_bottom_corner.x());
                    processed_values.emplace_back(x_coord, val);
                }

                new_data.emplace_back(it.value(), std::move(processed_values));
            } else {
                var_data.clear();
            }
        }
    }

    emit send_new_data(new_data);
}

void DataProcessor::reported_reader_status(ReaderId reader_id, UniversalReader::Status status)
{
    auto reader_iter = m_senders.find(reader_id);

    if (reader_iter != m_senders.end()) {
        reader_iter->latest_status = status;

        qDebug() << tr("Reader [#%1] state has been updated to %2")
                            .arg(reader_id)
                            .arg(QVariant::fromValue(status).toString());
    }
}

void DataProcessor::start_data_processing()
{
    for (const auto [id, reader] : m_senders.asKeyValueRange()) {
        if ((reader.latest_status == UniversalReader::Initialized)
            || (reader.latest_status == UniversalReader::Stopped)
            || (reader.latest_status == UniversalReader::Error)) {
            emit reader_start(id);
        }
    }
}
void DataProcessor::stop_data_processing()
{
    for (const auto [id, reader] : m_senders.asKeyValueRange()) {
        if ((reader.latest_status == UniversalReader::Running)
            || (reader.latest_status == UniversalReader::Error)) {
            emit reader_stop(id);
        }
    }
}

void DataProcessor::configure_reader(ReaderId id,
                                     std::shared_ptr<UniversalReaderDialogConfig> config)
{
    if (m_senders.contains(id)) {
        // modify reader
    } else {
        auto reader_config = config->to_reader_config();

        reader_config->update_period_ms = 20;

        UniversalReader *reader = nullptr;

        if (auto sim_config = std::dynamic_pointer_cast<SimulatedReaderConfig>(reader_config)) {
            reader = new SimulatedReader(id, sim_config);
        }

        if (reader != nullptr) {
            DataSenderInfo new_sender{ new QThread(), reader };

            new_sender.sender->moveToThread(new_sender.thread);

            connect(new_sender.thread, &QThread::started, new_sender.sender,
                    &UniversalReader::reader_setup);
            connect(new_sender.thread, &QThread::finished, new_sender.sender,
                    &QObject::deleteLater);

            connect(new_sender.sender, &UniversalReader::report_status, this,
                    &DataProcessor::reported_reader_status);
            connect(new_sender.sender, &UniversalReader::data_ready, this,
                    &DataProcessor::receive_data);
            connect(this, &DataProcessor::reader_start, new_sender.sender,
                    &UniversalReader::reader_start);
            connect(this, &DataProcessor::reader_stop, new_sender.sender,
                    &UniversalReader::reader_stop);

            m_senders.insert(id, std::move(new_sender));

            new_sender.thread->start();
        }
    }
}

void DataProcessor::remove_reader(ReaderId id)
{
    if (m_senders.contains(id)) {
        emit reader_stop(id);

        auto sender_info = m_senders.take(id);
        sender_info.thread->quit();

        m_senders.remove(id);
        m_buffers.remove(id);

        for (auto it = m_var_to_channel.begin(); it != m_var_to_channel.end();) {
            if (it.key().first == id) {
                m_channel_to_var.remove(it.value());
                it = m_var_to_channel.erase(it);
            } else {
                ++it;
            }
        }
    }
}

void DataProcessor::assign_channel(QPair<ReaderId, VariableId> variable, ChannelId channel_id)
{
    if (auto it = m_channel_to_var.constFind(channel_id); it != m_channel_to_var.constEnd()) {
        // TODO: send a command to the reader to stop the data transfer

        m_var_to_channel.remove(it.value());
        m_channel_to_var.remove(channel_id);
    }

    m_var_to_channel[variable] = channel_id;
    m_channel_to_var[channel_id] = variable;
}

void DataProcessor::set_time_width(int64_t us)
{
    if (us > 0) {
        m_time_width = us;
    }
}

void DataProcessor::receive_data(ReaderId reader_id, UniversalReaderBufferMap data)
{
    if (m_senders.contains(reader_id)) {
        for (auto &&[variable_id, new_data] : data.asKeyValueRange()) {
            if (m_var_to_channel.contains(qMakePair(reader_id, variable_id))) {
                m_buffers[reader_id][variable_id].append(
                        QList<UData::Point>(new_data->begin(), new_data->end()));
            }
        }

        QMetaObject::invokeMethod(m_senders[reader_id].sender, "release_buffer",
                                  Qt::QueuedConnection, Q_ARG(UniversalReaderBufferMap, data));
    }
}
