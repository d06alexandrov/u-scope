#pragma once

#include "serialreader.hpp"
#include "udata_time.hpp"

#include <QByteArray>
#include <chrono>
#include <functional>
#include <memory>

/**
 * @brief Parser for the serial data stream, which can be either single-byte or fixed-layout packet
 * format.
 */
class SerialFrameParser
{
public:
    using SampleCallback = std::function<void(
            VariableId variable_id,
            UData::Point &&sample)>; /**< Callback for each sample decoded from the stream. */

    SerialFrameParser() = default;

    SerialFrameParser(const SerialFrameParser &parser) = delete;
    SerialFrameParser(SerialFrameParser &&parser) = delete;

    /**
     * @brief Default destructor.
     */
    virtual ~SerialFrameParser() = default;

    SerialFrameParser &operator=(const SerialFrameParser &parser) = delete;
    SerialFrameParser &operator=(SerialFrameParser &&parser) = delete;

    /**
     * @brief Feed data into the parser.
     *
     * @param data The data chunk to feed into the parser.
     * @param chunk_end_time The timestamp of the end of the data chunk.
     * @param on_sample Callback to be called for each sample decoded from the data.
     */
    virtual void feed(const QByteArray &data, UData::Time chunk_end_time,
                      const SampleCallback &on_sample) = 0;

    /**
     * @brief Reset the parser state.
     */
    virtual void reset() = 0;

    /**
     * @brief Get the number of variables configured in the parser.
     *
     * @return The number of variables configured in the parser.
     */
    [[nodiscard]] virtual size_t variable_count() const = 0;

    /**
     * @brief Factory method to create a SerialFrameParser based on the configuration.
     *
     * @param config The configuration for the SerialReader.
     * @param wire_byte_duration The duration of a single byte on the wire.
     * @return A unique pointer to the created SerialFrameParser.
     */
    [[nodiscard]] static std::unique_ptr<SerialFrameParser>
    create(const SerialReaderConfig &config, std::chrono::duration<double> wire_byte_duration);
};

/**
 * @brief Parser that treats each received byte as one sample of a single configured field.
 */
class SingleByteFrameParser : public SerialFrameParser
{
public:
    /**
     * @brief Constructor for SingleByteFrameParser.
     *
     * @param field_configs Configuration of the fields in the packet. Must contain exactly one
     * field.
     * @param wire_byte_duration Duration of one byte on the wire.
     */
    SingleByteFrameParser(const QHash<VariableId, SerialReaderConfig::FieldConfig> &field_configs,
                          std::chrono::duration<double> wire_byte_duration);

    void feed(const QByteArray &data, UData::Time chunk_end_time,
              const SampleCallback &on_sample) override;
    void reset() override;
    [[nodiscard]] size_t variable_count() const override;

private:
    VariableId m_variable_id{ 0 }; /**< ID of the single configured variable. */
    bool m_is_signed{ true }; /**< Whether the byte is interpreted as signed. */
    std::chrono::duration<double> m_wire_byte_duration{ }; /**< Duration of one byte on the wire. */
};

/**
 * @brief Parser for fixed-layout packets with optional start and end magic bytes.
 */
class PacketFrameParser : public SerialFrameParser
{
public:
    /**
     * @brief Constructor for PacketFrameParser.
     *
     * @param format Packet framing configuration.
     * @param field_configs Field decode definitions.
     */
    PacketFrameParser(SerialReaderConfig::PacketFormat format,
                      QHash<VariableId, SerialReaderConfig::FieldConfig> field_configs);

    void feed(const QByteArray &data, UData::Time chunk_end_time,
              const SampleCallback &on_sample) override;
    void reset() override;
    [[nodiscard]] size_t variable_count() const override;

private:
    SerialReaderConfig::PacketFormat m_format; /**< Packet framing configuration. */
    QHash<VariableId, SerialReaderConfig::FieldConfig>
            m_field_configs; /**< Field decode definitions. */
    QByteArray m_buffer{ }; /**< Bytes received but not yet consumed into a packet. */

    /**
     * @brief Consume bytes from the buffer, attempting to decode packets and call the sample
     * callback.
     *
     * @param chunk_end_time The timestamp of the end of the data chunk.
     * @param on_sample Callback to be called for each sample decoded from the data.
     */
    void consume_buffer(UData::Time chunk_end_time, const SampleCallback &on_sample);

    /**
     * @brief Decode a single packet and call the sample callback for each field.
     *
     * @param packet The packet data to decode.
     * @param packet_time The timestamp of the packet.
     * @param on_sample Callback to be called for each sample decoded from the packet.
     */
    void decode_packet(const QByteArray &packet, UData::Time packet_time,
                       const SampleCallback &on_sample) const;

    /**
     * @brief Decode a single field from a packet.
     *
     * @param packet The packet data to decode from.
     * @param field The field configuration to use for decoding.
     * @return the decoded field value as a UData::DataVariant.
     */
    [[nodiscard]] static UData::DataVariant decode_field(const QByteArray &packet,
                                                     const SerialReaderConfig::FieldConfig &field);

    /**
     * @brief Get the size of a field type in bytes.
     *
     * @param type The field type to get the size of.
     * @return The size of the field type in bytes.
     */
    [[nodiscard]] static int field_size(SerialReaderConfig::FieldType type);
};
