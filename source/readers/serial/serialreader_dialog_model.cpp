#include "serialreader_dialog_model.hpp"

#include "id_allocator.hpp"
#include "serialreader_dialog.hpp"

#include <QSerialPortInfo>

SerialReaderDialogModel::SerialReaderDialogModel(QObject *parent)
    : QObject{ parent }
    , m_data_bits{ QSerialPort::DataBits::Data8 }
    , m_parity{ QSerialPort::Parity::NoParity }
    , m_stop_bits{ QSerialPort::StopBits::OneStop }
    , m_flow_control{ QSerialPort::FlowControl::NoFlowControl }
    , m_fields(new PacketFieldsModel(this))
{
}

QStringList SerialReaderDialogModel::availablePorts() const
{
    QStringList result{ };

    for (const auto &port : QSerialPortInfo::availablePorts()) {
        result << port.portName();
    }

    return result;
}

std::shared_ptr<UniversalReaderDialogConfig> SerialReaderDialogModel::build_config() const
{
    auto config = std::make_shared<SerialReaderDialogConfig>();

    config->port_name = m_port_name;
    config->baud_rate = m_baud_rate;
    config->data_bits = static_cast<QSerialPort::DataBits>(m_data_bits);
    config->parity = static_cast<QSerialPort::Parity>(m_parity);
    config->stop_bits = static_cast<QSerialPort::StopBits>(m_stop_bits);
    config->flow_control = static_cast<QSerialPort::FlowControl>(m_flow_control);

    if (m_format_mode == "packet") {
        config->format = SerialReaderConfig::PacketFormat{
            .start_magic = QByteArray::fromHex(m_start_magic_hex.toUtf8()),
            .end_magic = QByteArray::fromHex(m_end_magic_hex.toUtf8()),
            .packet_length = m_packet_length,
        };
        config->field_configs = m_fields->field_configs();
    } else {
        // TODO: generate id based on previous reserved IDs
        const VariableId variable_id{ 0 };

        config->format = std::nullopt;
        config->field_configs = {
            { variable_id,
              SerialReaderConfig::FieldConfig{
                      .name = m_signed_byte ? tr("Signed byte") : tr("Unsigned byte"),
                      .offset = 0,
                      .type = m_signed_byte ? SerialReaderConfig::FieldType::Int8
                                            : SerialReaderConfig::FieldType::UInt8,
                      .endianness = SerialReaderConfig::Endianness::Little,
              } },
        };
    }

    for (const auto &[variable_id, field] : config->field_configs.asKeyValueRange()) {
        config->variable_names.insert(variable_id, field.name);
    }

    return config;
}
