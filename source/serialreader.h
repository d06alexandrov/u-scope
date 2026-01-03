#pragma once

#include "universalreader.h"

#include <QElapsedTimer>
#include <QList>
#include <QObject>
#include <QPointF>
#include <QSerialPort>
#include <QTimer>

class DataProcessor;

struct SerialReaderConfig : UniversalReaderConfig
{
    QString port_name;
    int32_t baud_rate;
    QSerialPort::DataBits data_bits;
    QSerialPort::Parity parity;
    QSerialPort::StopBits stop_bits;
    QSerialPort::FlowControl flow_control;
};

class SerialReader : public UniversalReader
{
    Q_OBJECT

public:
    explicit SerialReader(uint64_t id, DataProcessor *processor,
                          std::shared_ptr<SerialReaderConfig> config);
    ~SerialReader();

public slots:
    void data_received();

private:
    QSerialPort *m_serial = nullptr;

    SerialReaderConfig *get_config();

protected:
    void setup() override;
    void process() override;
};
