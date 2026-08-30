#include "serialreader_dialog_model.hpp"

#include "serialreader_dialog.h"

#include <QSerialPortInfo>

SerialReaderDialogModel::SerialReaderDialogModel(QObject *parent)
    : QObject{ parent }
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

    VariableId variable_id = UniversalReaderDialogConfig::get_available_variable_idx();
    config->variable_names.insert(variable_id, tr("Byte data"));

    return config;
}
