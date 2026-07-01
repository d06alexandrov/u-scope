#include "simulatedreader_dialog_form.h"

#include "ui_simulatedreader_dialog_form.h"

#include <variant>

SimulatedReaderDialogForm::SimulatedReaderDialogForm(
        QWidget *parent, std::shared_ptr<const SimulatedReaderDialogConfig::Config> config)
    : QDialog{ parent }
    , ui(new Ui::SimulatedReaderDialogForm)
{
    ui->setupUi(this);

    connect(ui->graphType, QOverload<int>::of(&QComboBox::currentIndexChanged),
            ui->configStackedWidget, &QStackedWidget::setCurrentIndex);

    if (config != nullptr) {
        std::visit(overloads{
                           [ui = this->ui](
                                   const SimulatedReaderDialogConfig::ConstConfig &const_conf) {
                               ui->graphType->setCurrentIndex(TypeIndexes::Constant);
                               ui->constantValue->setValue(const_conf.value);
                           },
                           [ui = this->ui](const SimulatedReaderDialogConfig::SinConfig &sin_conf) {
                               ui->graphType->setCurrentIndex(TypeIndexes::Sinusoid);
                               ui->sinusoidalFrequency->setValue(sin_conf.frequency);
                               ui->sinusoidalAmplitude->setValue(sin_conf.amplitude);
                           },
                   },
                   *config);
    }

    // Ensure the stacked widget starts on the correct page
    ui->configStackedWidget->setCurrentIndex(ui->graphType->currentIndex());
}

SimulatedReaderDialogConfig::Config SimulatedReaderDialogForm::get_config()
{
    SimulatedReaderDialogConfig::Config config;

    if (ui->graphType->currentIndex() == TypeIndexes::Constant) {
        config = SimulatedReaderDialogConfig::ConstConfig{
            .value = ui->constantValue->value(),
        };
    } else if (ui->graphType->currentIndex() == TypeIndexes::Sinusoid) {
        config = SimulatedReaderDialogConfig::SinConfig{
            .frequency = ui->sinusoidalFrequency->value(),
            .amplitude = ui->sinusoidalAmplitude->value(),
        };
    } else {
        throw std::runtime_error("Unknown simulated type");
    }

    return config;
}
