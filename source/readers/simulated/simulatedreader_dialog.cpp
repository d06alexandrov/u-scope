#include "simulatedreader_dialog.h"

#include <QCoreApplication>
#include <variant>

std::shared_ptr<UniversalReaderConfig> SimulatedReaderDialogConfig::to_reader_config() const
{
    auto config = std::make_shared<SimulatedReaderConfig>();

    config->form_configs = this->form_configs;
    config->sample_rate = SimulatedReaderConfig::default_sample_rate;

    return config;
}

QString SimulatedReaderDialogConfig::get_form_config_short_name(
        const SimulatedReaderConfig::Config &form_conf)
{
    return std::visit(
            overloads{ [](const SimulatedReaderConfig::ConstConfig &const_conf) {
                          return QCoreApplication::translate("SimulatedReaderDialogConfig",
                                                             "Constant %1")
                                  .arg(const_conf.value);
                      },
                       [](const SimulatedReaderConfig::SinConfig &sin_conf) {
                           return QCoreApplication::translate("SimulatedReaderDialogConfig",
                                                              "Sinusoid (%1 Amplitude) %2 Hz")
                                   .arg(sin_conf.amplitude)
                                   .arg(sin_conf.frequency);
                       } },
            form_conf);
}
