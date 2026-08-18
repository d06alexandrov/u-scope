#pragma once

#include "commontypes.hpp"

#include <QObject>
#include <vector>

/**
 * @brief The model class for the vertical scale.
 */
class VerticalScaleModel : public QObject
{
    Q_OBJECT

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    Q_PROPERTY(int vGridCells READ vGridCells NOTIFY vGridCellsChanged)
#endif // DOXYGEN_SHOULD_SKIP_THIS

public:
    static constexpr int64_t default_division_uval =
            1000000; /**< Default division value in 10^-6. */
    static constexpr int default_grid_cells =
            2 * 4; /**< Default amount of the vertical grid cells. */

    /**
     * @brief Constructs a new VerticalScaleModel.
     *
     * @param channels_amount Number of channels in the application.
     * @param parent Parent object.
     */
    explicit VerticalScaleModel(size_t channels_amount, QObject *parent = nullptr);

    /**
     * @brief Returns the number of vertical grid cells.
     *
     * @return The number of vertical grid cells.
     */
    [[nodiscard]] int vGridCells() const;

    /**
     * @brief Returns the value of the dial corresponding to the vertical division.
     *
     * @param id The channel ID for which to get the dial value.
     * @return The value of the dial.
     */
    [[nodiscard]] Q_INVOKABLE int qDialValue(int id) const;

    /**
     * @brief Returns the vertical scale factor for a given channel.
     *
     * @param id The channel ID for which to get the vertical scale factor.
     * @return The vertical scale factor.
     */
    [[nodiscard]] qreal vScaleFactor(ChannelId id) const;

    /**
     * @brief Returns the text representation of the vertical scale.
     *
     * @param id The channel ID for which to get the vertical scale text.
     * @return The text representation of the vertical scale.
     */
    [[nodiscard]] QString vScaleText(ChannelId id) const;

    /**
     * @brief Update vertical division from dial value.
     *
     * @param channel_id The channel ID for which to update the vertical division.
     * @param dial_value The new value from the dial.
     */
    Q_INVOKABLE void qDialValueUpdate(int channel_id, int dial_value);

    /**
     * @brief Reset the vertical division of a channel to the default value.
     *
     * @param id The channel ID for which to reset the vertical division.
     */
    void reset_channel(ChannelId id);

public slots:

signals:

    /**
     * @brief Signal emitted when the number of vertical grid cells changes.
     */
    void vGridCellsChanged();

    /**
     * @brief Signal emitted when the vertical division changes.
     */
    void vDivisionChanged();

protected:
private slots:

private:
    std::vector<int64_t> m_channel_scales_uval; /**< Vertical scales of channels in 10^-6. */
};
