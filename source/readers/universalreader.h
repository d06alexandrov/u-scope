#pragma once

#include "commontypes.hpp"

#include <QList>
#include <QMap>
#include <QMetaType>
#include <QObject>
#include <QTimer>
#include <memory>

/**
 * @brief Configuration for the @ref UniversalReader.
 */
struct UniversalReaderConfig
{
    virtual ~UniversalReaderConfig() = default;
    /**
     * @brief Clone Reader Config
     *
     * @return pointer to the copy of the config
     */
    virtual std::shared_ptr<UniversalReaderConfig> clone() const = 0;
    uint32_t update_period_ms; /**< Period in ms of sending data to data processor */
};

Q_DECLARE_METATYPE(std::shared_ptr<UniversalReaderConfig>)

/**
 * @brief Class that provides an unified way to create different data readers.
 */
class UniversalReader : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Status of a current reader
     */
    enum Status {
        Uninitialized, /**< Reader was not initialized. */
        Initialized, /**< Reader was initialized and ready to start. */
        Running, /**< Reader is running and sending the data. */
        Stopped, /**< Reader was stopped and does not gather data. */
        Error, /**< Error occured. Restart is required. */
    };
    Q_ENUM(Status)

    /**
     * @brief Constructor.
     *
     * @param id ID of the reader.
     * @param config Reader configuration that includes config of the child.
     */
    explicit UniversalReader(ReaderId id, std::shared_ptr<UniversalReaderConfig> config);
    UniversalReader(const UniversalReader &processor) = delete;
    UniversalReader(UniversalReader &&processor) = delete;

    virtual ~UniversalReader() = default;

    UniversalReader &operator=(const UniversalReader &other) = delete;
    UniversalReader &operator=(UniversalReader &&other) = delete;

public slots:
    void reader_setup(); /**< Initialize common part of a reader and call setup method to initialize
                            specific readers. */
    /**
     * @brief Start the reader with its periodic timer
     *
     * @param id id of the reader
     */
    void reader_start(ReaderId id);
    /**
     * @brief Stop the reader with its periodic timer
     *
     * @param id id of the reader
     */
    void reader_stop(ReaderId id);

private slots:
    void reader_process(); /**< Periodic function that calls process method and sends data to the
                              data processor. Triggered by timer. */

signals:
    void report_status(ReaderId id, Status status); /**< Report reader status change. */
    void data_ready(ReaderId reader_id,
                    QMap<VariableId, QList<UData::Point>> data); /**< Send data when it's ready. */

protected:
    ReaderId m_id = 0; /**< ID of the reader. */
    Status m_status = Uninitialized; /**< Status of the reader. */
    std::shared_ptr<UniversalReaderConfig> m_config; /**< Reader configuration */
    QTimer *m_timer = nullptr; /**< Pointer to the timer for the periodical call of the
                                  reader_process maethod. */

    QMap<VariableId, QList<UData::Point>>
            m_buffer; /**< Buffer to store received data before it is sent to the data processor. */

    virtual void setup() = 0; /**< Initialization of a particular type of the reader. Called
                                 from reader_setup method. */
    virtual void start() = 0; /**< Start reading. */
    virtual void stop() = 0; /**< Stop reading. */
    virtual void process() = 0; /**< Prepare data before sending to the data processor. Called
                                   periodically from reader_process method. */

    void set_status(Status new_status); /**< Set new status of the reader. */
};
