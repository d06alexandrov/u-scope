#include "simulatedreader_dialog.h"

#include "ui_simulatedreader_dialog.h"

#include <variant>

std::shared_ptr<UniversalReaderConfig> SimulatedReaderDialogConfig::to_reader_config() const
{
    auto config = std::make_shared<SimulatedReaderConfig>();
    config->variable_id = this->variable_id;

    std::visit(overloads{ [&config](const ConstConfig &const_conf) {
                             config->form_conf =
                                     SimulatedReaderConfig::ConstConfig{ .value =
                                                                                 const_conf.value };
                             config->sample_rate = 100;
                         },
                          [&config](const SinConfig &sin_conf) {
                              config->form_conf = SimulatedReaderConfig::SinConfig{
                                  .frequency = sin_conf.frequency,
                                  .amplitude = sin_conf.amplitude,
                              };
                              config->sample_rate = sin_conf.frequency * 25;
                          } },
               this->form_conf);

    return config;
}

SimulatedReaderDialog::SimulatedReaderDialog(QWidget *parent,
                                             std::shared_ptr<const SimulatedReaderConfig> config)
    : QDialog{ parent }
    , ui(new Ui::SimulatedReaderDialog)
    , m_config(config)
{
    ui->setupUi(this);

    connect(ui->graphType, QOverload<int>::of(&QComboBox::currentIndexChanged),
            ui->configStackedWidget, &QStackedWidget::setCurrentIndex);

    if (config != nullptr) {
        std::visit(overloads{
                           [ui = this->ui](const SimulatedReaderConfig::ConstConfig &const_conf) {
                               ui->graphType->setCurrentIndex(TypeIndexes::Constant);
                               ui->constantValue->setValue(const_conf.value);
                           },
                           [ui = this->ui](const SimulatedReaderConfig::SinConfig &sin_conf) {
                               ui->graphType->setCurrentIndex(TypeIndexes::Sinusoid);
                               ui->sinusoidalFrequency->setValue(sin_conf.frequency);
                               ui->sinusoidalAmplitude->setValue(sin_conf.amplitude);
                           },
                   },
                   config->form_conf);
    }

    // Ensure the stacked widget starts on the correct page
    ui->configStackedWidget->setCurrentIndex(ui->graphType->currentIndex());
}

std::shared_ptr<UniversalReaderDialogConfig> SimulatedReaderDialog::get_config()
{
    auto config = std::make_shared<SimulatedReaderDialogConfig>();

    if (ui->graphType->currentIndex() == TypeIndexes::Constant) {
        config->form_conf = SimulatedReaderDialogConfig::ConstConfig{
            .value = ui->constantValue->value(),
        };
    } else if (ui->graphType->currentIndex() == TypeIndexes::Sinusoid) {
        config->form_conf = SimulatedReaderDialogConfig::SinConfig{
            .frequency = ui->sinusoidalFrequency->value(),
            .amplitude = ui->sinusoidalAmplitude->value(),
        };
    } else {
        throw std::runtime_error("Unknown simulated type");
    }

    return config;
}
