#pragma once

#include "dataprocessor.h"

#include <QChart>
#include <QLineSeries>
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/**
 * @brief Class that implements main window of the application
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

private:
    /**
     * @brief Configuration of the Graph
     */
    struct GraphStyle
    {
        static constexpr QPoint left_bottom_corner = {
            -100, -100
        }; /**< Value in the bottom left corner of the graph. */
        static constexpr QPoint right_top_corner = {
            100, 100
        }; /**< Value in the top right corner of the graph. */
        static constexpr int horizontal_grid = 10; /**< Amount of horizontal cells in a grid. */
        static constexpr int vertical_grid = 8; /**< Amount of vertical cells in a grid. */
    };

    Ui::MainWindow *ui = nullptr; /**< Pointer to the Main Window user interface. */
    QChart *m_chart = nullptr; /**< Pointer to the main QChart. */
    QLineSeries *m_series = nullptr;
    QLineSeries *m_series2 = nullptr;
    QThread *m_data_processor_thread = nullptr; /**< Thread with a running Data Processor. */

    void init_data_processor(); /**< Initialize and run Data Processor */
    void init_graph(); /**< Initialize graph */

public slots:
    void receive_new_data(const QList<GraphData>
                                  &new_data); /**< Slot to receive graph data from Data Processor */
};
