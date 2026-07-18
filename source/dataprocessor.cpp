#include "dataprocessor.h"

#include "serialreader.h"
#include "simulatedreader.h"
#include "universalreader.h"

#include <QDebug>
#include <QMutexLocker>
#include <QThread>
#include <QVariant>
#include <algorithm>
#include <cmath>
#include <functional>
#include <iterator>
#include <ranges>

DataProcessor::DataProcessor(QObject *parent)
    : QObject{ parent }
    , m_max_sample_points(default_max_sample_points)

{
    qRegisterMetaType<UniversalReaderBufferMap>("UniversalReaderBufferMap");
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

void DataProcessor::setup(void) { }

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
        } else if (auto serial_config =
                           std::dynamic_pointer_cast<SerialReaderConfig>(reader_config)) {
            reader = new SerialReader(id, serial_config);
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

void DataProcessor::receive_data(ReaderId reader_id, UniversalReaderBufferMap data)
{
    if (m_senders.contains(reader_id)) {
        for (auto &&[variable_id, new_data] : data.asKeyValueRange()) {
            if (!new_data || new_data->empty()
                || !m_var_to_channel.contains(qMakePair(reader_id, variable_id))) {
                continue;
            }

            auto &destination_buffer = m_buffers[reader_id][variable_id];
            const size_t new_data_size = new_data->size();

            if (new_data_size >= m_max_sample_points) {
                auto start_it = new_data->end() - m_max_sample_points;

                destination_buffer.assign(std::make_move_iterator(start_it),
                                          std::make_move_iterator(new_data->end()));
            } else {
                const size_t combined_size = destination_buffer.size() + new_data_size;

                if (combined_size > m_max_sample_points) {
                    const size_t overflow_count = combined_size - m_max_sample_points;

                    destination_buffer.erase(destination_buffer.begin(),
                                             destination_buffer.begin() + overflow_count);
                }

                destination_buffer.insert(destination_buffer.end(),
                                          std::make_move_iterator(new_data->begin()),
                                          std::make_move_iterator(new_data->end()));
            }
        }

        QMetaObject::invokeMethod(m_senders[reader_id].sender, "release_buffer",
                                  Qt::QueuedConnection, Q_ARG(UniversalReaderBufferMap, data));
    }
}

void DataProcessor::handle_data_request(UData::Time start_time, UData::Time end_time,
                                        int points_limit)
{
    QList<GraphData> new_data;

    auto get_time = [](const UData::Point &p) { return p.first; };

    for (auto &&[reader_id, reader_data] : m_buffers.asKeyValueRange()) {
        for (auto &&[variable_id, var_data] : reader_data.asKeyValueRange()) {
            auto it = m_var_to_channel.constFind(qMakePair(reader_id, variable_id));

            if (it != m_var_to_channel.constEnd()) {
                QList<QPointF> processed_values;

                auto left_it =
                        std::ranges::lower_bound(var_data, start_time, std::less<>{ }, get_time);
                auto right_it = std::ranges::upper_bound(left_it, var_data.end(), end_time,
                                                         std::less<>{ }, get_time);

                for (const auto &[timestamp, raw_val] : std::ranges::subrange(left_it, right_it)) {

                    const auto val =
                            std::visit([](auto &&arg) { return static_cast<qreal>(arg); }, raw_val);
                    processed_values.emplace_back(timestamp, val);
                }

                new_data.emplace_back(it.value(), std::move(processed_values));
            } else {
                var_data.clear();
            }
        }
    }

    emit send_new_data(new_data, start_time, end_time);
}
