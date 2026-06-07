#include "dataprocessor.h"

#include "serialreader.h"
#include "simulatedreader.h"
#include "universalreader.h"

#include <QDebug>
#include <QMutexLocker>
#include <QThread>
#include <QVariant>
#include <chrono>
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
        if ((x.thread != nullptr) && (x.thread->isRunning())) {
            x.thread->quit();

            if (!x.thread->wait(1000)) {
                x.thread->terminate();
                x.thread->wait();
            }
        }
    }
}

void DataProcessor::add_variables_data(ReaderId reader_id, QMap<VariableId, QList<DataPoint>> &data)
{
    if (m_senders.contains(reader_id)) {
        auto &sender_info = m_senders[reader_id];
        if (sender_info.buffer_mutex) {
            QMutexLocker locker(sender_info.buffer_mutex.get());

            for (auto [variable_id, new_data] : data.asKeyValueRange()) {
                m_in_buffers[reader_id][variable_id].append(std::move(new_data));
            }
        }
    } else {
        data.clear();
    }
}

DataTime DataProcessor::get_timestamp()
{
    return std::chrono::duration_cast<std::chrono::microseconds>(
                   std::chrono::steady_clock::now().time_since_epoch())
            .count();
}

uint64_t DataProcessor::get_timestamp_diff_us(DataTime before, DataTime after)
{
    return after - before;
}

DataTime DataProcessor::timestamp_add_us_roundup(DataTime timestamp, uint64_t us)
{
    return timestamp + us;
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

    for (auto [reader_id, sender_info] : m_senders.asKeyValueRange()) {
        QMap<VariableId, QList<DataPoint>> in_data;

        if (sender_info.buffer_mutex) {
            QMutexLocker locker(sender_info.buffer_mutex.get());
            std::swap(in_data, m_in_buffers[reader_id]);
        }

        DataTime current_time = get_timestamp();

        for (auto [variable_id, in_var_data] : in_data.asKeyValueRange()) {
            if (auto it = m_var_to_channel.constFind(qMakePair(reader_id, variable_id));
                it != m_var_to_channel.constEnd()) {
                QList<QPointF> processed_values;

                m_buffers[reader_id][variable_id].append(std::move(in_var_data));

                auto cut_it = std::lower_bound(
                        m_buffers[reader_id][variable_id].begin(),
                        m_buffers[reader_id][variable_id].end(), m_time_width,
                        [current_time](const DataPoint &point, uint64_t max_distance) {
                            return get_timestamp_diff_us(point.first, current_time) > max_distance;
                        });

                m_buffers[reader_id][variable_id].erase(m_buffers[reader_id][variable_id].begin(),
                                                        cut_it);

                for (auto &val_it : m_buffers[reader_id][variable_id]) {
                    const auto val = std::visit([](auto &&arg) { return static_cast<qreal>(arg); },
                                                val_it.second);

                    const qreal x_coord = static_cast<qreal>(m_left_bottom_corner.x())
                            + (m_time_width - get_timestamp_diff_us(val_it.first, current_time))
                                    / static_cast<qreal>(m_time_width)
                                    * static_cast<qreal>(m_right_top_corner.x()
                                                         - m_left_bottom_corner.x());
                    processed_values.emplace_back(x_coord, val);
                }

                new_data.emplace_back(it.value(), std::move(processed_values));
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

void DataProcessor::configure_reader(ReaderId id, std::shared_ptr<UniversalReaderConfig> config)
{
    if (m_senders.contains(id)) {
        // modify reader
    } else {
        auto config_copy = config->clone();

        config_copy->update_period_ms = 20;

        UniversalReader *reader = nullptr;

        if (std::dynamic_pointer_cast<SimulatedReaderConfig>(config)) {
            reader = new SimulatedReader(
                    id, this, std::dynamic_pointer_cast<SimulatedReaderConfig>(config_copy));
        }

        if (reader != nullptr) {
            DataSenderInfo new_sender{ new QThread(), reader, std::make_shared<QMutex>() };

            new_sender.sender->moveToThread(new_sender.thread);

            connect(new_sender.thread, &QThread::started, new_sender.sender,
                    &UniversalReader::reader_setup);
            connect(new_sender.thread, &QThread::finished, new_sender.sender,
                    &QObject::deleteLater);
            connect(new_sender.thread, &QThread::finished, new_sender.thread,
                    &QObject::deleteLater);

            connect(new_sender.sender, &UniversalReader::report_status, this,
                    &DataProcessor::reported_reader_status);
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
        m_in_buffers.remove(id);
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

void DataProcessor::set_time_width(uint64_t us)
{
    if (us > 0) {
        m_time_width = us;
    }
}
