#include "timebase_model.hpp"

#include "input_conversion.hpp"

TimebaseModel::TimebaseModel(QObject *parent)
    : QObject{ parent }
{
}

UData::Time::Duration TimebaseModel::division() const
{
    return m_div;
}

QString TimebaseModel::hScaleText() const
{
    return InputConversion::unit_scale_to_string(UData::duration_to_microseconds(m_div), u"s");
}

int TimebaseModel::hGridCells() const
{
    return default_grid_cells;
}

int TimebaseModel::qDialValue() const
{
    return InputConversion::div_uval_to_qdial_value(UData::duration_to_microseconds(m_div));
}

void TimebaseModel::qDialValueUpdate(int dial_value)
{
    auto new_div =
            UData::duration_from_microseconds(InputConversion::qdial_value_to_div_uval(dial_value));

    if (m_div != new_div) {
        m_div = new_div;

        emit hScaleTextChanged();
        emit qDialValueChanged();
        emit hDivisionChanged();
    }
}

UData::Time::Duration TimebaseModel::frame_width() const
{
    return m_div * default_grid_cells;
}
