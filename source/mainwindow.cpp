#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "uartconfigdialog.h"

#include <QChart>
#include <QLineSeries>
#include <QValueAxis>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_chart(new QChart)
    , m_series(new QLineSeries)
    , m_uart_settings(new UartConfigDialog(this))
{
    ui->setupUi(this);

    connect(ui->actionConfig_UART, &QAction::triggered, m_uart_settings, &UartConfigDialog::show);

    auto axisX = new QValueAxis;
    axisX->setRange(0, 100);
    axisX->setLabelFormat("%g");
    axisX->setTitleText("Samples");

    auto axisY = new QValueAxis;
    axisY->setRange(-10, 60);
    axisY->setTitleText("Value");

    m_chart->addAxis(axisX, Qt::AlignBottom);
    m_chart->addAxis(axisY, Qt::AlignLeft);
    m_chart->addSeries(m_series);

    m_series->setName("Sample series");
    m_series->attachAxis(axisX);
    m_series->attachAxis(axisY);
    m_series->setPointsVisible(true);

    m_series->append(0, 0);
    m_series->append(1, 15);
    m_series->append(2, 12);
    m_series->append(3, 11);
    m_series->append(4, 50);
    m_series->append(10, 13);
    m_series->append(20, 3);
    m_series->append(90, 2);

    ui->dataPlot->setChart(m_chart);
}

MainWindow::~MainWindow()
{
    delete ui;
}
