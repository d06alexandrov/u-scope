#pragma once

#include "commontypes.hpp"
#include "universalreader.hpp"
#include "universalreader_dialog.hpp"

#include <QHash>
#include <QList>
#include <QMap>
#include <QMutex>
#include <QObject>
#include <QPair>
#include <QPointF>
#include <QString>
#include <QThread>
#include <QTimer>
#include <deque>
#include <map>
#include <optional>
#include <unordered_map>

/**
 * @brief Class to store data of one particular graph.
 */
class GraphData
{
public:
    /**
     * @brief GraphData constructor.
     *
     * @param id Channel ID of the data.
     * @param values Points of the graph.
     */
    GraphData(ChannelId id, QList<QPointF> values)
        : m_id(id)
        , m_values(std::move(values))
    {
    }

    /**
     * @brief Gets the channel id.
     *
     * @return The channel id of the data.
     */
    [[nodiscard]] const ChannelId get_id() const { return m_id; }

    /**
     * @brief Gets the list of values.
     *
     * @return A reference to the values.
     */
    [[nodiscard]] const QList<QPointF> &get_values() const { return m_values; }

private:
    ChannelId m_id; /**< Channel ID of the data. */
    QList<QPointF> m_values; /**< Points of the graph. */
};

class UniversalReader;

/**
 * @brief Class that orchestrates data readers and convert data into graph points.
 */
class DataProcessor : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Data Processor constructor.
     *
     * @param parent QObject parent of the DataProcessor.
     */
    explicit DataProcessor(QObject *parent = nullptr);

    DataProcessor(const DataProcessor &other) = delete;
    DataProcessor(DataProcessor &&other) = delete;

    /**
     * @brief Data Processor destructor.
     */
    ~DataProcessor() override;

    DataProcessor &operator=(const DataProcessor &other) = delete;
    DataProcessor &operator=(DataProcessor &&other) = delete;

public slots:
    /**
     * @brief Initialize DataProcessor.
     */
    void setup();

    /**
     * @brief Handle reader status report.
     *
     * @param reader_id Unique ID of the reader.
     * @param status Status of the reader.
     */
    void reported_reader_status(ReaderId reader_id, UniversalReader::Status status);

    /**
     * @brief Start data processing in the Data Processor.
     */
    void start_data_processing();

    /**
     * @brief Stop data processing in the Data Processor.
     */
    void stop_data_processing();

    /**
     * @brief Configure reader
     *
     * If the reader exists, it changes the config of it.
     *
     * @param id ID of the reader.
     * @param config New configuration of the reader.
     */
    void configure_reader(ReaderId id, std::shared_ptr<UniversalReaderDialogConfig> config);

    /**
     * @brief Remove reader.
     *
     * @param id ID of the reader to be removed.
     */
    void remove_reader(ReaderId id);

    /**
     * @brief Bind particular variable to the channel.
     *
     * @param reader_id ID of the reader.
     * @param variable_id ID of the variable.
     * @param channel_id ID of the channel to be assigned to the variable.
     */
    void assign_channel(ReaderId reader_id, VariableId variable_id, ChannelId channel_id);

    /**
     * @brief Enable specific channel to receive data from the reader.
     *
     * @param channel_id ID of the channel to be enabled.
     */
    void enable_channel(ChannelId channel_id);

    /**
     * @brief Disable specific channel to stop receiving data from the reader.
     *
     * @param channel_id ID of the channel to be disabled.
     */
    void disable_channel(ChannelId channel_id);

    /**
     * @brief Update vertical scale of the specific channel.
     *
     * @param channel_id ID of the channel to update vertical scale.
     * @param scale New vertical scale value.
     */
    void update_channel_vertical_scale(ChannelId channel_id, double scale);

    /**
     * @brief Receive data from the reader and store it in the buffer.
     *
     * @param reader_id ID of the reader.
     * @param data Data from the reader to be stored in the buffer.
     */
    void receive_data(ReaderId reader_id, UniversalReaderBufferMap data);

    /**
     * @brief Handle data request to display stored data.
     *
     * @param start_time Start time of the requested data.
     * @param end_time End time of the requested data.
     * @param points_limit Maximum number of points to return.
     */
    void handle_data_request(UData::Time start_time, UData::Time end_time, int points_limit);

    /**
     * @brief Handle request for recent data.
     *
     * @param end_time Expected end time of the requested data.
     * @param data_window Time window for the requested data.
     * @param max_drift Maximum allowed drift for the requested data.
     * @param points_limit Maximum number of points to return for the recent data.
     */
    void handle_recent_data_request(UData::Time end_time, UData::Time::Duration data_window,
                                    UData::Time::Duration max_drift, int points_limit);

    /**
     * @brief Handle request for full history of data.
     *
     * @param points_limit Maximum number of points to return for the full history.
     */
    void handle_full_history_request(int points_limit);

