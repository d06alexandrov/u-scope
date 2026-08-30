#include "simulatedreader_dialog_model.hpp"

#include "simulatedreader_dialog.h"
#include "universalreader_dialog.h"

SimulatedReaderDialogModel::SimulatedReaderDialogModel(QObject *parent)
    : QAbstractListModel{ parent }
{
}

int SimulatedReaderDialogModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return static_cast<int>(m_order.size());
}

QVariant SimulatedReaderDialogModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_order.size()) {
        return { };
    }

    const VariableId variable_id = m_order.at(index.row());

    switch (role) {
    case VariableIdRole:
        return static_cast<int>(variable_id);
    case LabelRole:
        return SimulatedReaderDialogConfig::get_form_config_short_name(
                m_form_configs.value(variable_id));
    default:
        return { };
    }
}

QHash<int, QByteArray> SimulatedReaderDialogModel::roleNames() const
{
    return {
        { VariableIdRole, "variableId" },
        { LabelRole, "label" },
    };
}

void SimulatedReaderDialogModel::addConstantForm(qreal value)
{
    const VariableId new_id = UniversalReaderDialogConfig::get_available_variable_idx(
            m_original_variable_ids + m_current_variable_ids);

    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_current_variable_ids.insert(new_id);
    m_form_configs[new_id] = SimulatedReaderConfig::ConstConfig{ .value = value };
    m_order.append(new_id);
    endInsertRows();
}

void SimulatedReaderDialogModel::addSinusoidForm(int frequency, qreal amplitude)
{
    const VariableId new_id = UniversalReaderDialogConfig::get_available_variable_idx(
            m_original_variable_ids + m_current_variable_ids);

    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_current_variable_ids.insert(new_id);
    m_form_configs[new_id] =
            SimulatedReaderConfig::SinConfig{ .frequency = frequency, .amplitude = amplitude };
    m_order.append(new_id);
    endInsertRows();
}

void SimulatedReaderDialogModel::modifyConstantForm(int variable_id, qreal value)
{
    const auto id = static_cast<VariableId>(variable_id);

    if (!m_form_configs.contains(id)) {
        return;
    }

    m_form_configs[id] = SimulatedReaderConfig::ConstConfig{ .value = value };

    if (const auto row = static_cast<int>(m_order.indexOf(id)); row >= 0) {
        const QModelIndex idx = index(row);
        emit dataChanged(idx, idx, { LabelRole });
    }
}

void SimulatedReaderDialogModel::modifySinusoidForm(int variable_id, int frequency, qreal amplitude)
{
    const auto id = static_cast<VariableId>(variable_id);

    if (!m_form_configs.contains(id)) {
        return;
    }

    m_form_configs[id] =
            SimulatedReaderConfig::SinConfig{ .frequency = frequency, .amplitude = amplitude };

    if (const auto row = static_cast<int>(m_order.indexOf(id)); row >= 0) {
        const QModelIndex idx = index(row);
        emit dataChanged(idx, idx, { LabelRole });
    }
}

void SimulatedReaderDialogModel::removeForm(int variable_id)
{
    const auto id = static_cast<VariableId>(variable_id);
    const int row = static_cast<int>(m_order.indexOf(id));

    if (row < 0) {
        return;
    }

    beginRemoveRows(QModelIndex(), row, row);
    m_order.removeAt(row);
    m_form_configs.remove(id);
    m_current_variable_ids.remove(id);
    endRemoveRows();
}

QVariantMap SimulatedReaderDialogModel::formAt(int variable_id) const
{
    const auto id = static_cast<VariableId>(variable_id);

    if (!m_form_configs.contains(id)) {
        return { };
    }

    QVariantMap result{ };

    std::visit(overloads{ [&result](const SimulatedReaderConfig::ConstConfig &c) {
                             result["type"] = "constant";
                             result["value"] = c.value;
                         },
                          [&result](const SimulatedReaderConfig::SinConfig &c) {
                              result["type"] = "sinusoid";
                              result["frequency"] = c.frequency;
                              result["amplitude"] = c.amplitude;
                          } },
               m_form_configs.value(id));

    return result;
}

std::shared_ptr<UniversalReaderDialogConfig> SimulatedReaderDialogModel::build_config() const
{
    auto config = std::make_shared<SimulatedReaderDialogConfig>();

    config->form_configs = m_form_configs;

    for (const auto &[variable_id, form_conf] : m_form_configs.asKeyValueRange()) {
        config->variable_names.insert(
                variable_id, SimulatedReaderDialogConfig::get_form_config_short_name(form_conf));
    }

    return config;
}
