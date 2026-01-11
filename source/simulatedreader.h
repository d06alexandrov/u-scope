#pragma once

#include "universalreader.h"

#include <QObject>

class DataProcessor;

/**
 * @brief Configuration for the @ref SimulatedReader.
 */
struct SimulatedReaderConfig : UniversalReaderConfig
{
    /**
     * @brief Constant value.
     */
    struct ConstConfig
    {
        double value; /**< Constant value. */
    };

    /**
     * @brief Sinus wave.
     */
    struct SinConfig
    {
        int32_t frequency; /**< Frequency of the sin. */
        double amplitude; /**< Amplitude of the sinus. */
    };

    using Config = std::variant<ConstConfig, SinConfig>;

    uint64_t variable_id; /**< ID of the generated variable. */
    int32_t sample_rate; /**< Amount of samples per second. */
    Config configuration; /**< Universal configuration for different forms. */
};

/**
 * @brief Simulated reader.
 */
class SimulatedReader : public UniversalReader
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     *
     * @param id id of the reader.
     * @param processor pointer to the connected Data Processor instance.
     * @param config Simulated reader configuration.
     */
    explicit SimulatedReader(uint64_t id, DataProcessor *processor,
                             std::shared_ptr<SimulatedReaderConfig> config);

public slots:

private:
    enum SimulatedForm {
        Undefined, /**< Form was not defined. */
        Constant, /**< Constant value. */
        SinusWave, /**< Sinus form. */
    };

    SimulatedForm m_form = Undefined; /**< Type of a simulated form. */
    DataTime m_setup_timestamp = 0; /**< Setup timestamp. */
    DataTime m_prev_sample_timestamp = 0; /**< Timestamp of a previous sample. */

    const SimulatedReaderConfig *get_config() const; /**< Get configuration. */

protected:
    void setup() override; /**< Initialization of a particular type of the reader. */
    void start() override; /**< Start reading. */
    void stop() override; /**< Stop reading. */
    void process() override; /**< Prepare data before sending to the data processor. */
};
