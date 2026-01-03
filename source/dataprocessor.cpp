#include "dataprocessor.h"
#include "serialreader.h"
#include "universalreader.h"

#include <QMutexLocker>
#include <QThread>
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

void DataProcessor::add_variables_data(uint64_t sender_id, QMap<uint64_t, QList<DataPoint>> &data)
{
    auto &sender_info = m_senders[sender_id];
    if (sender_info.buffer_mutex) {
        QMutexLocker locker(sender_info.buffer_mutex.get());

        for (auto [variable_id, new_data] : data.asKeyValueRange()) {
            m_in_buffers[variable_id].append(std::move(new_data));
        }
    }
}

DataProcessor::DataTime DataProcessor::get_timestamp()
{
    return std::chrono::duration_cast<std::chrono::microseconds>(
                   std::chrono::steady_clock::now().time_since_epoch())
            .count();
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
    new_sender.thread->start();

    m_senders.insert(0, std::move(new_sender));
    m_buffer_to_sender[0] = 0;

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &DataProcessor::process);
    m_timer->start(30);
}

void DataProcessor::process(void)
{
    static unsigned int counter = 0;

    QList<GraphData> new_data;
    QList<QPointF> values;

    for (int i = 0; i <= 100; i++) {
        values.emplace_back(i, sin(((i * 720.0) / 100 + counter) * M_PI / 180) * 10);
    }

    for (uint64_t var_id = 0; var_id < 1; var_id++) {
        QList<QPointF> processed_values;
        QList<DataPoint> in_data;

        auto &sender_info = m_senders[m_buffer_to_sender[var_id]];
        if (sender_info.buffer_mutex) {
            QMutexLocker locker(sender_info.buffer_mutex.get());
            std::swap(in_data, m_in_buffers[var_id]);
        }

        m_buffers[var_id].append(std::move(in_data));

        if (m_buffers[var_id].size() > 101) {
            m_buffers[var_id].remove(0, m_buffers[var_id].size() - 101);
        }

        for (int i = 0; i < m_buffers[var_id].size(); i++) {
            const auto val = std::get<char>(m_buffers[var_id].at(i).second);
            processed_values.emplace_back(i, val);
        }

        new_data.emplace_back(QString("Test data2"), std::move(processed_values));
    }

    new_data.emplace_back(QString("Test data"), std::move(values));

    emit send_new_data(new_data);

    counter = (counter + 1) % 360;
}
