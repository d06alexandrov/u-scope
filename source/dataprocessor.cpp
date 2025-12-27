#include "dataprocessor.h"

#include <QThread>
#include <cmath>

DataProcessor::DataProcessor(QObject *parent)
    : QObject{parent}
{
}

void DataProcessor::setup(void) {
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &DataProcessor::process);
    m_timer->start(30);
}

void DataProcessor::process(void) {
    static unsigned int counter = 0;    

    QList<GraphData> new_data;

    QList<QPointF> values;

    for (int i = 0; i <= 100; i++) {
        values.emplace_back(i, sin(((i * 720.0) / 100 + counter) * M_PI / 180) * 10);
    }

    new_data.emplace_back(QString("Test data"), std::move(values));

    emit send_new_data(new_data);

    QThread::msleep(40);

    counter = (counter + 1) % 360;
}
