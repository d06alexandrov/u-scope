#include "simulatedreader.h"

#include "dataprocessor.h"

#include <cmath>

SimulatedReader::SimulatedReader(uint64_t id, DataProcessor *processor,
                                 std::shared_ptr<SimulatedReaderConfig> config)
    : UniversalReader{ id, processor, config }
{
}

const SimulatedReaderConfig *SimulatedReader::get_config() const
{
    return static_cast<const SimulatedReaderConfig *>(m_config.get());
}

void SimulatedReader::setup()
{
    const auto config = get_config();
    if (std::holds_alternative<SimulatedReaderConfig::ConstConfig>(config->form_conf)) {
        m_form = Constant;
    } else if (std::holds_alternative<SimulatedReaderConfig::SinConfig>(config->form_conf)) {
        m_form = SineWave;
    } else {
        throw std::runtime_error("Unknown simulated type");
    }

    if (config->sample_rate > 1000000) {
        throw std::range_error("Sample rate is limited to 1MHz");
    }

    m_setup_timestamp = DataProcessor::get_timestamp();
}

void SimulatedReader::start()
{
    m_prev_sample_timestamp = DataProcessor::get_timestamp();
}

void SimulatedReader::stop() { }

void SimulatedReader::process()
{
    const auto config = get_config();
    const auto current_timestamp = DataProcessor::get_timestamp();
    const auto sample_interval_us = 1000000UL / config->sample_rate;

    for (DataTime t = DataProcessor::timestamp_add_us_roundup(m_prev_sample_timestamp,
                                                              sample_interval_us);
         t < current_timestamp;
         t = DataProcessor::timestamp_add_us_roundup(t, sample_interval_us)) {

        double val = 0.0;

        switch (m_form) {
        case Constant: {
            val = std::get<SimulatedReaderConfig::ConstConfig>(config->form_conf).value;
        } break;
        case SineWave: {
            const auto &sin_conf =
                    std::get<SimulatedReaderConfig::SinConfig>(config->form_conf);
            const double x = DataProcessor::get_timestamp_diff_us(m_setup_timestamp, t) * 2 * M_PI
                    * sin_conf.frequency / 1000000;

            val = sin(x) * sin_conf.amplitude;
        } break;
        default:
            throw std::runtime_error("Unknown simulated type");
        }

        m_buffer[config->variable_id].push_back(DataPoint(t, val));

        m_prev_sample_timestamp = t;
    }
}
