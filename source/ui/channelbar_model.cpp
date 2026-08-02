#include "channelbar_model.h"

ChannelBarModel::ChannelBarModel(const std::vector<QColor> channel_colors, QObject *parent)
    : QAbstractListModel(parent)
{
    for (int i = 0; i < channel_colors.size(); ++i) {
        m_channels.push_back({ i + 1, "", channel_colors[i], false });
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
    case ChannelEnabledRole:
        return ch.enabled;
    default:
        return { };
    }
}

QHash<int, QByteArray> ChannelBarModel::roleNames() const
{
    return { { ChannelNumberRole, "channelNumber" },
             { ValueTextRole, "valueText" },
             { BadgeColorRole, "badgeColor" },
             { ChannelEnabledRole, "channelEnabled" } };
}

void ChannelBarModel::setChannelEnabled(int channel_idx, bool enabled, const QString &value_text)
{
    if (channel_idx < 0 || channel_idx >= m_channels.size())
        return;

    m_channels[channel_idx].enabled = enabled;
    m_channels[channel_idx].value_text = value_text;
    QModelIndex idx = createIndex(channel_idx, 0);
    emit dataChanged(idx, idx, { ValueTextRole, ChannelEnabledRole });
}
