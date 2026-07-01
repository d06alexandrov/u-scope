#pragma once

#include "simulatedreader.h"
#include "simulatedreader_dialog.h"
#include "universalreader_dialog.h"

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class SimulatedReaderDialogForm;
}
QT_END_NAMESPACE
#if 0
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
};

#endif

/**
 * @brief Dialog window to configure Simulated Reader Form
 */
class SimulatedReaderDialogForm : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Constructor of the dialog window.
     *
     * @param parent Parent widget.
     * @param config Pointer to the configuration of the simulated reader.
     */
    SimulatedReaderDialogForm(
            QWidget *parent = nullptr,
            std::shared_ptr<const SimulatedReaderDialogConfig::Config> config = nullptr);

    /**
     * @brief Get configuration of the simulated reader form.
     *
     * @return Pointer to the configuration of the simulated reader form.
     */
    std::shared_ptr<const SimulatedReaderDialogConfig::Config> get_config();

private:
    enum TypeIndexes {
        Constant = 0,
        Sinusoid = 1,
    };

    Ui::SimulatedReaderDialogForm *ui = nullptr; /**< Pointer to the user interface. */

signals:
};
