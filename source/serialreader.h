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

public slots:
    void data_received();

private:
    QSerialPort *m_serial = nullptr;

    SerialReaderConfig *get_config();

protected:
    void setup() override; /**< Initialization of a particular type of the reader. */
    void start() override; /**< Start reading. */
    void stop() override; /**< Stop reading. */
    void process() override; /**< Prepare data before sending to the data processor. */
};
