#pragma once

#include "commontypes.hpp"

#include <QAbstractListModel>
#include <QColor>
#include <optional>
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
        ChannelIdRole = Qt::UserRole + 1, /**< Index of the channel. */
        ChannelNumberRole, /**< Number of the channel. */
        BadgeColorRole, /**< Color of the badge. */
        ValueTextRole, /**< Text for the badge. */
        ChannelConnected, /**< If channel is connected to the source. */
        ChannelEnabledRole, /**< If channel is enabled. */
        ChannelSelectedRole, /**< If channel is selected. */
    };

    /**
     * @brief State of a channel in the channel bar.
     */
    struct ChannelState
    {
        QColor color; /**< Color of the badge. */
        QString value_text; /**< Text for the badge. */
        bool connected; /**< Channel is connected. */
        bool enabled; /**< Channel is enabled. */
        bool selected; /**< Channel is selected. */
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
    [[nodiscard]] int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    /**
     * @brief Returns the data for the given index and role.
     *
     * @param index Index of the channel.
     * @param role Role for which data is requested.
     * @return Data for the given index and role.
     */
    [[nodiscard]] QVariant data(const QModelIndex &index,
                                int role = Qt::DisplayRole) const override;

    /**
     * @brief Returns the role names for the model.
     *
     * @return Hash of role names.
     */
    QHash<int, QByteArray> roleNames() const override;

    /**
     * @brief Connects a channel to the source.
     *
     * @param id Index of the channel to connect.
     */
    void connect_channel(ChannelId id);

    /**
     * @brief Disconnects a channel from the source.
     *
     * @param id Index of the channel to disconnect.
     */
    void disconnect_channel(ChannelId id);

    /**
     * @brief Selects a channel in the channel bar.
     *
     * @param id Index of the channel to select.
     */
    void select_channel(ChannelId id);

    /**
     * @brief Enables a channel and updates its badge text.
     *
     * @param id Index of the channel to enable.
     * @param value_text Text for the badge (default is empty).
     */
    void enable_channel(ChannelId id, const QString &value_text = tr("ON"));

    /**
     * @brief Disables a channel.
     *
     * @param id Index of the channel to disable.
     */
    void disable_channel(ChannelId id);

    /**
     * @brief Sets the text for a channel's badge.
     *
     * @param id Index of the channel.
     * @param value_text Text for the badge.
     */
    void set_channel_text(ChannelId id, const QString &value_text);

    /**
     * @brief Checks if a channel is enabled.
     *
     * @param id Index of the channel to check.
     * @return True if the channel is enabled, false otherwise.
     */
    bool is_enabled(ChannelId id) const;

    /**
     * @brief Checks if a channel is selected.
     *
     * @param id Index of the channel to check.
     * @return True if the channel is selected, false otherwise.
     */
    bool is_selected(ChannelId id) const;

    /**
     * @brief Gets the currently selected channel.
     *
     * @return Optional containing the selected channel ID if one is selected, std::nullopt
     * otherwise.
     */
    [[nodiscard]] std::optional<ChannelId> get_selected() const;

private:
    std::vector<ChannelState> m_channels; /**< List of channels. */

    std::optional<ChannelId> m_selected_channel = std::nullopt; /**< Currently selected channel. */
};
