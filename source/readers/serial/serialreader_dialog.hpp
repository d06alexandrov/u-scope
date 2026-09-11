#pragma once

#include "serialreader.hpp"
#include "universalreader_dialog.hpp"

#include <QString>

/**
 * @brief Configuration for the @ref SerialReader.
 */
struct SerialReaderDialogConfig : UniversalReaderDialogConfig
{
    QString port_name{ }; /**< Name of the port. */
    int32_t baud_rate{ }; /**< Baud rate of the interface. */
    QSerialPort::DataBits data_bits{ }; /**< Amount of the data bits. */
    QSerialPort::Parity parity{ }; /**< Parity setting. */
    QSerialPort::StopBits stop_bits{ }; /**< Amount of the stop bits. */
    QSerialPort::FlowControl flow_control{ }; /**< Flow control setting. */

    std::optional<SerialReaderConfig::PacketFormat> format{
        std::nullopt
    }; /**< Format of the packet. Empty for single-byte format. */
    QHash<VariableId, SerialReaderConfig::FieldConfig>
            field_configs{ }; /**< Configuration of the fields in the packet. */

    [[nodiscard]] std::shared_ptr<UniversalReaderConfig> to_reader_config() const override;
};
