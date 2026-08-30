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

    [[nodiscard]] std::unique_ptr<UniversalReader>
    create_reader(ReaderId id, const std::shared_ptr<UniversalReaderConfig> &self) const override;

    QHash<VariableId, Config> form_configs{ }; /**< Configurations of the simulated values. */

    int32_t sample_rate{ }; /**< Amount of samples per second. */
};

Q_DECLARE_METATYPE(SimulatedReaderConfig::Config)

/**
 * @brief Simulated reader.
 */
class SimulatedReader : public UniversalReader
{
    Q_OBJECT

public:
    static constexpr int32_t min_frequency = 1; /**< Minimum allowed frequency. */
    static constexpr int32_t max_frequency = 1000000; /**< Maximum allowed frequency. */

    /**
     * @brief Constructor.
     *
     * @param id ID of the reader.
     * @param config Simulated reader configuration.
     */
    explicit SimulatedReader(ReaderId id, std::shared_ptr<SimulatedReaderConfig> config);

public slots:

private:
    UData::Time::Duration m_sample_interval{ }; /**< Interval between samples. */
    UData::Time m_setup_timestamp{ }; /**< Setup timestamp. */
    UData::Time m_prev_sample_timestamp{ }; /**< Timestamp of a previous sample. */

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
