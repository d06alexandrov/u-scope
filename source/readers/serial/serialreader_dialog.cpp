#include "serialreader_dialog.hpp"

#include <QSerialPortInfo>

std::shared_ptr<UniversalReaderConfig> SerialReaderDialogConfig::to_reader_config() const
{
    auto config = std::make_shared<SerialReaderConfig>();

    config->port_name = port_name;
    config->baud_rate = baud_rate;
    config->data_bits = data_bits;
    config->parity = parity;
    config->stop_bits = stop_bits;
    config->flow_control = flow_control;

    config->format = format;
    config->field_configs = field_configs;

    return config;
}
