#include "simulatedreader_dialog.h"

#include "simulatedreader_dialog_form.h"
#include "simulatedreader_dialog_row.h"
#include "ui_simulatedreader_dialog.h"

#include <QDebug>
#include <QPushButton>
#include <variant>

std::shared_ptr<UniversalReaderConfig> SimulatedReaderDialogConfig::to_reader_config() const
{
    auto config = std::make_shared<SimulatedReaderConfig>();

    for (const auto &[variable_id, form_conf] : this->form_configs.asKeyValueRange()) {
        std::visit(overloads{ [&config, variable_id](const ConstConfig &const_conf) {
                                 config->form_configs.insert(variable_id,
                                                             SimulatedReaderConfig::ConstConfig{
                                                                     .value = const_conf.value });
                             },
                              [&config, variable_id](const SinConfig &sin_conf) {
                                  config->form_configs.insert(
                                          variable_id,
                                          SimulatedReaderConfig::SinConfig{
                                                  .frequency = sin_conf.frequency,
                                                  .amplitude = sin_conf.amplitude });
                              } },
                   form_conf);
    }

    config->sample_rate = SimulatedReaderConfig::default_sample_rate;

    return config;
}

SimulatedReaderDialog::SimulatedReaderDialog(
        QWidget *parent, std::shared_ptr<const SimulatedReaderDialogConfig> config)
    : QDialog{ parent }
    , ui(new Ui::SimulatedReaderDialog)
    , m_config(config)
{
    ui->setupUi(this);

    auto add_button = ui->simulationForms->item(0);

    if (add_button == nullptr) {
        throw std::runtime_error("Add button not found in the list");
    }

    if (config != nullptr) {
        // TODO: Add existing elements in case of modification
        for (const auto &[variable_id, form_conf] : config->form_configs.asKeyValueRange()) {
            add_element_to_list(variable_id, form_conf);
        }

        this->m_original_variable_ids = this->m_reserved_variable_ids;
    }

    connect(ui->simulationForms, &QListWidget::itemClicked, this,
            [this, add_button](QListWidgetItem *item) {
                if (item == add_button) {

                    SimulatedReaderDialogForm dialog(this);

                    if (dialog.exec() == QDialog::Accepted) {
                        const VariableId new_variable_id =
                                UniversalReaderDialogConfig::get_available_variable_idx(
                                        this->m_reserved_variable_ids);

                        const auto config = dialog.get_config();

                        add_element_to_list(new_variable_id, config);
                    }
                }
            });
}

SimulatedReaderDialog::~SimulatedReaderDialog()
{
    delete ui;
}

std::shared_ptr<UniversalReaderDialogConfig> SimulatedReaderDialog::get_config()
{
    auto config = std::make_shared<SimulatedReaderDialogConfig>();

    config->form_configs = this->m_form_configs;

    for (const auto &[variable_id, form_conf] : this->m_form_configs.asKeyValueRange()) {
        std::visit(overloads{ [&config, variable_id](
                                      const SimulatedReaderDialogConfig::ConstConfig &const_conf) {
                                 config->variable_names.insert(
                                         variable_id, tr("Constant %1").arg(const_conf.value));
                             },
                              [&config, variable_id](
                                      const SimulatedReaderDialogConfig::SinConfig &sin_conf) {
                                  config->variable_names.insert(variable_id,
                                                                tr("Sinusoid (%1 Amplitude) %2 Hz")
                                                                        .arg(sin_conf.amplitude)
                                                                        .arg(sin_conf.frequency));
                              } },
                   form_conf);
    }

    return config;
}

void SimulatedReaderDialog::add_element_to_list(
        const VariableId variable_id, const SimulatedReaderDialogConfig::Config &form_conf)
{
    auto new_item = new QListWidgetItem();
    auto row_widget = new SimulatedReaderDialogRow();

    new_item->setSizeHint(row_widget->sizeHint());

    new_item->setData(ItemRoles::VariableIdRole, QVariant::fromValue(variable_id));

    std::visit(
            overloads{
                    [this, row_widget](const SimulatedReaderDialogConfig::ConstConfig &const_conf) {
                        row_widget->set_text(tr("Constant %1").arg(const_conf.value));
                    },
                    [this, row_widget](const SimulatedReaderDialogConfig::SinConfig &sin_conf) {
                        row_widget->set_text(tr("Sinusoid (%1 Amplitude) %2 Hz")
                                                     .arg(sin_conf.amplitude)
                                                     .arg(sin_conf.frequency));
                    } },
            form_conf);

    this->ui->simulationForms->insertItem(this->ui->simulationForms->count() - 1, new_item);
    this->ui->simulationForms->setItemWidget(new_item, row_widget);

    this->m_reserved_variable_ids.insert(variable_id);
    this->m_form_configs[variable_id] = form_conf;
}
