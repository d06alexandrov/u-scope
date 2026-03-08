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
     * @brief Sinusoidal wave.
     */
    struct SinConfig
    {
        int32_t frequency; /**< Frequency of the sinusoid. */
        double amplitude; /**< Amplitude of the sinusoid. */
    };

    /**
     * @brief Variants of a simulated form.
     */
    using Config = std::variant<ConstConfig, SinConfig>;

    std::shared_ptr<UniversalReaderConfig> clone() const override
    {
        return std::make_shared<SimulatedReaderConfig>(*this);
    }

    VariableId variable_id; /**< ID of the generated variable. */
    int32_t sample_rate; /**< Amount of samples per second. */
    Config form_conf; /**< Configuration for the specific form. */
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
     * @param processor Pointer to the connected Data Processor instance.
     * @param config Simulated reader configuration.
     */
    explicit SimulatedReader(ReaderId id, DataProcessor *processor,
                             std::shared_ptr<SimulatedReaderConfig> config);

public slots:

private:
    enum SimulatedForm {
        Undefined, /**< Form was not defined. */
        Constant, /**< Constant value. */
        SineWave, /**< Sinusoid. */
    };

    SimulatedForm m_form = Undefined; /**< Type of a simulated form. */
    DataTime m_setup_timestamp = 0; /**< Setup timestamp. */
    DataTime m_prev_sample_timestamp = 0; /**< Timestamp of a previous sample. */

    /**
     * @brief Get the configuration.
     *
     * @return A pointer to the configuration.
     */
    const SimulatedReaderConfig *get_config() const;

protected:
    void setup() override; /**< Initialization of a particular type of the reader. */
    void start() override; /**< Start reading. */
    void stop() override; /**< Stop reading. */
    void process() override; /**< Prepare data before sending to the data processor. */
};
