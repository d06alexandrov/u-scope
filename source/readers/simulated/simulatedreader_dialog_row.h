#pragma once

#include <QString>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class SimulatedReaderDialogRow;
}
QT_END_NAMESPACE

/**
 * @brief One row of the simulated reader dialog, containing a label and two buttons (edit and
 * delete).
 */
class SimulatedReaderDialogRow : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructor of the dialog row.
     *
     * @param parent Parent widget.
     */
    SimulatedReaderDialogRow(QWidget *parent = nullptr);

    /**
     * @brief Destructor of the dialog row.
     */
    ~SimulatedReaderDialogRow();

    /**
     * @brief Set the text of the label in the row.
     *
     * @param text The text to set in the label.
     */
    void set_text(const QString &text);

private:
    Ui::SimulatedReaderDialogRow *ui = nullptr; /**< Pointer to the user interface. */

signals:
    /**
     * @brief Signal emitted when the edit button is clicked.
     */
    void editRequested();
    /**
     * @brief Signal emitted when the delete button is clicked.
     */
    void deleteRequested();
};
