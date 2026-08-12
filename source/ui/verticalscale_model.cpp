#include "verticalscale_model.h"

#include "input_conversion.hpp"

#include <ratio>

VerticalScaleModel::VerticalScaleModel(size_t channelCount, QObject *parent)
    : QObject{ parent }
    , m_channel_scales_uval(std::vector<int64_t>(channelCount, default_division_uval))
{
}

int VerticalScaleModel::vGridCells() const
{
    return default_grid_cells;
}

int VerticalScaleModel::qDialValue(ChannelId id) const
{
    if (id >= m_channel_scales_uval.size()) {
        return InputConversion::div_uval_to_qdial_value(default_division_uval);
    }

    return InputConversion::div_uval_to_qdial_value(m_channel_scales_uval.at(id));
}

qreal VerticalScaleModel::vScaleFactor(ChannelId id) const
{
    if (id >= m_channel_scales_uval.size()) {
        return std::micro::den / (std::micro::num * static_cast<qreal>(default_division_uval));
    }

    return std::micro::den / (std::micro::num * static_cast<qreal>(m_channel_scales_uval.at(id)));
}

QString VerticalScaleModel::vScaleText(ChannelId id) const
{
    if (id >= m_channel_scales_uval.size()) {
        return InputConversion::unit_scale_to_string(default_division_uval);
    }

    return InputConversion::unit_scale_to_string(m_channel_scales_uval.at(id));
}

void VerticalScaleModel::dial_value_updated(ChannelId id, int dial_value)
{
    if (id >= m_channel_scales_uval.size()) {
        return;
    }

    m_channel_scales_uval.at(id) = InputConversion::qdial_value_to_div_uval(dial_value);

    emit vDivisionChanged();
}

void VerticalScaleModel::reset_channel(ChannelId id)
{
    if (id >= m_channel_scales_uval.size()) {
        return;
    }

    m_channel_scales_uval.at(id) = default_division_uval;

    emit vDivisionChanged();
}