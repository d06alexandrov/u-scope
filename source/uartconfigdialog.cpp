#include "uartconfigdialog.h"
#include "ui_uartconfigdialog.h"

#include <QSerialPortInfo>

UartConfigDialog::UartConfigDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::UartConfigDialog)
{
    ui->setupUi(this);

    fillInterfaceList();
    updateInterfaceDescription();

    connect(ui->interfaceListBox, &QComboBox::currentIndexChanged, this,
            &UartConfigDialog::updateInterfaceDescription);
}

void UartConfigDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    fillInterfaceList();
    updateInterfaceDescription();
}

void UartConfigDialog::fillInterfaceList()
{
    ui->interfaceListBox->clear();

    const auto infos = QSerialPortInfo::availablePorts();

    const QString blankString = "n/a";

    for (const QSerialPortInfo &info : infos) {
        QStringList list;
        QVariantMap parameters;

        if (!info.description().isEmpty()) {
            parameters["description"] = info.description();
        }

        if (!info.manufacturer().isEmpty()) {
            parameters["manufacturer"] = info.manufacturer();
        }

        if (!info.serialNumber().isEmpty()) {
            parameters["serial_number"] = info.serialNumber();
        }

        ui->interfaceListBox->addItem(info.portName(), parameters);
    }
}

void UartConfigDialog::updateInterfaceDescription()
{
    const int current_index = ui->interfaceListBox->currentIndex();
    const auto parameters = ui->interfaceListBox->itemData(current_index).toMap();

    ui->labelDescriptionData->setText(parameters.value("description", "N/A").toString());
    ui->labelManufacturerData->setText(parameters.value("manufacturer", "N/A").toString());
    ui->labelSerialNumberData->setText(parameters.value("serial_number", "N/A").toString());
}

UartConfigDialog::~UartConfigDialog()
{
    delete ui;
}
