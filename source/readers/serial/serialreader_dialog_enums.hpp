#pragma once
#include <QObject>
#include <QSerialPort>
#include <QtQmlIntegration>

namespace SerialPort {

struct SerialPortForeign
{
    Q_GADGET
    QML_FOREIGN_NAMESPACE(QSerialPort)
    QML_NAMED_ELEMENT(SerialPort)
};

} // namespace SerialPort