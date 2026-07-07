#include "serialreader.h"

SerialReader::SerialReader(ReaderId id, std::shared_ptr<SerialReaderConfig> config)
    : UniversalReader{ id, config }
    , m_serial(new QSerialPort(this))
{
}

void SerialReader::data_received()
{
    const QByteArray new_data = m_serial->readAll();

    const auto timestamp = UData::get_timestamp();

    for (auto x : new_data) {
        m_buffer[0].push_back(UData::Point(timestamp, x));
    }
}

SerialReaderConfig *SerialReader::get_config()
{
    return static_cast<SerialReaderConfig *>(m_config.get());
}

void SerialReader::setup()
{
    m_serial->setPortName(get_config()->port_name);
    m_serial->setBaudRate(get_config()->baud_rate);
    m_serial->setDataBits(get_config()->data_bits);
    m_serial->setParity(get_config()->parity);
    m_serial->setStopBits(get_config()->stop_bits);
    m_serial->setFlowControl(get_config()->flow_control);

    connect(m_serial, &QSerialPort::readyRead, this, &SerialReader::data_received);
}

void SerialReader::start()
{
    if (!m_serial->open(QIODevice::ReadOnly)) {
        throw std::runtime_error("Can not open device");
    }
}
void SerialReader::stop()
{
    if (m_serial->isOpen()) {
        m_serial->close();
    }
}
void SerialReader::process() { }
