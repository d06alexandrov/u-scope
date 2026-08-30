#pragma once

#include <QBindable>
#include <QObject>
#include <QProperty>
#include <QtQmlIntegration/qqmlintegration.h>

/**
 * @brief The model class for the horizontal timebase.
 */
class TimebaseModel : public QObject
{
    Q_OBJECT
    QML_ANONYMOUS

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    Q_PROPERTY(QString hScaleText READ hScaleText NOTIFY hScaleTextChanged)
    Q_PROPERTY(int hGridCells READ hGridCells NOTIFY hGridCellsChanged)
    Q_PROPERTY(int qDialValue READ qDialValue WRITE qDialValueUpdate NOTIFY qDialValueChanged)
#endif // DOXYGEN_SHOULD_SKIP_THIS

public:
    static constexpr int64_t default_division_us =
            10000; /**< Default division value in microseconds. */
    static constexpr int default_grid_cells =
            10; /**< Default amount of the horizontal grid cells. */

    /**
     * @brief Constructs a new TimebaseModel.
     *
     * @param parent Parent object.
     */
    explicit TimebaseModel(QObject *parent = nullptr);

    /**
     * @brief Returns the current horizontal division in microseconds.
     *
     * @return The current horizontal division in microseconds.
     */
    int64_t divisionUs() const;

    /**
     * @brief Returns a bindable property for the horizontal division in microseconds.
     *
     * @return A bindable property for the horizontal division in microseconds.
     */
    QBindable<int64_t> bindableDivisionUs();

    /**
     * @brief Returns the text representation of the horizontal scale.
     *
     * @return The text representation of the horizontal scale.
     */
    QString hScaleText() const;

    /**
     * @brief Returns the number of horizontal grid cells.
     *
     * @return The number of horizontal grid cells.
     */
    int hGridCells() const;

    /**
     * @brief Returns the value of the dial corresponding to the horizontal division.
     *
     * @return The value of the dial.
     */
    int qDialValue() const;

    /**
     * @brief Update horizontal division from dial value.
     *
     * @param dial_value The new value from the dial.
     */
    void qDialValueUpdate(int dial_value);

    /**
     * @brief Returns the width of the frame in microseconds.
     *
     * @return The width of the frame in microseconds.
     */
    int64_t frameWidthUs() const;

public slots:

signals:

    /**
     * @brief Signal emitted when the horizontal scale text changes.
     */
    void hScaleTextChanged();

    /**
     * @brief Signal emitted when the number of horizontal grid cells changes.
     */
    void hGridCellsChanged();

    /**
     * @brief Signal emitted when the horizontal division changes.
     */
    void hDivisionChanged();

    /**
     * @brief Signal emitted when the dial value changes.
     */
    void qDialValueChanged();

protected:
private slots:

private:
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(TimebaseModel, int64_t, m_div_us, default_division_us,
                                         &TimebaseModel::hDivisionChanged)
};
