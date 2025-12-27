#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "uartconfigdialog.h"
#include "dataprocessor.h"

#include <QMainWindow>
#include <QChart>
#include <QLineSeries>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui = nullptr;
    QChart *m_chart = nullptr;
    QLineSeries *m_series = nullptr;
    UartConfigDialog *m_uart_settings = nullptr;
    QThread *m_data_processor_thread = nullptr;

    void init_data_processor(void);

private slots:
    void receive_new_data(const QList<GraphData> &new_data);
};
#endif // MAINWINDOW_H
