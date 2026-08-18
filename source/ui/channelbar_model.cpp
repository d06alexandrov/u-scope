#include "channelbar_model.h"

ChannelBarModel::ChannelBarModel(const std::vector<QColor> channel_colors, QObject *parent)
    : QAbstractListModel(parent)
{
    m_channels.reserve(channel_colors.size());

    for (auto &color : channel_colors) {
        m_channels.push_back({ color, "", false, false, false });
    }
}

int ChannelBarModel::rowCount(const QModelIndex &) const
{
    return static_cast<int>(m_channels.size());
}

QVariant ChannelBarModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_channels.size())
        return { };

    const auto &ch = m_channels[index.row()];

    switch (role) {
    case ChannelIdRole:
        return index.row();
    case ChannelNumberRole:
        return index.row() + 1;
    case ValueTextRole:
        return ch.value_text;
    case BadgeColorRole:
        return ch.color;
    case ChannelConnected:
        return ch.connected;
    case ChannelEnabledRole:
        return ch.enabled;
    case ChannelSelectedRole:
        return ch.selected;
    default:
        return { };
    }
}

QHash<int, QByteArray> ChannelBarModel::roleNames() const
{
    return {
        { ChannelIdRole, "channelId" },
        { ChannelNumberRole, "channelNumber" },
        { ValueTextRole, "valueText" },
        { BadgeColorRole, "badgeColor" },
        { ChannelConnected, "channelConnected" },
        { ChannelEnabledRole, "channelEnabled" },
        { ChannelSelectedRole, "channelSelected" },
    };
}

int ChannelBarModel::selectedChannel() const
{
    return m_selected_channel.has_value() ? static_cast<int>(m_selected_channel.value()) : -1;
}

void ChannelBarModel::connect_channel(ChannelId id)
{
    if (id >= m_channels.size()) {
        return;
    }

    if (m_channels[id].connected) {
        m_channels[id].enabled = false;

        QModelIndex idx = createIndex(id, 0);
        emit dataChanged(idx, idx, { ChannelConnected, ChannelEnabledRole });
    } else {
        m_channels[id].connected = true;
        m_channels[id].enabled = false;

        if (m_channels[id].selected) {
            m_channels[id].selected = false;
            m_selected_channel.reset();

            emit selectedChannelChanged(selectedChannel());
        }

        QModelIndex idx = createIndex(id, 0);
        emit dataChanged(idx, idx, { ChannelConnected, ChannelEnabledRole, ChannelSelectedRole });
    }
}

void ChannelBarModel::disconnect_channel(ChannelId id)
{
    if (id >= m_channels.size()) {
        return;
    }

    if (m_channels[id].connected) {
        m_channels[id].connected = false;
        m_channels[id].enabled = false;

        if (m_channels[id].selected) {
            m_channels[id].selected = false;
            m_selected_channel.reset();

            emit selectedChannelChanged(selectedChannel());
        }

        QModelIndex idx = createIndex(id, 0);
        emit dataChanged(idx, idx, { ChannelConnected, ChannelEnabledRole, ChannelSelectedRole });
    }
}

void ChannelBarModel::select_channel(ChannelId id)
{
    if (id >= m_channels.size()) {
        return;
    }

    if (m_channels[id].connected) {
        if (m_selected_channel.has_value()) {
            ChannelId previous_channel_idx = m_selected_channel.value();

            if (previous_channel_idx == id) {
                return;
            }

            m_channels[previous_channel_idx].selected = false;
            QModelIndex previous_idx = createIndex(previous_channel_idx, 0);
            emit dataChanged(previous_idx, previous_idx, { ChannelSelectedRole });
        }

        m_channels[id].selected = true;
        m_selected_channel = id;
        emit selectedChannelChanged(id);

        QModelIndex idx = createIndex(id, 0);
        emit dataChanged(idx, idx, { ChannelSelectedRole });
    }
}

void ChannelBarModel::enable_channel(ChannelId id, const QString &value_text)
{
    if (id >= m_channels.size()) {
        return;
    }

    if (m_channels[id].connected) {
        m_channels[id].value_text = value_text;
        m_channels[id].enabled = true;

        QModelIndex idx = createIndex(id, 0);
        emit dataChanged(idx, idx, { ValueTextRole, ChannelEnabledRole });
    }
}

void ChannelBarModel::disable_channel(ChannelId id)
{
    if (id >= m_channels.size()) {
        return;
    }

    if (m_channels[id].connected) {
        m_channels[id].value_text.clear();
        m_channels[id].enabled = false;

        QModelIndex idx = createIndex(id, 0);
        emit dataChanged(idx, idx, { ValueTextRole, ChannelEnabledRole });
    }
}

void ChannelBarModel::set_channel_text(ChannelId id, const QString &value_text)
{
    if (id >= m_channels.size()) {
        return;
    }

    if (m_channels[id].connected) {
        m_channels[id].value_text = value_text;

        QModelIndex idx = createIndex(id, 0);
        emit dataChanged(idx, idx, { ValueTextRole });
    }
}

bool ChannelBarModel::is_enabled(ChannelId id) const
{
    if (id >= m_channels.size()) {
        return false;
    }

    return m_channels[id].enabled;
}

bool ChannelBarModel::is_selected(ChannelId id) const
{
    if (id >= m_channels.size()) {
        return false;
    }

    return m_channels[id].selected;
}

std::optional<ChannelId> ChannelBarModel::get_selected() const
{
    return m_selected_channel;
}
