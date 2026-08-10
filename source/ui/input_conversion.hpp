#pragma once

#include <QObject>
#include <QString>
#include <QStringView>
#include <algorithm>
#include <array>

/**
 * @brief Powers of 10.
 */
inline constexpr std::array<int64_t, 9> powers_of_10 = {
    1LL, 10LL, 100LL, 1000LL, 10000LL, 100000LL, 1000000LL, 10000000LL, 100000000LL,
};

/**
 * @brief Convert horizontal division in us to a string representation.
 *
 * @param us Horizontal division in us.
 * @return String representation of the horizontal division.
 */
inline QString horizontal_scale_to_string(int64_t us);

/**
 * @brief Convert division to QDial value.
 *
 * 10^-x division is converted into x.
 *
 * @param division_u Vertical division in 10^-6.
 * @return QDial value.
 */
inline int div_uval_to_qdial_value(int64_t division_u)
{
    if (division_u <= 0) {
        // It should never happen
        return 0;
    }

    int log10 = 0;

    // Take log10 and round up
    for (int64_t value_left = division_u - 1; value_left > 0; value_left /= 10) {
        log10++;
    }

    return 6 - log10;
}

/**
 * @brief Convert QDial value to vertical division in micro values.
 *
 * X is converted into 10^-x seconds.
 *
 * @param value QDial value.
 * @return Vertical division in 10^-6.
 */
inline int64_t qdial_value_to_div_uval(int value)
{
    return powers_of_10.at(std::clamp(6 - value, 0, static_cast<int>(powers_of_10.size() - 1)));
}

/**
 * @brief Convert vertical division to a string representation.
 *
 * @param division Horizontal division in 10^-6.
 * @return String representation of the vertical division.
 */
inline QString unit_scale_to_string(int64_t division, QStringView unit = u"")
{
    if (division < 1000LL) {
        return QObject::tr("%1 u%2").arg(division).arg(unit);
    } else if (division < 1000000LL) {
        return QObject::tr("%1 m%2").arg(static_cast<double>(division) / 1000.0).arg(unit);
    } else {
        return QObject::tr("%1 %2").arg(static_cast<double>(division) / 1000000.0).arg(unit);
    }
}
