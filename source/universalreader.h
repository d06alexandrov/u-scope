#pragma once

#include "dataprocessor.h"

#include <QObject>
#include <QVariant>

struct UniversalReaderConfig
{
    virtual ~UniversalReaderConfig();
};

class UniversalReader : public QObject
{
    Q_OBJECT

public:
    explicit UniversalReader(uint64_t id, DataProcessor *processor);
    UniversalReader(const UniversalReader &processor) = delete;
    UniversalReader(UniversalReader &&processor) = delete;

    virtual ~UniversalReader() = default;

    UniversalReader &operator=(const UniversalReader &other) = delete;
    UniversalReader &operator=(UniversalReader &&other) = delete;

public slots:
    virtual void setup() = 0;

protected:
    uint64_t m_id = 0;
    DataProcessor *m_data_processor = nullptr;

    QMap<uint64_t, QVector<DataProcessor::DataPoint>> m_buffer;
};
