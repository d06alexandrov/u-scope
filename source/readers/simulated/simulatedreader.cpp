#include "simulatedreader.h"

#include <cmath>

std::unique_ptr<UniversalReader>
SimulatedReaderConfig::create_reader(ReaderId id,
                                     const std::shared_ptr<UniversalReaderConfig> &self) const
{
    return std::make_unique<SimulatedReader>(id,
                                             std::static_pointer_cast<SimulatedReaderConfig>(self));
}

SimulatedReader::SimulatedReader(ReaderId id, std::shared_ptr<SimulatedReaderConfig> config)
    : UniversalReader{ id, config }
{
}

const SimulatedReaderConfig *SimulatedReader::get_config() const
{
    return dynamic_cast<const SimulatedReaderConfig *>(get_universal_config());
}

void SimulatedReader::setup()
{
    const auto config = get_config();

    if ((config->sample_rate < min_frequency) || (config->sample_rate > max_frequency)) {
        throw std::range_error(tr("Allowed sample rate is within the range [%1;%2]")
                                       .arg(min_frequency)
                                       .arg(max_frequency)
                                       .toStdString());
    }

    m_sample_interval = UData::duration_from_seconds(1.0 / config->sample_rate);

    m_setup_timestamp = UData::Time::now();

    allocate_buffer_pool(config->form_configs.size() * 2, config->sample_rate);
}

void SimulatedReader::start()
{
    m_prev_sample_timestamp = UData::Time::now();
}

void SimulatedReader::stop() { }

void SimulatedReader::process()
{
    const auto config = get_config();
    const UData::Time current_timestamp = UData::Time::now();

    UData::Time prev_sample_timestamp = m_prev_sample_timestamp;

    for (const auto &[variable_id, form_conf] :
         this->get_config()->form_configs.asKeyValueRange()) {

        prev_sample_timestamp = m_prev_sample_timestamp;

        std::visit(overloads{ [&](const SimulatedReaderConfig::ConstConfig &const_conf) {
                                 for (UData::Time t = prev_sample_timestamp + m_sample_interval;
                                      t < current_timestamp; t += m_sample_interval) {
                                     store_data(variable_id, UData::Point(t, const_conf.value));
                                     prev_sample_timestamp = t;
                                 }
                             },
                              [&](const SimulatedReaderConfig::SinConfig &sin_conf) {
                                  for (UData::Time t = prev_sample_timestamp + m_sample_interval;
                                       t < current_timestamp; t += m_sample_interval) {
                                      const double x = UData::to_double(t - this->m_setup_timestamp)
                                              * 2 * M_PI * sin_conf.frequency;
                                      store_data(variable_id,
                                                 UData::Point(t, sin(x) * sin_conf.amplitude));
                                      prev_sample_timestamp = t;
                                  }
                              } },
                   form_conf);
    }

    m_prev_sample_timestamp = prev_sample_timestamp;
}
