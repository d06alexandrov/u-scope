#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "uartconfigdialog.h"
#include "dataprocessor.h"

#include <QChart>
#include <QLineSeries>
#include <QValueAxis>
#include <QThread>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_chart(new QChart)
    , m_series(new QLineSeries)
    , m_uart_settings(new UartConfigDialog(this))
{
    ui->setupUi(this);

    // Configure buttons
    connect(ui->actionConfig_UART, &QAction::triggered, m_uart_settings, &UartConfigDialog::show);

    // Configure graph
    auto axisX = new QValueAxis;
    axisX->setRange(0, 100);
    axisX->setLabelFormat("%g");
    axisX->setTitleText("Samples");

    auto axisY = new QValueAxis;
    axisY->setRange(-10, 10);
    axisY->setTitleText("Value");
    auto axisY2 = new QValueAxis;
    axisY2->setRange(-15, 15);
    axisY2->setTitleText("Value2");

    m_chart->addAxis(axisX, Qt::AlignBottom);
    m_chart->addAxis(axisY, Qt::AlignLeft);
    m_chart->addAxis(axisY2, Qt::AlignLeft);
    m_chart->addSeries(m_series);

    // Setting mock series
    m_series->setName("Sample series");
    m_series->attachAxis(axisX);
    m_series->attachAxis(axisY2);
    m_series->setPointsVisible(true);

    ui->dataPlot->setChart(m_chart);

    init_data_processor();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init_data_processor(void)
{
    QThread *thread = new QThread;
    DataProcessor *data_processor = new DataProcessor;

    data_processor->moveToThread(thread);

    connect( thread, &QThread::started, data_processor, &DataProcessor::process);

    connect( data_processor, &DataProcessor::send_new_data, this, &MainWindow::receive_new_data);

    connect( data_processor, &DataProcessor::finished, thread, &QThread::quit);
    connect( data_processor, &DataProcessor::finished, data_processor, &DataProcessor::deleteLater);
    connect( thread, &QThread::finished, thread, &QThread::deleteLater);

    thread->start();
}

void MainWindow::receive_new_data(const QList<GraphData> &new_data)
{
    if (new_data.size() > 0) {
        m_series->replace(new_data.at(0).get_values());
    }
}
