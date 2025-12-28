#ifndef UARTCONFIGDIALOG_H
#define UARTCONFIGDIALOG_H

#include <QDialog>

namespace Ui {
class UartConfigDialog;
}

class UartConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UartConfigDialog(QWidget *parent = nullptr);
    ~UartConfigDialog();

private:
    Ui::UartConfigDialog *ui;

    virtual void showEvent(QShowEvent *event);

    void fillInterfaceList();
    void updateInterfaceDescription();
};

#endif // UARTCONFIGDIALOG_H
