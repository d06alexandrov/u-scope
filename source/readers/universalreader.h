#pragma once

#include "commontypes.hpp"

#include <QList>
#include <QMap>
#include <QMetaType>
#include <QObject>
#include <QSet>
#include <QTimer>
#include <memory>
#include <stack>

/**
 * @brief Configuration for the @ref UniversalReader.
 */
struct UniversalReaderConfig
{
    /**
     * @brief Default constructor.
     */
    UniversalReaderConfig() = default;
    /**
     * @brief Copy constructor.
     *
     * @param other The other UniversalReaderConfig to copy from.
     */
    UniversalReaderConfig(const UniversalReaderConfig &other) = default;
    /**
     * @brief Move constructor.
     *
     * @param other The other UniversalReaderConfig to move from.
     */
    UniversalReaderConfig(UniversalReaderConfig &&other) = default;
    /**
     * @brief Destructor.
     */
    virtual ~UniversalReaderConfig() = default;
    /**
     * @brief Copy assignment operator.
     *
     * @param other The other UniversalReaderConfig to copy from.
     * @return Reference to this UniversalReaderConfig.
     */
    UniversalReaderConfig &operator=(const UniversalReaderConfig &other) = default;
    /**
     * @brief Move assignment operator.
     *
     * @param other The other UniversalReaderConfig to move from.
     * @return Reference to this UniversalReaderConfig.
     */
    UniversalReaderConfig &operator=(UniversalReaderConfig &&other) = default;

    /**
     * @brief Clone Reader Config
     *
     * @return pointer to the copy of the config
     */
    [[nodiscard]] virtual std::shared_ptr<UniversalReaderConfig> clone() const = 0;

    std::chrono::milliseconds update_period{ }; /**< Period of sending data to data processor. */
};

Q_DECLARE_METATYPE(std::shared_ptr<UniversalReaderConfig>)

/**
 * @brief Type of the buffer that is used to store data from the reader.
 */
using UniversalReaderBufferMap = QMap<VariableId, std::shared_ptr<std::vector<UData::Point>>>;

Q_DECLARE_METATYPE(UniversalReaderBufferMap)

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

    ~UniversalReader() override = default;

    UniversalReader &operator=(const UniversalReader &other) = delete;
    UniversalReader &operator=(UniversalReader &&other) = delete;

public slots:

    void reader_setup(); /**< Initialize common part of a reader and call setup method to initialize
                            specific readers. */
    /**
     * @brief Start the reader with its periodic timer.
     *
     * @param id ID of the reader.
     */
    void reader_start(ReaderId id);

    /**
     * @brief Stop the reader with its periodic timer.
     *
     * @param id ID of the reader.
     */
    void reader_stop(ReaderId id);

    /**
     * @brief Return memory buffer to the reader.
     *
     * @param buffer Buffer to be released.
     */
    void release_buffer(UniversalReaderBufferMap buffer);

signals:

    /**
     * @brief Report reader status change.
     *
     * @param id ID of the reader.
     * @param status New status of the reader.
     */
    void report_status(ReaderId id, Status status);

    /**
     * @brief Send data to the data processor when it is ready.
     *
     * @param reader_id ID of the reader.
     * @param data Data to be sent to the data processor.
     */
    void data_ready(ReaderId reader_id, UniversalReaderBufferMap data);

protected:
    virtual void setup() = 0; /**< Initialization of a particular type of the reader. Called
                                 from reader_setup method. */
    virtual void start() = 0; /**< Start reading. */
    virtual void stop() = 0; /**< Stop reading. */
    virtual void process() = 0; /**< Prepare data before sending to the data processor. Called
                                   periodically from reader_process method. */

    /**
     * @brief Set new status of the reader and emit report_status signal if status was changed.
     *
     * @param new_status New status of the reader.
     */
    void set_status(Status new_status);

    /**
     * @brief Allocate pool of buffers to store data from the reader.
     *
     * @param amount Number of buffers to allocate.
     * @param reserved_size Number of points to reserve in each buffer.
     */
    void allocate_buffer_pool(size_t amount, size_t reserved_size);

    /**
     * @brief Store one data point in the buffer.
     *
     * @param id ID of the variable.
     * @param data Data point to be stored.
     * @return True if data was stored successfully, false otherwise.
     */
    bool store_data(const VariableId &id, UData::Point &&data);

    /**
     * @brief Get the configuration of the reader.
     *
     * @return Pointer to the configuration of the reader.
     */
    [[nodiscard]] const UniversalReaderConfig *get_universal_config() const;

private slots:

    void reader_process(); /**< Periodic function that calls process method and sends data to the
                              data processor. Triggered by timer. */

private:
    std::shared_ptr<UniversalReaderConfig> m_config{ }; /**< Reader configuration */
    ReaderId m_id{ 0 }; /**< ID of the reader. */
    Status m_status{ Status::Uninitialized }; /**< Status of the reader. */
    QTimer m_timer{ }; /**< The timer for the periodical call of the reader_process method. */

    UniversalReaderBufferMap m_buffer_map{ }; /**< Buffer to store received data before it is sent
                                                to the data processor. */
    std::stack<std::shared_ptr<std::vector<UData::Point>>>
            m_buffer_pool{ }; /**< Pool of variable buffers that can be reused. */
};
