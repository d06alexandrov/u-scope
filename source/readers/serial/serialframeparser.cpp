#include "serialframeparser.hpp"

#include <climits>
#include <cstdint>
#include <utility>

std::unique_ptr<SerialFrameParser>
SerialFrameParser::create(const SerialReaderConfig &config,
                          std::chrono::duration<double> wire_byte_duration)
{
    if (config.format.has_value()) {
        return std::make_unique<PacketFrameParser>(config.format.value(), config.field_configs);
    }

    return std::make_unique<SingleByteFrameParser>(config.field_configs, wire_byte_duration);
}

SingleByteFrameParser::SingleByteFrameParser(
        const QHash<VariableId, SerialReaderConfig::FieldConfig> &field_configs,
        std::chrono::duration<double> wire_byte_duration)
    : m_wire_byte_duration(wire_byte_duration)
{
    if (field_configs.size() != 1) {
        throw std::runtime_error("Single byte format requires exactly one configured field");
    }

    const auto it = field_configs.constBegin();
    m_variable_id = it.key();

    const auto field_type = it.value().type;

    if ((field_type != SerialReaderConfig::Int8) && (field_type != SerialReaderConfig::UInt8)) {
        throw std::runtime_error("Single byte format requires an Int8 or UInt8 field type");
    }

    m_is_signed = (field_type == SerialReaderConfig::Int8);
}

void SingleByteFrameParser::feed(const QByteArray &data, UData::Time chunk_end_time,
                                 const SampleCallback &on_sample)
{
    const auto data_size = data.size();

    for (int i = 0; i < data_size; i++) {
        const double offset_sec = ((data_size - 1 - i) * m_wire_byte_duration).count();
        const UData::Time::Duration offset = UData::duration_from_seconds(offset_sec);
        const UData::Time byte_timestamp = chunk_end_time - offset;

        const UData::Variant value = m_is_signed
                ? UData::Variant{ data[i] }
                : UData::Variant{ static_cast<int32_t>(static_cast<uint8_t>(data[i])) };

        on_sample(m_variable_id, UData::Point(byte_timestamp, value));
    }
}

void SingleByteFrameParser::reset() { }

size_t SingleByteFrameParser::variable_count() const
{
    return 1;
}

PacketFrameParser::PacketFrameParser(
        SerialReaderConfig::PacketFormat format,
        QHash<VariableId, SerialReaderConfig::FieldConfig> field_configs)
    : m_format(std::move(format))
    , m_field_configs(std::move(field_configs))
{
    if (m_format.packet_length <= 0) {
        throw std::runtime_error("Packet format requires a positive packet length");
    }

    if (m_format.start_magic.isEmpty()) {
        throw std::runtime_error("Packet format requires start magic to be set");
    }
}

void PacketFrameParser::reset()
{
    m_buffer.clear();
}

size_t PacketFrameParser::variable_count() const
{
    return static_cast<size_t>(m_field_configs.size());
}

void PacketFrameParser::feed(const QByteArray &data, UData::Time chunk_end_time,
                             const SampleCallback &on_sample)
{
    m_buffer.append(data);
    consume_buffer(chunk_end_time, on_sample);
}

void PacketFrameParser::consume_buffer(UData::Time chunk_end_time, const SampleCallback &on_sample)
{
    while (true) {
        const int start_idx = static_cast<int>(m_buffer.indexOf(m_format.start_magic));

        if (start_idx < 0) {
            const int keep = static_cast<int>(m_format.start_magic.size() - 1);
            if (m_buffer.size() > keep) {
                m_buffer.remove(0, m_buffer.size() - keep);
            }
            return;
        }

        if (start_idx > 0) {
            m_buffer.remove(0, start_idx);
        }

        if (m_buffer.size() < m_format.packet_length) {
            return;
        }

        const QByteArray candidate = m_buffer.left(m_format.packet_length);

        if (!m_format.end_magic.isEmpty()) {
            const auto magic_len = m_format.end_magic.size();

            if (candidate.right(magic_len) != m_format.end_magic) {
                m_buffer.remove(0, 1);
                continue;
            }
        }

        decode_packet(candidate, chunk_end_time, on_sample);

        m_buffer.remove(0, m_format.packet_length);
    }
}

void PacketFrameParser::decode_packet(const QByteArray &packet, UData::Time packet_time,
                                      const SampleCallback &on_sample) const
{
    for (auto it = m_field_configs.constBegin(); it != m_field_configs.constEnd(); ++it) {
        const VariableId variable_id = it.key();
        const auto &field = it.value();

        if ((field.offset + field_size(field.type)) > packet.size()) {
            continue;
        }

        on_sample(variable_id, UData::Point(packet_time, decode_field(packet, field)));
    }
}

UData::Variant PacketFrameParser::decode_field(const QByteArray &packet,
                                               const SerialReaderConfig::FieldConfig &field)
{
    const int size = field_size(field.type);

    uint32_t raw = 0;

    const auto byte_at = [&packet, &field](int i) -> uint8_t {
        return static_cast<uint8_t>(packet.at(field.offset + i));
    };

    if (field.endianness == SerialReaderConfig::Big) {
        for (int i = 0; i < size; i++) {
            raw = (raw << CHAR_BIT) | byte_at(i);
        }
    } else {
        for (int i = size - 1; i >= 0; i--) {
            raw = (raw << CHAR_BIT) | byte_at(i);
        }
    }

    switch (field.type) {
    case SerialReaderConfig::Int8:
        return static_cast<int32_t>(static_cast<int8_t>(raw));
    case SerialReaderConfig::UInt8:
        return static_cast<int32_t>(static_cast<uint8_t>(raw));
    case SerialReaderConfig::Int16:
        return static_cast<int32_t>(static_cast<int16_t>(raw));
    case SerialReaderConfig::UInt16:
        return static_cast<int32_t>(static_cast<uint16_t>(raw));
    case SerialReaderConfig::Int32:
        return static_cast<int32_t>(raw);
    case SerialReaderConfig::UInt32:
        return static_cast<uint32_t>(raw);
    default:
        return static_cast<int32_t>(raw);
    }
}

int PacketFrameParser::field_size(SerialReaderConfig::FieldType type)
{
    switch (type) {
    case SerialReaderConfig::Int8:
    case SerialReaderConfig::UInt8:
        return 1;
    case SerialReaderConfig::Int16:
    case SerialReaderConfig::UInt16:
        return 2;
    case SerialReaderConfig::Int32:
    case SerialReaderConfig::UInt32:
        return 4;
    default:
        return 0;
    }
}
