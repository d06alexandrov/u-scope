#pragma once

#include "universalreader.h"

#include <QHash>
#include <QObject>

/**
 * @brief Configuration for the @ref SimulatedReader.
 */
struct SimulatedReaderConfig : UniversalReaderConfig
{
    static constexpr int32_t default_sample_rate = 100; /**< Default sample rate. */

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

    [[nodiscard]] std::shared_ptr<UniversalReaderConfig> clone() const override
    {
        return std::make_shared<SimulatedReaderConfig>(*this);
    }

    QHash<VariableId, Config> form_configs; /**< Configurations of the simulated values. */

    int32_t sample_rate; /**< Amount of samples per second. */
};

Q_DECLARE_METATYPE(SimulatedReaderConfig::Config)

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
     * @param id ID of the reader.
     * @param config Simulated reader configuration.
     */
    explicit SimulatedReader(ReaderId id, std::shared_ptr<SimulatedReaderConfig> config);

public slots:

private:
    UData::Time m_setup_timestamp = 0; /**< Setup timestamp. */
    UData::Time m_prev_sample_timestamp = 0; /**< Timestamp of a previous sample. */

    /**
     * @brief Get the configuration.
     *
     * @return A pointer to the configuration.
     */
    [[nodiscard]] const SimulatedReaderConfig *get_config() const;

protected:
    void setup() override; /**< Initialization of a particular type of the reader. */
    void start() override; /**< Start reading. */
    void stop() override; /**< Stop reading. */
    void process() override; /**< Prepare data before sending to the data processor. */
};
