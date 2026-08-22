#pragma once

#include "udata_time.hpp"

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
using Point = QPair<Time, Variant>; /**< Combination of the timestamp and the value provided by
                                       universal readers. */

} // namespace UData

using ReaderId = uint64_t; /**< ID of the reader. */
using VariableId = uint64_t; /**< ID of the variable. */
using ChannelId = uint8_t; /**< Id of the channel starting with 0. */

Q_DECLARE_METATYPE(ReaderId)
Q_DECLARE_METATYPE(ChannelId)
