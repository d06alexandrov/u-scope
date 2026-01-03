#pragma once

#include "dataprocessor.h"

#include <QObject>
#include <QVariant>

struct UniversalReaderConfig
{
    virtual ~UniversalReaderConfig() = default;
    uint32_t update_period_ms; /**< Period in ms of sending data to data processor */
};

/**
 * @brief Class that provides an unified way to create different data readers.
 */
class UniversalReader : public QObject
{
    Q_OBJECT

public:
    explicit UniversalReader(uint64_t id, DataProcessor *processor,
                             std::shared_ptr<UniversalReaderConfig> config);
    UniversalReader(const UniversalReader &processor) = delete;
    UniversalReader(UniversalReader &&processor) = delete;

    virtual ~UniversalReader() = default;

    UniversalReader &operator=(const UniversalReader &other) = delete;
    UniversalReader &operator=(UniversalReader &&other) = delete;

public slots:
    void reader_setup(); /**< Initialize common part of a reader and call setup method to initialize
                            specific readers. */
    void reader_process(); /**< Periodic function that calls process method and sends data to the
                              data processor. */

protected:
    uint64_t m_id = 0; /**< ID of the reader. */
    DataProcessor *m_data_processor = nullptr; /**< Pointer to the DataProcessor. */
    std::shared_ptr<UniversalReaderConfig> m_config; /**< Reader configuration */
    QTimer *m_timer = nullptr; /**< Pointer to the timer for the periodical call of the
                                  reader_process maethod. */

    QMap<uint64_t, QList<DataProcessor::DataPoint>>
            m_buffer; /**< Buffer to store received data before it is sent to the data processor. */

    virtual void setup() = 0; /**< Initialization of a particular type of the reader. Called
                                 from reader_setup method. */
    virtual void process() = 0; /**< Prepare data before sending to the data processor. Called
                                   periodically from reader_process method. */
};
