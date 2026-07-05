#include "simulatedreader_dialog.h"

#include "simulatedreader_dialog_form.h"
#include "simulatedreader_dialog_row.h"
#include "ui_simulatedreader_dialog.h"

#include <QCoreApplication>
#include <QDebug>
#include <QObject>
#include <QPushButton>
#include <QtLogging>
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

                        add_element_to_list(new_variable_id, dialog.get_config());
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
        config->variable_names.insert(
                variable_id, SimulatedReaderDialogConfig::get_form_config_short_name(form_conf));
    }

    return config;
}

void SimulatedReaderDialog::add_element_to_list(VariableId variable_id,
                                                SimulatedReaderConfig::Config form_conf)
{
    auto new_item = new QListWidgetItem();
    auto row_widget = new SimulatedReaderDialogRow();

    new_item->setSizeHint(row_widget->sizeHint());

    new_item->setData(ItemRoles::VariableIdRole, QVariant::fromValue(variable_id));

    row_widget->set_text(SimulatedReaderDialogConfig::get_form_config_short_name(form_conf));

    connect(row_widget, &SimulatedReaderDialogRow::deleteRequested, this,
            [this, variable_id]() { this->remove_element_from_list(variable_id); });

    connect(row_widget, &SimulatedReaderDialogRow::editRequested, this, [this, variable_id]() {
        if (!this->m_form_configs.contains(variable_id)) {
            return;
        }

        const auto current_config = this->m_form_configs.value(variable_id);

        SimulatedReaderDialogForm dialog(this, &current_config);

        if (dialog.exec() == QDialog::Accepted) {
            modify_element_in_list(variable_id, dialog.get_config());
        }
    });

    this->ui->simulationForms->insertItem(this->ui->simulationForms->count() - 1, new_item);
    this->ui->simulationForms->setItemWidget(new_item, row_widget);

    this->m_reserved_variable_ids.insert(variable_id);
    this->m_form_configs[variable_id] = std::move(form_conf);
}

void SimulatedReaderDialog::remove_element_from_list(VariableId variable_id)
{
    this->m_form_configs.remove(variable_id);
    this->m_reserved_variable_ids.remove(variable_id);

    for (int i = 0; i < this->ui->simulationForms->count(); i++) {
        const auto &element_data =
                this->ui->simulationForms->item(i)->data(ItemRoles::VariableIdRole);

        if (element_data.isValid() && (element_data.value<VariableId>() == variable_id)) {
            delete this->ui->simulationForms->takeItem(i);
            break;
        }
    }
}

void SimulatedReaderDialog::modify_element_in_list(VariableId variable_id,
                                                   SimulatedReaderConfig::Config form_conf)
{
    for (int i = 0; i < this->ui->simulationForms->count(); i++) {
        const auto &element_data =
                this->ui->simulationForms->item(i)->data(ItemRoles::VariableIdRole);

        if (element_data.isValid() && (element_data.value<VariableId>() == variable_id)) {
            auto row_widget = qobject_cast<SimulatedReaderDialogRow *>(
                    this->ui->simulationForms->itemWidget(this->ui->simulationForms->item(i)));

            if (row_widget != nullptr) {
                row_widget->set_text(
                        SimulatedReaderDialogConfig::get_form_config_short_name(form_conf));
            } else {
                qWarning() << tr("Widget at index %1 has an unexpected type.").arg(i);
            }

            break;
        }
    }

    this->m_form_configs[variable_id] = std::move(form_conf);
}
