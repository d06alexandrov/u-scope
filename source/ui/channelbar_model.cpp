#include "channelbar_model.h"

ChannelBarModel::ChannelBarModel(const std::vector<QColor> channel_colors, QObject *parent)
    : QAbstractListModel(parent)
{
    for (int i = 0; i < channel_colors.size(); ++i) {
        m_channels.push_back({ i + 1, channel_colors[i], "", false, false, false });
    }
}

int ChannelBarModel::rowCount(const QModelIndex &) const
{
    return m_channels.size();
}

QVariant ChannelBarModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_channels.size())
        return { };

    const auto &ch = m_channels[index.row()];
    switch (role) {
    case ChannelNumberRole:
        return ch.number;
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
        { ChannelNumberRole, "channelNumber" },   { ValueTextRole, "valueText" },
        { BadgeColorRole, "badgeColor" },         { ChannelConnected, "channelConnected" },
        { ChannelEnabledRole, "channelEnabled" }, { ChannelSelectedRole, "channelSelected" },
    };
}

void ChannelBarModel::connect_channel(ChannelId id)
{
    int channel_idx = static_cast<int>(id) - 1;

    if (channel_idx < 0 || channel_idx >= m_channels.size()) {
        return;
    }

    if (m_channels[channel_idx].connected) {
        m_channels[channel_idx].enabled = false;

        QModelIndex idx = createIndex(channel_idx, 0);
        emit dataChanged(idx, idx, { ChannelConnected, ChannelEnabledRole });
    } else {
        m_channels[channel_idx].connected = true;
        m_channels[channel_idx].enabled = false;

        if (m_channels[channel_idx].selected) {
            m_channels[channel_idx].selected = false;
            m_selected_channel.reset();
        }

        QModelIndex idx = createIndex(channel_idx, 0);
        emit dataChanged(idx, idx, { ChannelConnected, ChannelEnabledRole, ChannelSelectedRole });
    }
}

void ChannelBarModel::disconnect_channel(ChannelId id)
{
    int channel_idx = static_cast<int>(id) - 1;

    if (channel_idx < 0 || channel_idx >= m_channels.size()) {
        return;
    }

    if (m_channels[channel_idx].connected) {
        m_channels[channel_idx].connected = false;
        m_channels[channel_idx].enabled = false;

        if (m_channels[channel_idx].selected) {
            m_channels[channel_idx].selected = false;
            m_selected_channel.reset();
        }

        QModelIndex idx = createIndex(channel_idx, 0);
        emit dataChanged(idx, idx, { ChannelConnected, ChannelEnabledRole, ChannelSelectedRole });
    }
}

void ChannelBarModel::select_channel(ChannelId id)
{
    int channel_idx = static_cast<int>(id) - 1;

    if (channel_idx < 0 || channel_idx >= m_channels.size()) {
        return;
    }

    if (m_channels[channel_idx].connected) {
        if (m_selected_channel.has_value()) {
            int previous_channel_idx = static_cast<int>(m_selected_channel.value()) - 1;

            m_channels[previous_channel_idx].selected = false;
            QModelIndex previous_idx = createIndex(previous_channel_idx, 0);
            emit dataChanged(previous_idx, previous_idx, { ChannelSelectedRole });
        }

        m_channels[channel_idx].selected = true;
        m_selected_channel = id;

        QModelIndex idx = createIndex(channel_idx, 0);
        emit dataChanged(idx, idx, { ChannelSelectedRole });
    }
}

void ChannelBarModel::enable_channel(ChannelId id, const QString &value_text)
{
    int channel_idx = static_cast<int>(id) - 1;

    if (channel_idx < 0 || channel_idx >= m_channels.size()) {
        return;
    }

    if (m_channels[channel_idx].connected) {
        m_channels[channel_idx].value_text = value_text;
        m_channels[channel_idx].enabled = true;

        QModelIndex idx = createIndex(channel_idx, 0);
        emit dataChanged(idx, idx, { ValueTextRole, ChannelEnabledRole });
    }
}

void ChannelBarModel::disable_channel(ChannelId id)
{
    int channel_idx = static_cast<int>(id) - 1;

    if (channel_idx < 0 || channel_idx >= m_channels.size()) {
        return;
    }

    if (m_channels[channel_idx].connected) {
        m_channels[channel_idx].value_text.clear();
        m_channels[channel_idx].enabled = false;

        QModelIndex idx = createIndex(channel_idx, 0);
        emit dataChanged(idx, idx, { ValueTextRole, ChannelEnabledRole });
    }
}

bool ChannelBarModel::is_enabled(ChannelId id)
{
    int channel_idx = static_cast<int>(id) - 1;

    if (channel_idx < 0 || channel_idx >= m_channels.size()) {
        return false;
    }

    return m_channels[channel_idx].enabled;
}
