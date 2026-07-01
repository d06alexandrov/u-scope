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
    SimulatedReaderDialogConfig::Config get_config();

private:
    enum TypeIndexes {
        Constant = 0,
        Sinusoid = 1,
    };

    Ui::SimulatedReaderDialogForm *ui = nullptr; /**< Pointer to the user interface. */

signals:
};
