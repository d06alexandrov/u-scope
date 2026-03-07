#include "simulatedreader_dialog.h"

#include "ui_simulatedreader_dialog.h"

#include <variant>

SimulatedReaderDialog::SimulatedReaderDialog(QWidget *parent,
                                             std::shared_ptr<SimulatedReaderConfig> config)
    : QDialog{ parent }
    , ui(new Ui::SimulatedReaderDialog)
    , m_config(config)
{
    ui->setupUi(this);

    connect(ui->graphType, QOverload<int>::of(&QComboBox::currentIndexChanged),
            ui->configStackedWidget, &QStackedWidget::setCurrentIndex);

    if (config != nullptr) {
        std::visit(overloads{
                           [ui = this->ui](SimulatedReaderConfig::ConstConfig &const_conf) {
                               ui->graphType->setCurrentIndex(TypeIndexes::Constant);
                               ui->constantValue->setValue(const_conf.value);
                           },
                           [ui = this->ui](SimulatedReaderConfig::SinConfig &sin_conf) {
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

std::shared_ptr<UniversalReaderConfig> SimulatedReaderDialog::get_config()
{
    auto config = std::make_shared<SimulatedReaderConfig>();

    if (ui->graphType->currentIndex() == TypeIndexes::Constant) {
        config->sample_rate = 100;
        config->form_conf = SimulatedReaderConfig::ConstConfig{
            .value = ui->constantValue->value(),
        };
    } else if (ui->graphType->currentIndex() == TypeIndexes::Sinusoid) {
        config->sample_rate = ui->sinusoidalFrequency->value() * 25;
        config->form_conf = SimulatedReaderConfig::SinConfig{
            .frequency = ui->sinusoidalFrequency->value(),
            .amplitude = ui->sinusoidalAmplitude->value(),
        };
    } else {
        throw std::runtime_error("Unknown simulated type");
    }

    return config;
}