signals:
    /**
     * @brief Send new data to show in a chart.
     *
     * @param new_data Data to show in a chart.
     * @param requested_start_time Start time of the requested data.
     * @param requested_end_time End time of the requested data.
     */
    void send_new_data(const QList<GraphData> &new_data, UData::Time requested_start_time,
                       UData::Time requested_end_time);

    /**
     * @brief Send full history data to show in a chart.
     *
     * @param new_data Data to show in a chart.
     * @param start_time Start time of the data.
     * @param end_time End time of the data.
     */
    void send_full_history(const QList<GraphData> &new_data, UData::Time start_time,
                           UData::Time end_time);

    /**
     * @brief Report finish of the Data Processor.
     */
    void finished();

    /**
     * @brief Start specific reader.
     *
     * @param reader_id Unique ID of the reader.
     */
    void reader_start(ReaderId reader_id);

    /**
     * @brief Stop specific reader.
     *
     * @param reader_id Unique ID of the reader.
     */
    void reader_stop(ReaderId reader_id);

private:
    static constexpr size_t default_max_sample_points =
            10000000; /**< Default amount of sample points. */
    static constexpr std::chrono::milliseconds default_reader_update_period{
        20
    }; /** Default update period of readers. */

    /**
     * @brief Structure to store information regarding Senders (Readers)
     */
    struct DataSenderInfo
    {
        std::unique_ptr<QThread> thread = nullptr; /**< Pointer to the sender's thread. */
        UniversalReader *sender = nullptr; /**< Pointer to the sender. */

        UniversalReader::Status latest_status =
                UniversalReader::Uninitialized; /**< Latest known status of the sender. */
    };

    /**
     * @brief Hasher for std::pair<ReaderId, VariableId> to be used in unordered_map.
     */
    struct ReaderVariableHash
    {
        static constexpr int hash_bit_shift =
                32; /**< Bit shift value used to combine ReaderId and VariableId. */
        /**
         * @brief Calculate the hash value for a pair of ReaderId and VariableId.
         *
         * @param p The pair of ReaderId and VariableId to hash.
         * @return The calculated hash value.
         */
        size_t operator()(const std::pair<ReaderId, VariableId> &p) const noexcept
        {
            uint64_t combined = (static_cast<uint64_t>(p.first) << hash_bit_shift)
                    ^ static_cast<uint64_t>(p.second);

            return static_cast<size_t>(combined);
        }
    };

    QTimer *m_timer =
            nullptr; /**< Timer to execute processing function and send data to graph Widget. */
    std::unordered_map<ReaderId, DataSenderInfo>
            m_senders{ }; /**< Initialized data senders. Sender is used as a key. */
    std::map<ChannelId, std::deque<UData::Point>>
            m_buffers{ }; /**< Buffers to store raw data from senders. */
    std::unordered_map<std::pair<ReaderId, VariableId>, ChannelId, ReaderVariableHash>
            m_var_to_channel{ }; /**< Correspondence between variables and channels. */
    std::unordered_map<ChannelId, std::pair<ReaderId, VariableId>>
            m_channel_to_var{ }; /**< Correspondence between
                   channels and variables. */
    std::unordered_map<ChannelId, bool> m_channel_enabled{ }; /**< If channels are enabled. */
    std::unordered_map<ChannelId, double> m_channel_vscale{ }; /**< Vertical channel scale. */

    size_t m_max_sample_points; /**< Maximum amount of sample points per channel. */

    /**
     * @brief Calculate the earilest time in the stored buffers.
     *
     * @return Optional earliest time if buffers are not empty, otherwise std::nullopt.
     */
    std::optional<UData::Time> get_earliest_stored_time() const;

    /**
     * @brief Calculate the latest time in the stored buffers.
     *
     * @return Optional latest time if buffers are not empty, otherwise std::nullopt.
     */
    std::optional<UData::Time> get_latest_stored_time() const;

    /**
     * @brief Prepare graph data from the stored buffers.
     *
     * Prepare graph data for the range between start and end time, if specified. Otherwise the
     * whole history will be returned. Parameter @par strict controls if only elements from the
     * provided range are returned, or additional elements outside the range could be provided.
     *
     * @param points_limit Maximum number of points to return.
     * @param start_time Optional start time of the requested data.
     * @param end_time Optional end time of the requested data.
     * @param strict Flag indicating whether to strictly enforce the time range.
     * @return Optional tuple containing a list of GraphData, start time, and end time.
     */
    std::optional<std::tuple<QList<GraphData>, UData::Time, UData::Time>>
    prepare_graph_data(int points_limit, std::optional<UData::Time> start_time = std::nullopt,
                       std::optional<UData::Time> end_time = std::nullopt, bool strict = true);
};
