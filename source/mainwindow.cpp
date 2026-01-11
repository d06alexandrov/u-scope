#include "mainwindow.h"

#include "dataprocessor.h"
#include "ui_mainwindow.h"

#include <QChart>
#include <QLineSeries>
#include <QThread>
#include <QValueAxis>

MainWindow::MainWindow()
    : QMainWindow(nullptr)
    , ui(new Ui::MainWindow)
    , m_chart(new QChart)
    , m_series(new QLineSeries)
    , m_series2(new QLineSeries)
{
    ui->setupUi(this);

    init_graph();

    try {
        // Setting mock series
        auto axisX = m_chart->axes(Qt::Horizontal).front();
        auto axisY = m_chart->axes(Qt::Vertical).front();

        m_chart->addSeries(m_series);

        m_series->setName("Sample series");
        m_series->attachAxis(axisX);
        m_series->attachAxis(axisY);
        m_series->setPointsVisible(true);

        m_chart->addSeries(m_series2);

        m_series2->setName("Sample series2");
        m_series2->attachAxis(axisX);
        m_series2->attachAxis(axisY);
        m_series2->setPointsVisible(true);

    } catch (const std::exception &e) {
    }

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

void MainWindow::init_data_processor()
{
    m_data_processor_thread = new QThread;
    DataProcessor *data_processor = new DataProcessor;

    data_processor->moveToThread(m_data_processor_thread);

    connect(m_data_processor_thread, &QThread::started, data_processor, &DataProcessor::setup);

    connect(data_processor, &DataProcessor::send_new_data, this, &MainWindow::receive_new_data);

    connect(m_data_processor_thread, &QThread::finished, data_processor,
            &DataProcessor::deleteLater);

    connect(ui->pushButton_StartAll, &QPushButton::clicked, data_processor,
            &DataProcessor::start_data_processing);
    connect(ui->pushButton_StopAll, &QPushButton::clicked, data_processor,
            &DataProcessor::stop_data_processing);

    m_data_processor_thread->start();
}

/**
 * @brief Config an axis
 *
 * @param axis pointer to the axis
 * @param min minimum axis value
 * @param max maximum axis value
 * @param grid_cells amount of the grid cells
 * @param grid_color color of the grid
 */
static void config_axis(QValueAxis *axis, int min, int max, int grid_cells,
                        const QColor grid_color);

void MainWindow::init_graph()
{
    // Configure graph
    auto axisX = new QValueAxis;

    config_axis(axisX, GraphStyle::left_bottom_corner.x(), GraphStyle::right_top_corner.x(),
                GraphStyle::horizontal_grid, GraphStyle::grid_line_color);

    auto axisY = new QValueAxis;

    config_axis(axisY, GraphStyle::left_bottom_corner.y(), GraphStyle::right_top_corner.y(),
                GraphStyle::vertical_grid, GraphStyle::grid_line_color);

    m_chart->addAxis(axisX, Qt::AlignBottom);
    m_chart->addAxis(axisY, Qt::AlignLeft);

    m_chart->setBackgroundBrush(QBrush(GraphStyle::background_color));

    m_chart->legend()->hide();

    ui->dataPlot->setChart(m_chart);
}

void MainWindow::receive_new_data(const QList<GraphData> &new_data)
{
    if (new_data.size() > 1) {
        m_series2->replace(new_data.at(0).get_values());
        m_series->replace(new_data.at(1).get_values());

        if (new_data.at(0).get_values().size() > 0) {
            ui->ch1Val->setText(QString::number(new_data.at(0).get_values().back().y()));
        }
        if (new_data.at(1).get_values().size() > 0) {
            ui->ch2Val->setText(QString::number(new_data.at(1).get_values().back().y()));
        }
    }
}

static void config_axis(QValueAxis *axis, int min, int max, int grid_cells, const QColor grid_color)
{
    axis->setRange(min, max);

    // Define grid line style
    QPen gridPen;
    gridPen.setColor(grid_color);
    gridPen.setWidth(1);
    axis->setGridLinePen(gridPen);
    axis->setGridLineVisible(true);

    // Hide axis labels and main line
    axis->setLabelsVisible(false);
    axis->setLineVisible(false);

    axis->setTickCount(grid_cells + 1);
}