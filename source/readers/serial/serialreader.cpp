#include "serialreader.h"

namespace {
/**
 * @brief Calculates the number of bits per byte based on the serial port configuration.
 *
 * @param data_bits Number of data bits.
 * @param parity Parity configuration.
 * @param stop_bits Number of stop bits.
 * @return Number of bits per byte.
 */
double calculate_bits_per_byte(QSerialPort::DataBits data_bits, QSerialPort::Parity parity,
                               QSerialPort::StopBits stop_bits);

} // namespace

SerialReader::SerialReader(ReaderId id, std::shared_ptr<SerialReaderConfig> config)
    : UniversalReader{ id, config }
    , m_serial(new QSerialPort(this))
{
}

void SerialReader::data_received()
{
    const QByteArray new_data = m_serial->readAll();
    const auto timestamp = UData::get_timestamp();
    const auto new_data_size = new_data.size();

    for (int i = 0; i < new_data_size; i++) {
        auto ns_offset = (new_data_size - 1 - i) * m_wire_byte_duration;
        auto us_offset = std::chrono::duration_cast<std::chrono::microseconds>(ns_offset).count();

        UData::Time byte_timestamp = UData::timestamp_sub_us_rounddown(timestamp, us_offset);

        store_data(0, UData::Point(byte_timestamp, new_data[i]));
    }
}

const SerialReaderConfig *SerialReader::get_config()
{
    return dynamic_cast<const SerialReaderConfig *>(get_universal_config());
}

void SerialReader::setup()
{
    if (get_config()->baud_rate <= 0) {
        throw std::range_error("Baud rate must be positive");
    }

    m_serial->setPortName(get_config()->port_name);
    m_serial->setBaudRate(get_config()->baud_rate);
    m_serial->setDataBits(get_config()->data_bits);
    m_serial->setParity(get_config()->parity);
    m_serial->setStopBits(get_config()->stop_bits);
    m_serial->setFlowControl(get_config()->flow_control);

    connect(m_serial, &QSerialPort::readyRead, this, &SerialReader::data_received);

    // Reserve more than enough space for the buffer
    allocate_buffer_pool(2, (get_config()->baud_rate + 7) / 8);

    // Calculate one byte duration
    double bits_per_byte =
            calculate_bits_per_byte(m_serial->dataBits(), m_serial->parity(), m_serial->stopBits());

    std::chrono::duration<double, std::ratio<1>> byte_duration_sec(
            bits_per_byte / static_cast<double>(get_config()->baud_rate));

    m_wire_byte_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(byte_duration_sec);
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

namespace {
double calculate_bits_per_byte(QSerialPort::DataBits data_bits, QSerialPort::Parity parity,
                               QSerialPort::StopBits stop_bits)
{
    double bits_per_byte = 1.0;

    switch (data_bits) {
    case QSerialPort::Data5:
        bits_per_byte += 5.0;
        break;
    case QSerialPort::Data6:
        bits_per_byte += 6.0;
        break;
    case QSerialPort::Data7:
        bits_per_byte += 7.0;
        break;
    case QSerialPort::Data8:
        bits_per_byte += 8.0;
        break;
    default:
        bits_per_byte += 8.0;
        break;
    }

    if (parity != QSerialPort::NoParity) {
        bits_per_byte += 1.0;
    }

    switch (stop_bits) {
    case QSerialPort::OneStop:
        bits_per_byte += 1.0;
        break;
    case QSerialPort::OneAndHalfStop:
        bits_per_byte += 1.5;
        break;
    case QSerialPort::TwoStop:
        bits_per_byte += 2.0;
        break;
    default:
        bits_per_byte += 1.0;
        break;
    }

    return bits_per_byte;
}

} // namespace
