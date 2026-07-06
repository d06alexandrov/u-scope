#include "simulatedreader_dialog_form.h"

#include "ui_simulatedreader_dialog_form.h"

#include <variant>

SimulatedReaderDialogForm::SimulatedReaderDialogForm(QWidget *parent,
                                                     const SimulatedReaderConfig::Config *config)
    : QDialog{ parent }
    , ui(new Ui::SimulatedReaderDialogForm)
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
                   *config);
    }

    // Ensure the stacked widget starts on the correct page
    ui->configStackedWidget->setCurrentIndex(ui->graphType->currentIndex());
}

SimulatedReaderDialogForm::~SimulatedReaderDialogForm()
{
    delete ui;
}

SimulatedReaderConfig::Config SimulatedReaderDialogForm::get_config()
{
    SimulatedReaderConfig::Config config;

    if (ui->graphType->currentIndex() == TypeIndexes::Constant) {
        config = SimulatedReaderConfig::ConstConfig{
            .value = ui->constantValue->value(),
        };
    } else if (ui->graphType->currentIndex() == TypeIndexes::Sinusoid) {
        config = SimulatedReaderConfig::SinConfig{
            .frequency = ui->sinusoidalFrequency->value(),
            .amplitude = ui->sinusoidalAmplitude->value(),
        };
    } else {
        throw std::runtime_error("Unknown simulated type");
    }

    return config;
}
