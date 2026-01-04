#include "mainwindow.h"

#include "dataprocessor.h"
#include "uartconfigdialog.h"
#include "ui_mainwindow.h"

#include <QChart>
#include <QLineSeries>
#include <QThread>
#include <QValueAxis>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_chart(new QChart)
    , m_series(new QLineSeries)
    , m_series2(new QLineSeries)
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
    axisY2->setRange(-129, 128);
    axisY2->setTitleText("Value2");

    m_chart->addAxis(axisX, Qt::AlignBottom);
    m_chart->addAxis(axisY, Qt::AlignLeft);
    m_chart->addAxis(axisY2, Qt::AlignLeft);
    m_chart->addSeries(m_series);
    m_chart->addSeries(m_series2);

    // Setting mock series
    m_series->setName("Sample series");
    m_series->attachAxis(axisX);
    m_series->attachAxis(axisY);
    m_series->setPointsVisible(true);

    m_series2->setName("Sample series2");
    m_series2->attachAxis(axisX);
    m_series2->attachAxis(axisY2);
    m_series2->setPointsVisible(true);

    ui->dataPlot->setChart(m_chart);

    init_data_processor();
}

MainWindow::~MainWindow()
{
    if ((m_data_processor_thread != nullptr) && (m_data_processor_thread->isRunning())) {
        m_data_processor_thread->quit();

        if (!m_data_processor_thread->wait(1000)) {
            m_data_processor_thread->terminate();
            m_data_processor_thread->wait();
        }

        delete m_data_processor_thread;
    }
    delete ui;
}

void MainWindow::init_data_processor(void)
{
    m_data_processor_thread = new QThread;
    DataProcessor *data_processor = new DataProcessor;

    data_processor->moveToThread(m_data_processor_thread);

    connect(m_data_processor_thread, &QThread::started, data_processor, &DataProcessor::setup);

    connect(data_processor, &DataProcessor::send_new_data, this, &MainWindow::receive_new_data);

    connect(m_data_processor_thread, &QThread::finished, data_processor,
            &DataProcessor::deleteLater);

    connect(ui->pushButton, &QPushButton::clicked, data_processor,
            &DataProcessor::start_data_processing);
    connect(ui->pushButton_4, &QPushButton::clicked, data_processor,
            &DataProcessor::stop_data_processing);

    m_data_processor_thread->start();
}

void MainWindow::receive_new_data(const QList<GraphData> &new_data)
{
    if (new_data.size() > 1) {
        m_series2->replace(new_data.at(0).get_values());
        m_series->replace(new_data.at(1).get_values());
    }
}
