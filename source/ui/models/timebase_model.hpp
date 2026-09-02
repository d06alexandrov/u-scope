#pragma once

#include "commontypes.hpp"

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
    static constexpr std::chrono::milliseconds default_div{
        10
    }; /**< Default horizontal division duration. */
    static constexpr int default_grid_cells =
            10; /**< Default amount of the horizontal grid cells. */

    /**
     * @brief Constructs a new TimebaseModel.
     *
     * @param parent Parent object.
     */
    explicit TimebaseModel(QObject *parent = nullptr);

    /**
     * @brief Returns the current horizontal division.
     *
     * @return The current horizontal division.
     */
    [[nodiscard]] UData::Time::Duration division() const;

    /**
     * @brief Returns the text representation of the horizontal scale.
     *
     * @return The text representation of the horizontal scale.
     */
    [[nodiscard]] QString hScaleText() const;

    /**
     * @brief Returns the number of horizontal grid cells.
     *
     * @return The number of horizontal grid cells.
     */
    [[nodiscard]] int hGridCells() const;

    /**
     * @brief Returns the value of the dial corresponding to the horizontal division.
     *
     * @return The value of the dial.
     */
    [[nodiscard]] int qDialValue() const;

    /**
     * @brief Update horizontal division from dial value.
     *
     * @param dial_value The new value from the dial.
     */
    void qDialValueUpdate(int dial_value);

    /**
     * @brief Returns the width of the frame.
     *
     * @return The width of the frame.
     */
    [[nodiscard]] UData::Time::Duration frame_width() const;

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
    UData::Time::Duration m_div{ default_div }; /**< The current horizontal division. */
};
