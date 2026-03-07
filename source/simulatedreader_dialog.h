#pragma once

#include "simulatedreader.h"

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class SimulatedReaderDialog;
}
QT_END_NAMESPACE

/**
 * @brief Dialog window to configure Simulated Reader
 */
class SimulatedReaderDialog : public QDialog
{
    Q_OBJECT

public:
    SimulatedReaderDialog(QWidget *parent = nullptr,
                          std::shared_ptr<SimulatedReaderConfig> config = nullptr);

    std::shared_ptr<UniversalReaderConfig> get_config();

private:
    enum TypeIndexes {
        Constant = 0,
        Sinusoid = 1,
    };

    Ui::SimulatedReaderDialog *ui = nullptr; /**< Pointer to the user interface. */
    std::shared_ptr<SimulatedReaderConfig> m_config = nullptr;

signals:
};
