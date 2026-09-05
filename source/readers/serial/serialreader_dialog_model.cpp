#include "serialreader_dialog_model.hpp"

#include "id_allocator.hpp"
#include "serialreader_dialog.hpp"

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

    const auto variable_id = UData::get_available_id<VariableId>();
    config->variable_names.insert(variable_id, tr("Byte data"));

    return config;
}
