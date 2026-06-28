#pragma once

#include "simulatedreader.h"
#include "universalreader_dialog.h"

#include <QDialog>

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

    VariableId variable_id; /**< ID of the generated variable. */
    Config form_conf; /**< Configuration for the specific form. */

    std::shared_ptr<UniversalReaderConfig> to_reader_config() const override;
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
                          std::shared_ptr<const SimulatedReaderConfig> config = nullptr);

    /**
     * @brief Get configuration of the simulated reader.
     *
     * @return Pointer to the configuration of the simulated reader.
     */
    std::shared_ptr<UniversalReaderDialogConfig> get_config();

private:
    enum TypeIndexes {
        Constant = 0,
        Sinusoid = 1,
    };

    Ui::SimulatedReaderDialog *ui = nullptr; /**< Pointer to the user interface. */
    std::shared_ptr<const SimulatedReaderConfig> m_config =
            nullptr; /**< Pointer to the previous configuration of the simulated reader. */

signals:
};
