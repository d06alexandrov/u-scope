#pragma once

#include <QList>
#include <QMap>
#include <QMutex>
#include <QObject>
#include <QPointF>
#include <QSharedDataPointer>
#include <QString>
#include <QTimer>

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

class DataProcessor : public QObject
{
    Q_OBJECT

public:
    using DataVariant = std::variant<int32_t, double>;
    using DataTime = uint64_t;
    using DataPoint = QPair<DataTime, DataVariant>;

    explicit DataProcessor(QObject *parent = nullptr);
    ~DataProcessor();

    void add_variables_data(uint64_t sender_id, QMap<uint64_t, QList<DataPoint>> &data);

    static DataTime get_timestamp();

public slots:
    void setup(void);
    void process(void);

signals:
    void send_new_data(const QList<GraphData> &new_data);
    void finished(void);

private:
    struct DataSenderInfo
    {
        QThread *thread = nullptr;
        UniversalReader *sender = nullptr;
        std::shared_ptr<QMutex> buffer_mutex = nullptr;
    };

    QTimer *m_timer =
            nullptr; /**< Timer to execute processing function and send data to graph Widget. */
    QMap<uint64_t, DataSenderInfo>
            m_senders; /**< Initialized data senders. Sender is is used as a key. */
    QMap<uint64_t, QList<DataPoint>> m_in_buffers; /**< Buffers where senders store the data.
                                                     Buffer (variable) id is used as a key. Sender
                                                     Mutex is used for a thread-safety. */
    QMap<uint64_t, QVector<DataPoint>> m_buffers; /**< Buffers to store raw data from senders.
                                                     Buffer (variable) id is used as a key. */
    QMap<uint64_t, uint64_t> m_buffer_to_sender; /**< Buffer id and to sender id correspondence. */
};
