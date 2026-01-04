#pragma once

#include <QPair>
#include <cstdint>
#include <variant>

using DataVariant =
        std::variant<char, int32_t, double>; /**< Data variants provided by universal readers. */
using DataTime = uint64_t; /**< Timestamp format provided by universal readers. */
using DataPoint = QPair<DataTime, DataVariant>; /**< Combination of the timestamp and the value
                                                   provided by universal readers. */
