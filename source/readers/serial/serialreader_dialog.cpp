#include "serialreader_dialog.hpp"

#include <QSerialPortInfo>

std::shared_ptr<UniversalReaderConfig> SerialReaderDialogConfig::to_reader_config() const
{
    auto config = std::make_shared<SerialReaderConfig>();

    config->port_name = port_name;
    config->baud_rate = baud_rate;

    // TODO: make configurable
    config->data_bits = QSerialPort::Data8;
    config->parity = QSerialPort::OddParity;
    config->stop_bits = QSerialPort::OneStop;
    config->flow_control = QSerialPort::NoFlowControl;

    return config;
}
