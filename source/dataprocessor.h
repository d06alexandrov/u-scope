#pragma once

#include "commontypes.hpp"
#include "universalreader.h"

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
#include <QVector>

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
     * @brief Copy constructor for GraphData.
     *
     * @param other The source object to copy from.
     */
    GraphData(const GraphData &other)
        : m_id(other.m_id)
        , m_values(other.m_values)
    {
    }

    /**
     * @brief Move constructor for GraphData.
     *
     * @param other The rvalue reference of the object to move from.
     */
    GraphData(GraphData &&other) noexcept
        : m_id(other.m_id)
        , m_values(std::move(other.m_values))
    {
    }

    /**
     * @brief Move assignment operator for GraphData.
     *
     * @param other The rvalue reference of the object to move.
     * @return A reference to this object.
     */
    GraphData &operator=(GraphData &&other) noexcept
    {
        if (this != &other) {
            m_id = other.m_id;
            m_values = std::move(other.m_values);
        }
        return *this;
    }

    /**
     * @brief Gets the channel id.
     *
     * @return A reference to the name.
     */
    const ChannelId get_id() const { return m_id; }

    /**
     * @brief Gets the list of values.
     *
     * @return A reference to the values.
     */
    const QList<QPointF> &get_values() const { return m_values; }

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
    ~DataProcessor();

    /**
     * @brief Add varibles data into incoming buffer.
     *
     * @note Thread-safe if the senders' info is updated when all senders are stopped.
     *
     * @param reader_id ID of the sender.
     * @param data Data to be stored.
     */
    void add_variables_data(ReaderId reader_id, QMap<VariableId, QList<DataPoint>> &data);

    /**
     * @brief Get current timestamp.
     */
    static DataTime get_timestamp();

    /**
     * @brief Get the difference between two timestamps.
     *
     * @param before Earlier timestamp.
     * @param after Timestamp after @p before.
     * @return Time in microseconds between @p before and @p after.
     */
    static uint64_t get_timestamp_diff_us(DataTime before, DataTime after);

    /**
     * @brief Increase timestamp by provided time.
     *
     * @param timestamp Original timestamp.
     * @param us Time in microseconds that should be added to original timestamp.
     * @return @p timestamp plus @p us round up to the nearest DataTime.
     */
    static DataTime timestamp_add_us_roundup(DataTime timestamp, uint64_t us);

public slots:
    /**
     * @brief Initialize DataProcessor.
     */
    void setup(void);

    /**
     * @brief Process incoming data and send clean data to the chart.
     * Triggered by timer.
     */
    void process(void);

    /**
     * @brief Handle reader status report.
     *
     * @param reader_id Unique ID of the reader.
     * @param status Status of the reader.
     */
    void reported_reader_status(ReaderId reader_id, UniversalReader::Status status);

    /* TODO: remove connection to the button. */
    void start_data_processing(); /**< Slot connected to the start button. */
    void stop_data_processing(); /**< Slot connected to the stop button. */

    /**
     * @brief Configure reader
     *
     * If the reader exists, it changes the config of it.
     *
     * @param id reader id
     * @param config configuration of the reader
     */
    void configure_reader(ReaderId id, std::shared_ptr<UniversalReaderConfig> config);

    /**
     * @brief Remove reader
     *
     * @param id reader id
     */
    void remove_reader(ReaderId id);

    /**
     * @brief Bind particular variable to the channel
     *
     * @param variable uniq identificator of a variable
     * @param channel_id id of the channel
     */
    void assign_channel(QPair<ReaderId, VariableId> variable, ChannelId channel_id);

signals:
    /**
     * @brief Send new data to show in a chart.
     *
     * @param new_data Data to show in a chart.
     */
    void send_new_data(const QList<GraphData> &new_data);

    /**
     * @brief Report finish of the Data Processor.
     */
    void finished(void);

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
    /**
     * @brief Structure to store information regarding Senders (Readers)
     */
    struct DataSenderInfo
    {
        QThread *thread = nullptr; /**< Pointer to the sender's thread. */
        UniversalReader *sender = nullptr; /**< Pointer to the sender. */
        std::shared_ptr<QMutex> buffer_mutex =
                nullptr; /**< Mutex that protects data from this particular sender. */

        UniversalReader::Status latest_status =
                UniversalReader::Uninitialized; /**< Latest known status of the sender. */
    };

    QTimer *m_timer =
            nullptr; /**< Timer to execute processing function and send data to graph Widget. */
    QHash<ReaderId, DataSenderInfo>
            m_senders; /**< Initialized data senders. Sender is used as a key. */
    QMap<ReaderId, QMap<VariableId, QList<DataPoint>>>
            m_in_buffers; /**< Buffers where senders store the data. Sender Mutex is used for a
                             thread-safety. */
    QMap<ReaderId, QMap<VariableId, QVector<DataPoint>>>
            m_buffers; /**< Buffers to store raw data from senders. */
    QHash<QPair<ReaderId, VariableId>, ChannelId>
            m_var_to_channel; /**< Correspondence between variables and channels. */
    QHash<ChannelId, QPair<ReaderId, VariableId>> m_channel_to_var; /**< Correspondence between
                                                                     channels and variables. */
};
