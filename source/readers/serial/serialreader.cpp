#include "serialreader.hpp"

#include "serialframeparser.hpp"

namespace {

constexpr double stop_bits_one = 1.0; /**< Number of stop bits for the "one" configuration. */
constexpr double stop_bits_one_and_half =
        1.5; /**< Number of stop bits for the "one and a half" configuration. */
constexpr double stop_bits_two = 2.0; /**< Number of stop bits for the "two" configuration. */

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

std::unique_ptr<UniversalReader>
SerialReaderConfig::create_reader(ReaderId id,
                                  const std::shared_ptr<UniversalReaderConfig> &self) const
{
    return std::make_unique<SerialReader>(id, std::static_pointer_cast<SerialReaderConfig>(self));
}

SerialReader::SerialReader(ReaderId id, std::shared_ptr<SerialReaderConfig> config)
    : UniversalReader{ id, config }
    , m_serial(new QSerialPort(this))
{
}

SerialReader::~SerialReader() = default;

void SerialReader::data_received()
{
    const QByteArray new_data = m_serial->readAll();
    const auto timestamp = UData::Time::now();
    m_parser->feed(new_data, timestamp, [this](VariableId id, UData::Point &&sample) {
        store_data(id, std::move(sample));
    });
}

const SerialReaderConfig *SerialReader::get_config()
{
    return dynamic_cast<const SerialReaderConfig *>(get_universal_config());
}

void SerialReader::setup()
{
    if (get_config()->baud_rate < minimum_baud_rate) {
        throw std::range_error(tr("Baud rate must be equal or greater than %1")
                                       .arg(minimum_baud_rate)
                                       .toStdString());
    }

    m_serial->setPortName(get_config()->port_name);
    m_serial->setBaudRate(get_config()->baud_rate);
    m_serial->setDataBits(get_config()->data_bits);
    m_serial->setParity(get_config()->parity);
    m_serial->setStopBits(get_config()->stop_bits);
    m_serial->setFlowControl(get_config()->flow_control);

    connect(m_serial, &QSerialPort::readyRead, this, &SerialReader::data_received);

    // Calculate one byte duration
    const double bits_per_byte =
            calculate_bits_per_byte(m_serial->dataBits(), m_serial->parity(), m_serial->stopBits());

    m_wire_byte_duration = std::chrono::duration<double>(
            bits_per_byte / static_cast<double>(get_config()->baud_rate));

    m_parser = SerialFrameParser::create(*get_config(), m_wire_byte_duration);

    allocate_buffer_pool(2 * m_parser->variable_count(),
                         static_cast<size_t>(std::ceil(get_config()->baud_rate / bits_per_byte)));
}

void SerialReader::start()
{
    if (!m_serial->open(QIODevice::ReadOnly)) {
        throw std::runtime_error("Can not open device");
    }

    m_parser->reset();
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
    double bits_per_byte = 1.0; // Start bit

    switch (data_bits) {
    case QSerialPort::Data5:
    case QSerialPort::Data6:
    case QSerialPort::Data7:
    case QSerialPort::Data8:
        bits_per_byte += static_cast<double>(data_bits);
        break;
    default:
        bits_per_byte += static_cast<double>(QSerialPort::Data8);
        break;
    }

    if (parity != QSerialPort::NoParity) {
        bits_per_byte += 1.0;
    }

    switch (stop_bits) {
    case QSerialPort::OneStop:
        bits_per_byte += stop_bits_one;
        break;
    case QSerialPort::OneAndHalfStop:
        bits_per_byte += stop_bits_one_and_half;
        break;
    case QSerialPort::TwoStop:
        bits_per_byte += stop_bits_two;
        break;
    default:
        bits_per_byte += stop_bits_one;
        break;
    }

    return bits_per_byte;
}

} // namespace
