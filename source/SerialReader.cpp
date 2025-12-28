#include "SerialReader.h"

#include <iostream>

SerialReader::SerialReader(QObject *parent)
    : QObject{ parent }
    , m_serial(new QSerialPort(this))
{
    // TODO: move timer to DataProcessor, so it could be usedby multiple
    // readers
    m_elapsed_timer = new QElapsedTimer;
}

SerialReader::~SerialReader()
{
    if (m_serial->isOpen()) {
        m_serial->close();
    }
}

void SerialReader::setup()
{
    m_elapsed_timer->start();

    m_serial->setPortName("/dev/pts/3");
    m_serial->setBaudRate(QSerialPort::Baud115200);
    m_serial->setDataBits(QSerialPort::Data8);
    m_serial->setParity(QSerialPort::OddParity);
    m_serial->setStopBits(QSerialPort::OneStop);
    m_serial->setFlowControl(QSerialPort::NoFlowControl);

    connect(m_serial, &QSerialPort::readyRead, this, &SerialReader::data_received);

    if (m_serial->open(QIODevice::ReadOnly)) {
        std::cout << "Opened" << std::endl;
    } else {
        std::cout << "Can not open" << std::endl;
    }

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &SerialReader::process);
    m_timer->start(10);
}

void SerialReader::data_received()
{
    const QByteArray new_data = m_serial->readAll();

    qreal timestamp = m_elapsed_timer->nsecsElapsed() / 1000000000.0;

    for (auto x : new_data) {
        m_buffer.push_back(QPointF(timestamp, x));
    }
}

void SerialReader::process()
{
    emit send_data(0, m_buffer);
    m_buffer.clear();
}