#pragma once

#include <QMetaType>
#include <QPair>
#include <chrono>
#include <cstdint>
#include <variant>

template <class... Ts>
struct overloads : Ts...
{
    using Ts::operator()...;
};
template <class... Ts>
overloads(Ts...) -> overloads<Ts...>;

namespace UData {

using Variant =
        std::variant<char, int32_t, double>; /**< Data variants provided by universal readers. */
using Time = int64_t; /**< Timestamp format provided by universal readers. */
using Point = QPair<Time, Variant>; /**< Combination of the timestamp and the value provided by
                                       universal readers. */

/**
 * @brief Get current timestamp.
 *
 * @note This function uses monotonic clock, so every new call returns a higher value.
 *
 * @return Current time timestamp.
 */
inline Time get_timestamp()
{
    return std::chrono::duration_cast<std::chrono::microseconds>(
                   std::chrono::steady_clock::now().time_since_epoch())
            .count();
}

/**
 * @brief Get the difference between two timestamps.
 *
 * @param before Earlier timestamp.
 * @param after Timestamp after @p before.
 * @return Time in microseconds between @p before and @p after.
 */
inline int64_t get_timestamp_diff_us(Time before, Time after)
{
    return after - before;
}

/**
 * @brief Increase timestamp by provided time.
 *
 * @param timestamp Original timestamp.
 * @param us Time in microseconds that should be added to original timestamp.
 * @return @p timestamp plus @p us round up to the nearest Data::Time.
 */
inline Time timestamp_add_us_roundup(Time timestamp, int64_t us)
{
    return timestamp + us;
}

/**
 * @brief Decrease timestamp by provided time.
 *
 * @param timestamp Original timestamp.
 * @param us Time in microseconds that should be subtracted from original timestamp.
 * @return @p timestamp minus @p us round down to the nearest Data::Time.
 */
inline Time timestamp_sub_us_rounddown(Time timestamp, int64_t us)
{
    return timestamp - us;
}

} // namespace UData

using ReaderId = uint64_t; /**< ID of the reader. */
Q_DECLARE_METATYPE(ReaderId)

using VariableId = uint64_t; /**< ID of the variable. */

using ChannelId = uint8_t; /**< Id of the channel starting with 1. */
