#pragma once

#include "commontypes.hpp"
#include "universalreader.h"

#include <QHash>
#include <QList>
#include <QMap>
#include <QMutex>
#include <QObject>
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
    GraphData(QString name, QList<QPointF> values)
        : name(std::move(name))
        , values(std::move(values))
    {
    }

    GraphData(const GraphData &other)
        : name(other.name)
        , values(other.values)
    {
    }

    GraphData(GraphData &&other) noexcept
        : name(std::move(other.name))
        , values(std::move(other.values))
    {
    }

    GraphData &operator=(GraphData &&other) noexcept
    {
        if (this != &other) {
            name = std::move(other.name);
            values = std::move(other.values);
        }
        return *this;
    }

    const QString &get_name(void) const { return name; }

    const QList<QPointF> &get_values(void) const { return values; }

private:
    QString name;
    QList<QPointF> values;
};

class UniversalReader;

/**
 * @brief Class that orchestrates data readers and convert data into graph points.
 */
class DataProcessor : public QObject
{
    Q_OBJECT

public:
    explicit DataProcessor(QObject *parent = nullptr);
    ~DataProcessor();

    void add_variables_data(uint64_t sender_id, QMap<uint64_t, QList<DataPoint>> &data);

    static DataTime get_timestamp();

public slots:
    void setup(void);
    void process(void);

    void reported_reader_status(uint64_t reader_id, UniversalReader::Status status);

    /* TODO: remove connection to the button. */
    void start_data_processing(); /**< Slot connected to the start button. */
    void stop_data_processing(); /**< Slot connected to the stop button. */

signals:
    void send_new_data(const QList<GraphData> &new_data);
    void finished(void);

    void reader_start(uint64_t reader_id);
    void reader_stop(uint64_t reader_id);

private:
    struct DataSenderInfo
    {
        QThread *thread = nullptr;
        UniversalReader *sender = nullptr;
        std::shared_ptr<QMutex> buffer_mutex = nullptr;

        UniversalReader::Status latest_status = UniversalReader::Uninitialized;
    };

    QTimer *m_timer =
            nullptr; /**< Timer to execute processing function and send data to graph Widget. */
    QHash<uint64_t, DataSenderInfo>
            m_senders; /**< Initialized data senders. Sender is is used as a key. */
    QMap<uint64_t, QList<DataPoint>> m_in_buffers; /**< Buffers where senders store the data.
                                                     Buffer (variable) id is used as a key. Sender
                                                     Mutex is used for a thread-safety. */
    QMap<uint64_t, QVector<DataPoint>> m_buffers; /**< Buffers to store raw data from senders.
                                                     Buffer (variable) id is used as a key. */
    QMap<uint64_t, uint64_t> m_buffer_to_sender; /**< Buffer id and to sender id correspondence. */
};
