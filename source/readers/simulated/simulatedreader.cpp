#include "simulatedreader.h"

#include <cmath>

SimulatedReader::SimulatedReader(ReaderId id, std::shared_ptr<SimulatedReaderConfig> config)
    : UniversalReader{ id, config }
{
}

const SimulatedReaderConfig *SimulatedReader::get_config() const
{
    return static_cast<const SimulatedReaderConfig *>(m_config.get());
}

void SimulatedReader::setup()
{
    const auto config = get_config();

    if (config->sample_rate > 1000000) {
        throw std::range_error("Sample rate is limited to 1MHz");
    } else if (config->sample_rate < 1) {
        throw std::range_error("Sample rate must be at least 1");
    }

    m_setup_timestamp = UData::get_timestamp();

    allocate_buffer_pool(config->form_configs.size() * 2, config->sample_rate);
}

void SimulatedReader::start()
{
    m_prev_sample_timestamp = UData::get_timestamp();
}

void SimulatedReader::stop() { }

void SimulatedReader::process()
{
    const auto config = get_config();
    const auto current_timestamp = UData::get_timestamp();
    const auto sample_interval_us = 1000000UL / config->sample_rate;

    UData::Time prev_sample_timestamp = m_prev_sample_timestamp;

    for (const auto &[variable_id, form_conf] :
         this->get_config()->form_configs.asKeyValueRange()) {

        prev_sample_timestamp = m_prev_sample_timestamp;

        std::visit(overloads{ [&](const SimulatedReaderConfig::ConstConfig &const_conf) {
                                 for (UData::Time t = UData::timestamp_add_us_roundup(
                                              prev_sample_timestamp, sample_interval_us);
                                      t < current_timestamp;
                                      t = UData::timestamp_add_us_roundup(t, sample_interval_us)) {
                                     store_data(variable_id, UData::Point(t, const_conf.value));
                                     prev_sample_timestamp = t;
                                 }
                             },
                              [&](const SimulatedReaderConfig::SinConfig &sin_conf) {
                                  for (UData::Time t = UData::timestamp_add_us_roundup(
                                               prev_sample_timestamp, sample_interval_us);
                                       t < current_timestamp;
                                       t = UData::timestamp_add_us_roundup(t, sample_interval_us)) {
                                      const double x = UData::get_timestamp_diff_us(
                                                               this->m_setup_timestamp, t)
                                              * 2 * M_PI * sin_conf.frequency / 1000000;
                                      store_data(variable_id,
                                                 UData::Point(t, sin(x) * sin_conf.amplitude));
                                      prev_sample_timestamp = t;
                                  }
                              } },
                   form_conf);
    }

    m_prev_sample_timestamp = prev_sample_timestamp;
}
