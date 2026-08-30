#pragma once

#include "commontypes.hpp"
#include "simulatedreader.h"

#include <QAbstractListModel>
#include <QSet>
#include <QVariantMap>
#include <QtQmlIntegration/qqmlintegration.h>
#include <memory>

struct UniversalReaderDialogConfig;

/**
 * @brief QML-facing session model backing the simulated source configuration dialog.
 */
class SimulatedReaderDialogModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    /**
     * @brief Roles for the model.
     */
    enum Roles {
        VariableIdRole = Qt::UserRole + 1, /**< Role for the variable ID. */
        LabelRole, /**< Role for the variable label. */
    };

    /**
     * @brief Constructor for the SimulatedSourceDialogModel.
     *
     * @param parent The parent QObject, default is nullptr.
     */
    explicit SimulatedReaderDialogModel(QObject *parent = nullptr);

    /**
     * @brief Returns the number of simulated forms in the model.
     *
     * @param parent Parent index.
     * @return Number of forms.
     */
    [[nodiscard]] int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    /**
     * @brief Returns the data for a given index and role.
     *
     * @param index Index of the form to retrieve data for.
     * @param role Role to retrieve data for.
     * @return Data for the given index and role.
     */
    [[nodiscard]] QVariant data(const QModelIndex &index,
                                int role = Qt::DisplayRole) const override;

    /**
     * @brief Returns the role names for the model.
     *
     * @return Hash of role names.
     */
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    /**
     * @brief Add a new constant form to the list of simulated forms.
     *
     * @param value The constant value of the form to add.
     */
    Q_INVOKABLE void addConstantForm(qreal value);

    /**
     * @brief Add a new sinusoid form to the list of simulated forms.
     *
     * @param frequency Frequency of the sinusoid to add.
     * @param amplitude Amplitude of the sinusoid to add.
     */
    Q_INVOKABLE void addSinusoidForm(int frequency, qreal amplitude);

    /**
     * @brief Modify an existing element in the list of simulated forms.
     *
     * @note If the corresponding element is not a constant form, it will be replaced with a
     * constant form.
     *
     * @param variable_id ID of the form to modify.
     * @param value New constant value of the form.
     */
    Q_INVOKABLE void modifyConstantForm(int variable_id, qreal value);

    /**
     * @brief Modify an existing element in the list of simulated forms.
     *
     * @note If the corresponding element is not a sinusoid form, it will be replaced with a
     * sinusoid form.
     *
     * @param variable_id ID of the form to modify.
     * @param frequency New frequency of the sinusoid form.
     * @param amplitude New amplitude of the sinusoid form.
     */
    Q_INVOKABLE void modifySinusoidForm(int variable_id, int frequency, qreal amplitude);

    /**
     * @brief Remove an existing element from the list of simulated forms.
     *
     * @param variable_id ID of the form to remove.
     */
    Q_INVOKABLE void removeForm(int variable_id);

    /**
     * @brief Get a form's current parameters, for pre-filling the edit popup.
     *
     * @param variable_id ID of the form to look up.
     * @return Map with "type" ("constant"/"sinusoid") and the relevant numeric fields.
     */
    [[nodiscard]] Q_INVOKABLE QVariantMap formAt(int variable_id) const;

    /**
     * @brief Build a reader configuration from the model's current rows.
     *
     * @return New SimulatedReaderDialogConfig.
     */
    [[nodiscard]] std::shared_ptr<UniversalReaderDialogConfig> build_config() const;

private:
    QSet<VariableId> m_original_variable_ids; /**< Set of original variable IDs. */
    QSet<VariableId> m_current_variable_ids; /**< Set of currently used variable IDs. */
    QHash<VariableId, SimulatedReaderConfig::Config>
            m_form_configs; /**< Configurations of the simulated values. */
    QList<VariableId> m_order; /**< Order of the simulated values in the displayed list. */
};
