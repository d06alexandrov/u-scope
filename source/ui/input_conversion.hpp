#pragma once

#include <QObject>
#include <QString>
#include <QStringView>
#include <algorithm>
#include <array>
#include <cmath>
#include <ranges>
#include <ratio>

namespace InputConversion {

/**
 * @brief Scale threshold in micro values.
 *
 * The threshold is calculated as the ratio of the target scale to the input scale.
 */
template <typename TargetScale, typename InputScale = std::micro>
constexpr int64_t scale_threshold_uval = std::ratio_divide<TargetScale, InputScale>::num
        / std::ratio_divide<TargetScale, InputScale>::den;

/**
 * @brief Base steps (exponent) for division values.
 */
inline constexpr std::array<int64_t, 3> base_steps = { 1, 2, 5 };

/**
 * @brief Convert division to QDial value.
 *
 * Divisions 1, 2, 5, 10, 20, 50, ... are converted to 0, -1, -2, and so on.
 *
 * @param division_u Division in 10^-6.
 * @return QDial value.
 */
inline int div_uval_to_qdial_value(int64_t division_u)
{
    if (division_u <= 0) {
        // It should never happen
        return 0;
    }

    int qdial_val = 0;

    while (division_u >= 10) {
        division_u /= 10;
        qdial_val += base_steps.size();
    }

    if (auto it = std::ranges::lower_bound(base_steps, division_u); it != base_steps.end()) {
        qdial_val += std::distance(base_steps.begin(), it);
    } else {
        qdial_val += base_steps.size() - 1;
    }

    return -qdial_val;
}

/**
 * @brief Convert QDial value to division in micro values.
 *
 * Negative X is converted into a number from the sequence 1, 2, 5, 10, 20, ...
 * Where 0 is translated to 1, -1 is translated to 2, -2 is translated to 5, -3 is translated to 10,
 * and so on.
 *
 * @param value QDial value.
 * @return Division in 10^-6.
 */
inline int64_t qdial_value_to_div_uval(int dial_value)
{
    if (dial_value > 0) {
        return 1;
    }

    int inverted_value = -dial_value;

    return base_steps.at(inverted_value % base_steps.size())
            * static_cast<int64_t>(std::pow(10, inverted_value / base_steps.size()));
}

/**
 * @brief Convert division to a string representation.
 *
 * @param division Horizontal division in 10^-6.
 * @param unit Optional unit string to append to the result.
 * @return String representation of the division.
 */
inline QString unit_scale_to_string(int64_t division, QStringView unit = u"")
{
    if (division < scale_threshold_uval<std::milli>) {
        return QObject::tr("%1 u%2").arg(division).arg(unit);
    } else if (division < scale_threshold_uval<std::ratio<1>>) {
        return QObject::tr("%1 m%2").arg(static_cast<double>(division) / 1000.0).arg(unit);
    } else {
        return QObject::tr("%1 %2").arg(static_cast<double>(division) / 1000000.0).arg(unit);
    }
}

} // namespace InputConversion
