#pragma once

#include <QElapsedTimer>
#include <QList>
#include <QObject>
#include <QPointF>
#include <QSerialPort>
#include <QTimer>

class SerialReader : public QObject
{
    Q_OBJECT

public:
    explicit SerialReader(QObject *parent = nullptr);
    ~SerialReader();

public slots:
    void setup();
    void data_received();
    void process();

signals:
    void send_data(uint64_t variable_id, const QList<QPointF> &new_data);

private:
    QTimer *m_timer = nullptr;
    QSerialPort *m_serial = nullptr;
    QElapsedTimer *m_elapsed_timer = nullptr;

    QList<QPointF> m_buffer;
};
