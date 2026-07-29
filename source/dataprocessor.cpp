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
#include <vector>

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
    m_buffers.clear();

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
    if (points_limit < 1) {
        return;
    }

    auto prepared_data = prepare_graph_data(points_limit, start_time, end_time);

    if (prepared_data.has_value()) {
        auto &&new_data = std::get<0>(std::move(prepared_data.value()));

        emit send_new_data(std::move(new_data), start_time, end_time);
    } else {
        emit send_new_data(QList<GraphData>(), start_time, end_time);
    }
}

void DataProcessor::handle_recent_data_request(UData::Time end_time, int64_t data_window_us,
                                               int64_t max_drift_us, int points_limit)
{
    if (points_limit < 1) {
        return;
    }

    UData::Time end_time_actual = get_latest_stored_time().value_or(end_time);

    if (UData::get_timestamp_diff_us(end_time_actual, end_time) > max_drift_us) {
        // Latest received value can not be on the right border
        end_time_actual = UData::timestamp_sub_us_rounddown(end_time, max_drift_us);
    } else if (UData::get_timestamp_diff_us(end_time_actual, end_time) < 0) {
        // Latest received value was received after end_time
        end_time_actual = end_time;
    }

    UData::Time start_time_actual =
            UData::timestamp_sub_us_rounddown(end_time_actual, data_window_us);

    auto prepared_data = prepare_graph_data(points_limit, start_time_actual, end_time_actual);

    if (prepared_data.has_value()) {
        auto &&new_data = std::get<0>(std::move(prepared_data.value()));

        emit send_new_data(std::move(new_data), start_time_actual, end_time_actual);
    } else {
        emit send_new_data(QList<GraphData>(), start_time_actual, end_time_actual);
    }
}

void DataProcessor::handle_full_history_request(int points_limit)
{
    if (points_limit < 1) {
        return;
    }

    auto prepared_data = prepare_graph_data(points_limit);

    if (prepared_data.has_value()) {
        auto &&[new_data, start_time, end_time] = std::move(prepared_data.value());

        emit send_full_history(std::move(new_data), start_time, end_time);
    } else {
        emit send_full_history(QList<GraphData>(), -1, -1);
    }
}

std::optional<UData::Time> DataProcessor::get_earliest_stored_time() const
{
    std::optional<UData::Time> min_time = std::nullopt;

    for (const auto &channel_input : m_channel_to_var) {
        const auto channel_buffer =
                m_buffers.value(channel_input.first).value(channel_input.second);

        if (!channel_buffer.empty()
            && (!min_time.has_value() || min_time.value() > channel_buffer.front().first)) {
            min_time = channel_buffer.front().first;
        }
    }

    return min_time;
}

std::optional<UData::Time> DataProcessor::get_latest_stored_time() const
{
    std::optional<UData::Time> max_time = std::nullopt;

    for (const auto &channel_input : m_channel_to_var) {
        const auto channel_buffer =
                m_buffers.value(channel_input.first).value(channel_input.second);

        if (!channel_buffer.empty()
            && (!max_time.has_value() || max_time.value() < channel_buffer.back().first)) {
            max_time = channel_buffer.back().first;
        }
    }

    return max_time;
}

std::optional<std::tuple<QList<GraphData>, UData::Time, UData::Time>>
DataProcessor::prepare_graph_data(int points_limit, std::optional<UData::Time> start_time,
                                  std::optional<UData::Time> end_time)
{
    QList<GraphData> new_data;

    UData::Time start_time_actual;
    UData::Time end_time_actual;

    if (start_time.has_value()) {
        start_time_actual = start_time.value();
    } else {
        std::optional<UData::Time> min_time = get_earliest_stored_time();

        if (min_time.has_value()) {
            start_time_actual = min_time.value();
        } else {
            return std::nullopt;
        }
    }

    if (end_time.has_value()) {
        end_time_actual = end_time.value();
    } else {
        std::optional<UData::Time> max_time = get_latest_stored_time();

        if (max_time.has_value()) {
            end_time_actual = max_time.value();
        } else {
            return std::nullopt;
        }
    }

    if (end_time_actual <= start_time_actual) {
        return std::nullopt;
    }

    auto get_time = [](const UData::Point &p) { return p.first; };

    for (auto &&[reader_id, reader_data] : m_buffers.asKeyValueRange()) {
        for (auto &&[variable_id, var_data] : reader_data.asKeyValueRange()) {
            auto it = m_var_to_channel.constFind(qMakePair(reader_id, variable_id));

            if (it != m_var_to_channel.constEnd()) {
                QList<QPointF> processed_values;

                auto left_it = std::ranges::lower_bound(var_data, start_time_actual, std::less<>{ },
                                                        get_time);
                auto right_it = std::ranges::upper_bound(left_it, var_data.end(), end_time_actual,
                                                         std::less<>{ }, get_time);

                if (std::distance(left_it, right_it) <= points_limit) {
                    processed_values.reserve(std::distance(left_it, right_it));

                    for (const auto &[timestamp, raw_val] :
                         std::ranges::subrange(left_it, right_it)) {
                        const auto val = std::visit(
                                [](auto &&arg) { return static_cast<qreal>(arg); }, raw_val);
                        processed_values.emplace_back(timestamp, val);
                    }
                } else {
                    processed_values.reserve(points_limit);

                    // Divide the range into equal pieces and provide an average value
                    const double time_per_point_us =
                            static_cast<double>(UData::get_timestamp_diff_us(start_time_actual,
                                                                             end_time_actual))
                            / points_limit;

                    auto next_point = left_it;

                    for (int i = 0; (i < points_limit) && (next_point != right_it); i++) {
                        const UData::Time piece_end = UData::timestamp_add_us_roundup(
                                start_time_actual,
                                static_cast<int64_t>(time_per_point_us * (i + 1)));

                        UData::Time min_time = next_point->first;
                        UData::Time max_time = min_time;
                        int amount = 0;
                        qreal sum = 0;

                        while ((next_point->first < piece_end) && (next_point != right_it)) {
                            const auto val =
                                    std::visit([](auto &&arg) { return static_cast<qreal>(arg); },
                                               next_point->second);
                            sum += val;
                            max_time = next_point->first;
                            amount++;

                            next_point++;
                        }

                        if (amount > 0) {
                            const UData::Time average_time = min_time + (max_time - min_time) / 2;
                            const qreal average_value = sum / amount;

                            processed_values.emplace_back(average_time, average_value);
                        }
                    }
                }

                new_data.emplace_back(it.value(), std::move(processed_values));
            }
        }
    }

    return std::tuple{ new_data, start_time_actual, end_time_actual };
}
