#include "simulatedreader_dialog.h"

#include "ui_simulatedreader_dialog.h"

SimulatedReaderDialog::SimulatedReaderDialog(QWidget *parent)
    : QDialog{ parent }
    , ui(new Ui::SimulatedReaderDialog)
{
    ui->setupUi(this);
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
