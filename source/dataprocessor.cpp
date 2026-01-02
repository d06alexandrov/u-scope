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
            m_buffers[variable_id].append(std::move(new_data));

            if (m_buffers[variable_id].size() > 100) {
                m_buffers[variable_id].remove(0, m_buffers[variable_id].size() - 100);
            }
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
    DataSenderInfo new_sender;

    new_sender.thread = new QThread;
    new_sender.sender = new SerialReader(0, this);
    new_sender.buffer_mutex = std::make_shared<QMutex>();

    new_sender.sender->moveToThread(new_sender.thread);
    connect(new_sender.thread, &QThread::started, new_sender.sender, &UniversalReader::setup);
    connect(new_sender.thread, &QThread::finished, new_sender.sender, &QObject::deleteLater);
    connect(new_sender.thread, &QThread::finished, new_sender.thread, &QObject::deleteLater);
    new_sender.thread->start();

    m_senders.insert(0, std::move(new_sender));

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &DataProcessor::process);
    m_timer->start(30);
}

void DataProcessor::receive_data(uint64_t variable_id, const QList<QPointF> &new_data)
{
    //    m_buffers[variable_id].append(std::move(new_data));
    if (m_buffers[variable_id].size() > 100) {
        m_buffers[variable_id].remove(0, m_buffers[variable_id].size() - 100);
    }
}

void DataProcessor::process(void)
{
    static unsigned int counter = 0;

    QList<GraphData> new_data;
    QList<QPointF> values;

    for (int i = 0; i <= 100; i++) {
        values.emplace_back(i, sin(((i * 720.0) / 100 + counter) * M_PI / 180) * 10);
    }

    QList<QPointF> values2;

    for (int i = 0; i < m_buffers[0].size(); i++) {
        const auto val = std::get<int32_t>(m_buffers[0].at(i).second);
        values2.emplace_back(i, val);
    }

    new_data.emplace_back(QString("Test data1"), std::move(values2));

    new_data.emplace_back(QString("Test data"), std::move(values));

    emit send_new_data(new_data);

    QThread::msleep(40);

    counter = (counter + 1) % 360;
}
