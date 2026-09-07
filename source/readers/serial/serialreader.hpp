#pragma once

#include "universalreader.hpp"

#include <QObject>
#include <QSerialPort>
#include <chrono>
#include <memory>

class SerialFrameParser;

/**
 * @brief Configuration for the @ref SerialReader.
 */
struct SerialReaderConfig : UniversalReaderConfig
{
    /**
     * @brief Type of the field in a fixed-layout packet.
     */
    enum FieldType {
        Int8, /**< Signed 8-bit integer. */
        UInt8, /**< Unsigned 8-bit integer. */
        Int16, /**< Signed 16-bit integer. */
        UInt16, /**< Unsigned 16-bit integer. */
        Int32, /**< Signed 32-bit integer. */
        UInt32, /**< Unsigned 32-bit integer. */
    };

    /**
     * @brief Endianness of the field in a fixed-layout packet.
     */
    enum Endianness {
        Little, /**< Little-endian. */
        Big, /**< Big-endian. */
    };

    /**
     * @brief Packet format for fixed-layout packets.
     */
    struct PacketFormat
    {
        QByteArray start_magic; /**< Start of the packet in hex. */
        QByteArray end_magic; /**< End of the packet in hex. Could be empty. */
        int packet_length = 0; /**< Total length including magics. */
    };

    /**
     * @brief Field in a fixed-layout packet or whole single-byte format.
     */
    struct FieldConfig
    {
        QString name; /**< Name of the field. */
        int offset = 0; /**< Offset of the field in the packet. */
        FieldType type = FieldType::Int8; /**< Type of the field. */
        Endianness endianness = Endianness::Little; /**< Endianness of the field. */
    };

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

    std::optional<PacketFormat> format{
        std::nullopt
    }; /**< Format of the packet. Empty for single-byte format. */
    QHash<VariableId, FieldConfig>
            field_configs{ }; /**< Configuration of the fields in the packet. */
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

    SerialReader(const SerialReader &reader) = delete;
    SerialReader(SerialReader &&reader) = delete;

    /**
     * @brief Destructor.
     */
    ~SerialReader() override;

    SerialReader &operator=(const SerialReader &other) = delete;
    SerialReader &operator=(SerialReader &&other) = delete;

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

    std::unique_ptr<SerialFrameParser> m_parser{ }; /**< Pointer to the serial frame parser. */

    /**
     * @brief Get the configuration.
     *
     * @return A pointer to the configuration.
     */
    [[nodiscard]] const SerialReaderConfig *get_config();
};
