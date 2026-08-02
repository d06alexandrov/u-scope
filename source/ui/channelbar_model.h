#pragma once

#include <QAbstractListModel>
#include <QColor>
#include <vector>

/**
 * @brief Model for the channel bar in the main window.
 */
class ChannelBarModel : public QAbstractListModel
{
    Q_OBJECT

public:
    /**
     * @brief Roles for the channel bar model.
     */
    enum Roles {
        ChannelNumberRole = Qt::UserRole + 1, /**< Index of the channel. */
        ValueTextRole, /**< Text for the badge. */
        BadgeColorRole, /**< Color of the badge. */
        ChannelEnabledRole /**< If channel is enabled. */
    };

    /**
     * @brief State of a channel in the channel bar.
     */
    struct ChannelState
    {
        int number; /**< Index of the channel. */
        QString value_text; /**< Text for the badge. */
        QColor color; /**< Color of the badge. */
        bool enabled; /**< Channel status. */
    };

    /**
     * @brief Constructs a new ChannelBarModel with the given channel colors.
     *
     * @param channel_colors Colors for each channel.
     * @param parent Parent object.
     */
    explicit ChannelBarModel(const std::vector<QColor> channel_colors, QObject *parent = nullptr);

    /**
     * @brief Returns the number of channels in the model.
     *
     * @param parent Parent index.
     * @return Number of channels.
     */
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    /**
     * @brief Returns the data for the given index and role.
     *
     * @param index Index of the channel.
     * @param role Role for which data is requested.
     * @return Data for the given index and role.
     */
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    /**
     * @brief Returns the role names for the model.
     *
     * @return Hash of role names.
     */
    QHash<int, QByteArray> roleNames() const override;

    /**
     * @brief Sets the enabled state of a channel and updates its badge text.
     *
     * @param channel_idx Index of the channel.
     * @param enabled Whether the channel is enabled.
     * @param value_text Text for the badge (default is empty).
     */
    void setChannelEnabled(int channel_idx, bool enabled, const QString &value_text = "");

private:
    std::vector<ChannelState> m_channels;
};
