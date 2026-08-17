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
    for (auto &[id, info] : m_senders) {
        if (info.thread != nullptr) {
            if (info.thread->isRunning()) {
                info.thread->quit();

                info.thread->wait();
            }

            std::unique_ptr<QThread>(info.thread);
        }
    }
}

void DataProcessor::setup() { }

void DataProcessor::reported_reader_status(ReaderId reader_id, UniversalReader::Status status)
{
    auto reader_iter = m_senders.find(reader_id);

    if (reader_iter != m_senders.end()) {
        reader_iter->second.latest_status = status;

        qDebug() << tr("Reader [#%1] state has been updated to %2")
                            .arg(reader_id)
                            .arg(QVariant::fromValue(status).toString());
    }
}

void DataProcessor::start_data_processing()
{
    m_buffers.clear();

    for (const auto &[id, reader] : m_senders) {
        if ((reader.latest_status == UniversalReader::Initialized)
            || (reader.latest_status == UniversalReader::Stopped)
            || (reader.latest_status == UniversalReader::Error)) {
            emit reader_start(id);
        }
    }
}
void DataProcessor::stop_data_processing()
{
    for (const auto &[id, reader] : m_senders) {
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

        reader_config->update_period_ms = default_reader_update_period_ms;

        std::unique_ptr<UniversalReader> reader = nullptr;

        if (auto sim_config = std::dynamic_pointer_cast<SimulatedReaderConfig>(reader_config)) {
            reader = std::make_unique<SimulatedReader>(id, sim_config);
        } else if (auto serial_config =
                           std::dynamic_pointer_cast<SerialReaderConfig>(reader_config)) {
            reader = std::make_unique<SerialReader>(id, serial_config);
        }

        if (reader != nullptr) {
            DataSenderInfo new_sender{ new QThread(), reader.release() };

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

            m_senders.emplace(id, std::move(new_sender));

            new_sender.thread->start();
        }
    }
}

void DataProcessor::remove_reader(ReaderId id)
{
    if (m_senders.contains(id)) {
        emit reader_stop(id);

        auto &sender_info = m_senders.at(id);
        sender_info.thread->quit();

        m_senders.erase(id);

        for (auto it = m_var_to_channel.begin(); it != m_var_to_channel.end();) {
            if (it->first.first == id) {
                const ChannelId channel_id = it->second;

                const auto buff_it = m_buffers.find(channel_id);

                if (buff_it != m_buffers.end()) {
                    m_buffers.erase(buff_it);
                }

                m_channel_to_var.erase(it->second);
                it = m_var_to_channel.erase(it);
            } else {
                ++it;
            }
        }
    }
}

void DataProcessor::assign_channel(ReaderId reader_id, VariableId variable_id, ChannelId channel_id)
{
    auto it = m_channel_to_var.find(channel_id);

    if (it != m_channel_to_var.end()) {
        // TODO: send a command to the reader to stop the data transfer

        m_var_to_channel.erase(it->second);
        m_channel_to_var.erase(it);
        m_buffers.erase(channel_id);
    }

    m_var_to_channel[{ reader_id, variable_id }] = channel_id;
    m_channel_to_var[channel_id] = { reader_id, variable_id };
    m_channel_enabled[channel_id] = true;
}

void DataProcessor::enable_channel(ChannelId channel_id)
{
    auto it = m_channel_to_var.find(channel_id);

    if (it != m_channel_to_var.end()) {
        // TODO: send a command to the reader to start sending data

        m_channel_enabled[channel_id] = true;
    }
}

void DataProcessor::disable_channel(ChannelId channel_id)
{
    auto it = m_channel_to_var.find(channel_id);

    if (it != m_channel_to_var.end()) {
        // TODO: send a command to the reader to stop sending data

        m_channel_enabled[channel_id] = false;
        m_buffers[channel_id].clear();
    }
}

void DataProcessor::update_channel_vertical_scale(ChannelId channel_id, double scale)
{
    if (scale > 0) {
        m_channel_vscale[channel_id] = scale;
    }
}

