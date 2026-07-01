#pragma once

#include <QString>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class SimulatedReaderDialogRow;
}
QT_END_NAMESPACE

/**
 * @brief Dialog window to configure Simulated Reader Form
 */
class SimulatedReaderDialogRow : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructor of the dialog window.
     *
     * @param parent Parent widget.
     * @param config Pointer to the configuration of the simulated reader.
     */
    SimulatedReaderDialogRow(QWidget *parent = nullptr);

    /**
     * @brief Set the text of the label in the row.
     *
     * @param text The text to set in the label.
     */
    void set_text(const QString &text);

private:
    Ui::SimulatedReaderDialogRow *ui = nullptr; /**< Pointer to the user interface. */

signals:
};
