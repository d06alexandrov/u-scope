#include "serialreader_dialog.h"

#include "ui_serialreader_dialog.h"

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

SerialReaderDialog::SerialReaderDialog(QWidget *parent)
    : QDialog{ parent }
    , ui(new Ui::SerialReaderDialog)
{
    ui->setupUi(this);

    update_devices_list();
}

SerialReaderDialog::~SerialReaderDialog()
{
    delete ui;
}

std::shared_ptr<UniversalReaderDialogConfig> SerialReaderDialog::get_config()
{
    auto config = std::make_shared<SerialReaderDialogConfig>();

    QVariantMap parameters = ui->deviceListBox->currentData().toMap();

    config->port_name = parameters["portName"].toString();
    config->baud_rate = static_cast<int32_t>(ui->baudRateListBox->currentText().toInt());

    // Current variant has only one variable
    VariableId variable_id = UniversalReaderDialogConfig::get_available_variable_idx();
    config->variable_names.insert(variable_id, tr("Byte data"));

    return config;
}

void SerialReaderDialog::update_devices_list()
{
    ui->deviceListBox->clear();

    const auto available_ports = QSerialPortInfo::availablePorts();

    for (const auto &port : available_ports) {
        QVariantMap parameters;

        parameters["portName"] = port.portName();

        ui->deviceListBox->addItem(port.portName(), parameters);
    }
}
