#include "simulatedreader.h"

#include "dataprocessor.h"

#include <cmath>

SimulatedReader::SimulatedReader(ReaderId id, DataProcessor *processor,
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

    for (const auto &form_conf : config->form_configs) {
        if (!std::holds_alternative<SimulatedReaderConfig::ConstConfig>(form_conf)
            && !std::holds_alternative<SimulatedReaderConfig::SinConfig>(form_conf)) {
            throw std::runtime_error("Unknown simulated type");
        }
    }

    if (config->sample_rate > 1000000) {
        throw std::range_error("Sample rate is limited to 1MHz");
    } else if (config->sample_rate < 1) {
        throw std::range_error("Sample rate must be at least 1");
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

    DataTime prev_sample_timestamp = m_prev_sample_timestamp;

    for (const auto &[variable_id, form_conf] :
         this->get_config()->form_configs.asKeyValueRange()) {

        prev_sample_timestamp = m_prev_sample_timestamp;

        for (DataTime t = DataProcessor::timestamp_add_us_roundup(prev_sample_timestamp,
                                                                  sample_interval_us);
             t < current_timestamp;
             t = DataProcessor::timestamp_add_us_roundup(t, sample_interval_us)) {

            double val = 0.0;

            std::visit(overloads{
                               [&val](const SimulatedReaderConfig::ConstConfig &const_conf) {
                                   val = const_conf.value;
                               },
                               [&val, setup_timestamp = this->m_setup_timestamp,
                                t](const SimulatedReaderConfig::SinConfig &sin_conf) {
                                   const double x =
                                           DataProcessor::get_timestamp_diff_us(setup_timestamp, t)
                                           * 2 * M_PI * sin_conf.frequency / 1000000;
                                   val = sin(x) * sin_conf.amplitude;
                               },
                       },
                       form_conf);

            m_buffer[variable_id].push_back(DataPoint(t, val));

            prev_sample_timestamp = t;
        }
    }

    m_prev_sample_timestamp = prev_sample_timestamp;
}
