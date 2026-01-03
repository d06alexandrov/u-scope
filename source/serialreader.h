#pragma once

#include "universalreader.h"

#include <QElapsedTimer>
#include <QList>
#include <QObject>
#include <QPointF>
#include <QSerialPort>
#include <QTimer>

class DataProcessor;

class SerialReader : public UniversalReader
{
    Q_OBJECT

public:
    explicit SerialReader(uint64_t id, DataProcessor *processor);
    ~SerialReader();

public slots:
    void data_received();

private:
    QSerialPort *m_serial = nullptr;

protected:
    void setup() override;
    void process() override;
};