void DataProcessor::receive_data(ReaderId reader_id, UniversalReaderBufferMap data)
{
    if (m_senders.contains(reader_id)) {
        for (auto &&[variable_id, new_data] : data.asKeyValueRange()) {
            if (!new_data || new_data->empty()
                || !m_var_to_channel.contains({ reader_id, variable_id })) {
                continue;
            }

            ChannelId channel_id = m_var_to_channel.at({ reader_id, variable_id });

            if (!m_channel_enabled.contains(channel_id) || !m_channel_enabled.at(channel_id)) {
                continue;
            }

            auto &destination_buffer = m_buffers[channel_id];
            const size_t new_data_size = new_data->size();

            if (new_data_size >= m_max_sample_points) {
                auto start_it = std::prev(new_data->end(),
                                          static_cast<std::ptrdiff_t>(m_max_sample_points));

                destination_buffer.assign(std::make_move_iterator(start_it),
                                          std::make_move_iterator(new_data->end()));
            } else {
                const size_t combined_size = destination_buffer.size() + new_data_size;

                if (combined_size > m_max_sample_points) {
                    const size_t overflow_count = combined_size - m_max_sample_points;

                    destination_buffer.erase(
                            destination_buffer.begin(),
                            std::next(destination_buffer.begin(),
                                      static_cast<std::ptrdiff_t>(overflow_count)));
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

    auto prepared_data = prepare_graph_data(points_limit, start_time, end_time, false);

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

    auto prepared_data =
            prepare_graph_data(points_limit, start_time_actual, end_time_actual, false);

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

    for (const auto &channel_id : std::views::keys(m_channel_to_var)) {
        auto it = m_buffers.find(channel_id);

        if (it != m_buffers.end() && !it->second.empty()
            && (!min_time.has_value() || min_time.value() > it->second.front().first)) {
            min_time = it->second.front().first;
        }
    }

    return min_time;
}

std::optional<UData::Time> DataProcessor::get_latest_stored_time() const
{
    std::optional<UData::Time> max_time = std::nullopt;

    for (const auto &channel_id : std::views::keys(m_channel_to_var)) {
        auto it = m_buffers.find(channel_id);

        if (it != m_buffers.end() && !it->second.empty()
            && (!max_time.has_value() || max_time.value() < it->second.back().first)) {
            max_time = it->second.back().first;
        }
    }

    return max_time;
}

std::optional<std::tuple<QList<GraphData>, UData::Time, UData::Time>>
DataProcessor::prepare_graph_data(int points_limit, std::optional<UData::Time> start_time,
                                  std::optional<UData::Time> end_time, bool strict)
{
    QList<GraphData> new_data;

    UData::Time start_time_actual{ };
    UData::Time end_time_actual{ };

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

    for (const auto &[channel_id, channel_data] : m_buffers) {
        QList<QPointF> processed_values;

        auto scale_it = m_channel_vscale.find(channel_id);
        qreal scale = (scale_it != m_channel_vscale.end()) ? scale_it->second : 1.0;

        auto left_it =
                std::ranges::lower_bound(channel_data, start_time_actual, std::less<>{ }, get_time);
        auto right_it = std::ranges::upper_bound(left_it, channel_data.end(), end_time_actual,
                                                 std::less<>{ }, get_time);

        const size_t points_to_return =
                std::min(static_cast<int>(std::distance(left_it, right_it)), points_limit)
                + (strict ? 0
                          : ((left_it != channel_data.begin() ? 1 : 0)
                             + (right_it != channel_data.end() ? 1 : 0)));

        processed_values.reserve(static_cast<qsizetype>(points_to_return));

        if (!strict && (left_it != channel_data.begin())) {
            const auto &[timestamp, raw_val] = *std::prev(left_it);
            const auto val =
                    std::visit([](auto &&arg) { return static_cast<qreal>(arg); }, raw_val) * scale;
            processed_values.emplace_back(timestamp, val);
        }

        if (std::distance(left_it, right_it) <= points_limit) {
            for (const auto &[timestamp, raw_val] : std::ranges::subrange(left_it, right_it)) {
                const auto val =
                        std::visit([](auto &&arg) { return static_cast<qreal>(arg); }, raw_val)
                        * scale;

                processed_values.emplace_back(timestamp, val);
            }
        } else {
            // Divide the range into equal pieces and provide an average value
            const double time_per_point_us = static_cast<double>(UData::get_timestamp_diff_us(
                                                     start_time_actual, end_time_actual))
                    / points_limit;

            auto next_point = left_it;

            for (int i = 0; (i < points_limit) && (next_point != right_it); i++) {
                const UData::Time piece_end = UData::timestamp_add_us_roundup(
                        start_time_actual, static_cast<int64_t>(time_per_point_us * (i + 1)));

                UData::Time min_time = next_point->first;
                UData::Time max_time = min_time;
                int amount = 0;
                qreal sum = 0;

                while ((next_point != right_it) && (next_point->first < piece_end)) {
                    const auto val = std::visit([](auto &&arg) { return static_cast<qreal>(arg); },
                                                next_point->second);
                    sum += val;
                    max_time = next_point->first;
                    amount++;

                    next_point++;
                }

                if (amount > 0) {
                    const UData::Time average_time = min_time + (max_time - min_time) / 2;
                    const qreal average_value = sum / amount * scale;

                    processed_values.emplace_back(average_time, average_value);
                }
            }
        }

        if (!strict && (right_it != channel_data.end())) {
            const auto &[timestamp, raw_val] = *right_it;
            const auto val =
                    std::visit([](auto &&arg) { return static_cast<qreal>(arg); }, raw_val) * scale;
            processed_values.emplace_back(timestamp, val);
        }

        new_data.emplace_back(channel_id, std::move(processed_values));
    }

    return std::tuple{ new_data, start_time_actual, end_time_actual };
}
