#pragma once

#include "simulatedreader.h"
#include "universalreader_dialog.h"

#include <QDialog>
#include <QHash>
#include <QSet>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui {
class SimulatedReaderDialog;
}
QT_END_NAMESPACE

/**
 * @brief Configuration for the @ref SimulatedReader.
 */
struct SimulatedReaderDialogConfig : UniversalReaderDialogConfig
{
    /**
     * @brief Constant value.
     */
    struct ConstConfig
    {
        double value; /**< Constant value. */
    };

    /**
     * @brief Sinusoidal wave.
     */
    struct SinConfig
    {
        int32_t frequency; /**< Frequency of the sinusoid. */
        double amplitude; /**< Amplitude of the sinusoid. */
    };

    /**
     * @brief Variants of a simulated form.
     */
    using Config = std::variant<ConstConfig, SinConfig>;

    QHash<VariableId, Config> form_configs; /**< Configurations of the simulated values. */

    std::shared_ptr<UniversalReaderConfig> to_reader_config() const override;

    static QString get_form_config_short_name(const Config &form_conf);
};

/**
 * @brief Dialog window to configure Simulated Reader
 */
class SimulatedReaderDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Constructor of the dialog window.
     *
     * @param parent Parent widget.
     * @param config Pointer to the configuration of the simulated reader.
     */
    SimulatedReaderDialog(QWidget *parent = nullptr,
                          std::shared_ptr<const SimulatedReaderDialogConfig> config = nullptr);

    /**
     * @brief Destructor of the dialog window.
     */
    ~SimulatedReaderDialog();

    /**
     * @brief Get configuration of the simulated reader.
     *
     * @return Pointer to the configuration of the simulated reader.
     */
    std::shared_ptr<UniversalReaderDialogConfig> get_config();

private:
    /**
     * @brief Roles of the items in the list.
     */
    enum ItemRoles {
        VariableIdRole = Qt::UserRole + 1, /**< Role for the variable ID. */
    };

    Ui::SimulatedReaderDialog *ui = nullptr; /**< Pointer to the user interface. */
    std::shared_ptr<const SimulatedReaderDialogConfig> m_config =
            nullptr; /**< Pointer to the previous configuration of the simulated reader. */
    QSet<VariableId> m_original_variable_ids; /**< Set of original variable IDs. */
    QSet<VariableId>
            m_reserved_variable_ids; /**< Set of currently used or original variable IDs. */
    QHash<VariableId, SimulatedReaderDialogConfig::Config>
            m_form_configs; /**< Configurations of the simulated values. */

    /**
     * @brief Add a new element to the list of simulated forms.
     *
     * @param variable_id ID of the variable to add.
     * @param form_conf Configuration of the simulated form to add.
     */
    void add_element_to_list(VariableId variable_id, SimulatedReaderDialogConfig::Config form_conf);

    /**
     * @brief Remove an element from the list of simulated forms.
     *
     * @param variable_id ID of the variable to remove.
     */
    void remove_element_from_list(VariableId variable_id);

    /**
     * @brief Modify an existing element in the list of simulated forms.
     *
     * @param variable_id ID of the existing variable to modify.
     * @param form_conf New configuration of the simulated form.
     */
    void modify_element_in_list(VariableId variable_id,
                                SimulatedReaderDialogConfig::Config form_conf);

signals:
};
