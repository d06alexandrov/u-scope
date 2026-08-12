#include "timebase_model.h"

#include "input_conversion.hpp"

TimebaseModel::TimebaseModel(QObject *parent)
    : QObject{ parent }
{
}

QString TimebaseModel::hScaleText() const
{
    return InputConversion::unit_scale_to_string(m_div_us, u"s");
}

int TimebaseModel::hGridCells() const
{
    return default_grid_cells;
}

int TimebaseModel::qDialValue() const
{
    return InputConversion::div_uval_to_qdial_value(m_div_us);
}

int64_t TimebaseModel::divisionUs() const
{
    return m_div_us;
}

QBindable<int64_t> TimebaseModel::bindableDivisionUs()
{
    return &m_div_us;
}

int64_t TimebaseModel::frameWidthUs() const
{
    return m_div_us * default_grid_cells;
}

void TimebaseModel::dial_value_updated(int dial_value)
{
    int64_t new_div_us = InputConversion::qdial_value_to_div_uval(dial_value);

    if (m_div_us != new_div_us) {
        m_div_us = new_div_us;

        emit hScaleTextChanged();
        emit hGridCellsChanged();
    }
}
