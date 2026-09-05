#pragma once

#include "universalreader.hpp"

#include <QObject>
#include <QSerialPort>
#include <chrono>

/**
 * @brief Configuration for the @ref SerialReader.
 */
struct SerialReaderConfig : UniversalReaderConfig
{
    [[nodiscard]] std::shared_ptr<UniversalReaderConfig> clone() const override
    {
        return std::make_shared<SerialReaderConfig>(*this);
    }

    [[nodiscard]] std::unique_ptr<UniversalReader>
    create_reader(ReaderId id, const std::shared_ptr<UniversalReaderConfig> &self) const override;

    QString port_name{ }; /**< Serial port reference. */
    int32_t baud_rate{ }; /**< Baud rate of the port. */
    QSerialPort::DataBits data_bits{ }; /**< Amount of the data bits. */
    QSerialPort::Parity parity{ }; /**< Parity bits type. */
    QSerialPort::StopBits stop_bits{ }; /**< Stop bits configuration. */
    QSerialPort::FlowControl flow_control{ }; /**< Type of the flow control. */
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

protected:
    void setup() override; /**< Initialization of a particular type of the reader. */
    void start() override; /**< Start reading. */
    void stop() override; /**< Stop reading. */
    void process() override; /**< Prepare data before sending to the data processor. */

private:
    static constexpr int32_t minimum_baud_rate = 1200; /**< Minimum allowed baud rate. */

    QSerialPort *m_serial = nullptr; /**< Pointer to the serial port object. */
    std::chrono::duration<double>
            m_wire_byte_duration{ }; /**< Transmission duration for a single byte over the wire. */

    /**
     * @brief Get the configuration.
     *
     * @return A pointer to the configuration.
     */
    [[nodiscard]] const SerialReaderConfig *get_config();
};
