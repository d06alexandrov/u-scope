#pragma once

#include <QMetaType>
#include <chrono>
#include <compare>
#include <cstdint>

namespace UData {

/**
 * @brief Represents a timestamp since an epoch, with nanosecond precision.
 */
class Time
{
public:
    using Rep = int64_t; /**< The underlying representation type for the timestamp. */
    using Duration = std::chrono::duration<Rep, std::nano>; /**< The duration type representing the
                                                               time since epoch. */

    /**
     * @brief Default constructor for Time, initializes to epoch (0).
     */
    constexpr Time() noexcept = default;

    /**
     * @brief Construct a Time object from a duration since epoch.
     *
     * @param d The duration since epoch.
     */
    constexpr explicit Time(Duration d) noexcept
        : m_since_epoch(d)
    {
    }

    /**
     * @brief Create a Time object from a duration in nanoseconds.
     *
     * @param ns The duration in nanoseconds.
     * @return A Time object representing the specified duration since epoch.
     */
    [[nodiscard]] static constexpr Time from_nanoseconds(Rep ns) noexcept
    {
        return Time(Duration(ns));
    }

    /**
     * @brief Create a Time object from a duration in microseconds.
     *
     * @param us The duration in microseconds.
     * @return A Time object representing the specified duration since epoch.
     */
    [[nodiscard]] static constexpr Time from_microseconds(int64_t us) noexcept
    {
        return Time(std::chrono::duration_cast<Duration>(std::chrono::microseconds(us)));
    }

    /**
     * @brief Create a Time object for the current time.
     *
     * @note This function uses a monotonic clock, so every new call returns a higher value.
     *
     * @return A Time object representing the current time since epoch.
     */
    [[nodiscard]] static Time now() noexcept
    {
        return Time(std::chrono::duration_cast<Duration>(
                std::chrono::steady_clock::now().time_since_epoch()));
    }

    /**
     * @brief Get the timestamp in microseconds since epoch.
     *
     * @return The timestamp in microseconds since epoch.
     */
    [[nodiscard]] constexpr int64_t to_microseconds() const noexcept
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(m_since_epoch).count();
    }

    /**
     * @brief Get the timestamp in nanoseconds since epoch.
     *
     * @return The timestamp in nanoseconds since epoch.
     */
    [[nodiscard]] constexpr int64_t to_nanoseconds() const noexcept
    {
        return m_since_epoch.count();
    }

    /**
     * @brief Comparison operator for Time objects.
     *
     * @param lhs The left-hand side Time object.
     * @param rhs The right-hand side Time object.
     * @return True if the comparison holds, false otherwise.
     */
    friend constexpr auto operator<=>(const Time &lhs, const Time &rhs) noexcept = default;

    /**
     * @brief Increment the Time object by a specified duration.
     *
     * @param d The duration to add to the Time object.
     * @return A reference to the modified Time object.
     */
    constexpr Time &operator+=(Duration d) noexcept
    {
        m_since_epoch += d;
        return *this;
    }

    /**
     * @brief Decrement the Time object by a specified duration.
     *
     * @param d The duration to subtract from the Time object.
     * @return A reference to the modified Time object.
     */
    constexpr Time &operator-=(Duration d) noexcept
    {
        m_since_epoch -= d;
        return *this;
    }

    /**
     * @brief Add a duration to a Time object.
     *
     * @param t The Time object to which the duration will be added.
     * @param d The duration to add.
     * @return A new Time object representing the result of the addition.
     */
    friend constexpr Time operator+(Time t, Duration d) noexcept { return t += d; }

    /**
     * @brief Add a duration to a Time object (commutative).
     *
     * @param d The duration to add.
     * @param t The Time object to which the duration will be added.
     * @return A new Time object representing the result of the addition.
     */
    friend constexpr Time operator+(Duration d, Time t) noexcept { return t += d; }

    /**
     * @brief Subtract a duration from a Time object.
     *
     * @param t The Time object from which the duration will be subtracted.
     * @param d The duration to subtract.
     * @return A new Time object representing the result of the subtraction.
     */
    friend constexpr Time operator-(Time t, Duration d) noexcept { return t -= d; }

    /**
     * @brief Subtract one Time object from another to get the duration between them.
     *
     * @param lhs The left-hand side Time object.
     * @param rhs The right-hand side Time object.
     * @return The duration between the two Time objects as a Time::Duration object.
     */
    friend constexpr Duration operator-(Time lhs, Time rhs) noexcept
    {
        return lhs.m_since_epoch - rhs.m_since_epoch;
    }

private:
    Duration m_since_epoch{ }; /**< The duration since epoch represented by this Time object. */
};

/**
 * @brief Create a Time::Duration from a count of microseconds.
 *
 * @param us The duration in microseconds.
 * @return A Time::Duration representing the specified span.
 */
[[nodiscard]] constexpr Time::Duration duration_from_microseconds(int64_t us) noexcept
{
    return std::chrono::duration_cast<Time::Duration>(std::chrono::microseconds(us));
}

/**
 * @brief Create a Time::Duration from a count of seconds.
 *
 * @param sec The duration in seconds.
 * @return A Time::Duration representing the specified span.
 */
[[nodiscard]] constexpr Time::Duration duration_from_seconds(double sec) noexcept
{
    return std::chrono::duration_cast<Time::Duration>(std::chrono::duration<double>(sec));
}

/**
 * @brief Convert a Time::Duration to a double representing seconds.
 *
 * @param d The duration to convert.
 * @return The duration in seconds as a double.
 */
[[nodiscard]] constexpr double to_double(Time::Duration d) noexcept
{
    return std::chrono::duration<double, std::ratio<1>>(d).count();
}

} // namespace UData

Q_DECLARE_METATYPE(UData::Time)
Q_DECLARE_METATYPE(UData::Time::Duration)
