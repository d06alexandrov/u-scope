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

DataProcessor::DataProcessor(QObject *parent)
    : QObject{ parent }
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
    auto &sender_info = m_senders[reader_id];
    if (sender_info.buffer_mutex) {
        QMutexLocker locker(sender_info.buffer_mutex.get());

        for (auto [variable_id, new_data] : data.asKeyValueRange()) {
            m_in_buffers[variable_id].append(std::move(new_data));
        }
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
    auto config = std::make_shared<SerialReaderConfig>();
    config->port_name = QString("/dev/pts/3");
    config->baud_rate = 115200;
    config->data_bits = QSerialPort::Data8;
    config->parity = QSerialPort::OddParity;
    config->stop_bits = QSerialPort::OneStop;
    config->flow_control = QSerialPort::NoFlowControl;

    config->update_period_ms = 10;

    DataSenderInfo new_sender{ new QThread(), new SerialReader(0, this, std::move(config)),
                               std::make_shared<QMutex>() };

    new_sender.sender->moveToThread(new_sender.thread);
    connect(new_sender.thread, &QThread::started, new_sender.sender,
            &UniversalReader::reader_setup);
    connect(new_sender.thread, &QThread::finished, new_sender.sender, &QObject::deleteLater);
    connect(new_sender.thread, &QThread::finished, new_sender.thread, &QObject::deleteLater);

    connect(new_sender.sender, &UniversalReader::report_status, this,
            &DataProcessor::reported_reader_status);
    connect(this, &DataProcessor::reader_start, new_sender.sender, &UniversalReader::reader_start);
    connect(this, &DataProcessor::reader_stop, new_sender.sender, &UniversalReader::reader_stop);

    m_senders.insert(0, std::move(new_sender));
    m_buffer_to_sender[0] = 0;

    auto config2 = std::make_shared<SimulatedReaderConfig>();
    config2->form_conf = SimulatedReaderConfig::SinConfig{
        .frequency = 10,
        .amplitude = 25,
    };

    config2->update_period_ms = 20;
    config2->variable_id = 1;
    config2->sample_rate = 1000;

    DataSenderInfo new_sender2{ new QThread(), new SimulatedReader(1, this, std::move(config2)),
                                std::make_shared<QMutex>() };

    new_sender2.sender->moveToThread(new_sender2.thread);
    connect(new_sender2.thread, &QThread::started, new_sender2.sender,
            &UniversalReader::reader_setup);
    connect(new_sender2.thread, &QThread::finished, new_sender2.sender, &QObject::deleteLater);
    connect(new_sender2.thread, &QThread::finished, new_sender2.thread, &QObject::deleteLater);

    connect(new_sender2.sender, &UniversalReader::report_status, this,
            &DataProcessor::reported_reader_status);
    connect(this, &DataProcessor::reader_start, new_sender2.sender, &UniversalReader::reader_start);
    connect(this, &DataProcessor::reader_stop, new_sender2.sender, &UniversalReader::reader_stop);

    m_senders.insert(1, std::move(new_sender2));
    m_buffer_to_sender[1] = 1;

    new_sender.thread->start();
    new_sender2.thread->start();

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &DataProcessor::process);
    m_timer->start(30);
}

void DataProcessor::process(void)
{
    QList<GraphData> new_data;

    for (VariableId var_id = 0; var_id < 2; var_id++) {
        QList<QPointF> processed_values;
        QList<DataPoint> in_data;

        auto &sender_info = m_senders[m_buffer_to_sender[var_id]];
        if (sender_info.buffer_mutex) {
            QMutexLocker locker(sender_info.buffer_mutex.get());
            std::swap(in_data, m_in_buffers[var_id]);
        }

        m_buffers[var_id].append(std::move(in_data));

        if (m_buffers[var_id].size() > 201) {
            m_buffers[var_id].remove(0, m_buffers[var_id].size() - 201);
        }

        for (int i = 0; i < m_buffers[var_id].size(); i++) {
            const auto val = std::visit([](auto &&arg) { return static_cast<qreal>(arg); },
                                        m_buffers[var_id].at(i).second);
            processed_values.emplace_back(i - 100, val);
        }

        new_data.emplace_back(QString("Test data2"), std::move(processed_values));
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
