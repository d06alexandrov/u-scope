#pragma once

#include "universalreader.h"

#include <QObject>
#include <QSerialPort>
#include <chrono>

class DataProcessor;

/**
 * @brief Configuration for the @ref SerialReader.
 */
struct SerialReaderConfig : UniversalReaderConfig
{
    std::shared_ptr<UniversalReaderConfig> clone() const override
    {
        return std::make_shared<SerialReaderConfig>(*this);
    }

    QString port_name; /**< Serial port reference. */
    int32_t baud_rate; /**< Baud rate of the port. */
    QSerialPort::DataBits data_bits; /**< Amount of the data bits. */
    QSerialPort::Parity parity; /**< Parity bits type. */
    QSerialPort::StopBits stop_bits; /**< Stop bits configuration. */
    QSerialPort::FlowControl flow_control; /**< Type of the flow control. */
};

/**
 * @brief Serial port reader.
 */
class SerialReader : public UniversalReader
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     *
     * @param id ID of the reader.
     * @param config Serial port configuration.
     */
    explicit SerialReader(ReaderId id, std::shared_ptr<SerialReaderConfig> config);

public slots:
    /**
     * @brief Handler for the incoming data.
     */
    void data_received();

private:
    QSerialPort *m_serial = nullptr;
    std::chrono::nanoseconds m_wire_byte_duration{
        0
    }; /**< Transmission duration for a single byte over the wire. */

    SerialReaderConfig *get_config();

protected:
    void setup() override; /**< Initialization of a particular type of the reader. */
    void start() override; /**< Start reading. */
    void stop() override; /**< Stop reading. */
    void process() override; /**< Prepare data before sending to the data processor. */
};
