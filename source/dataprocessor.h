#pragma once

#include "SerialReader.h"

#include <QList>
#include <QMap>
#include <QObject>
#include <QPointF>
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

class DataProcessor : public QObject
{
    Q_OBJECT

public:
    explicit DataProcessor(QObject *parent = nullptr);
    ~DataProcessor();

public slots:
    void setup(void);
    void receive_data(uint64_t variable_id, const QList<QPointF> &new_data);

private slots:
    void process(void);

signals:
    void send_new_data(const QList<GraphData> &new_data);
    void finished(void);

private:
    class DataSender
    {
    public:
        QThread *thread = nullptr;
        SerialReader *sender = nullptr;
    };

    QTimer *m_timer = nullptr;
    QMap<uint64_t, DataSender> m_senders;
    QMap<uint64_t, QVector<QPointF>> m_buffers;
};
