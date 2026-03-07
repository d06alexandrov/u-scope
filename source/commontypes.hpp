#pragma once

#include <QMetaType>
#include <QPair>
#include <cstdint>
#include <variant>

using DataVariant =
        std::variant<char, int32_t, double>; /**< Data variants provided by universal readers. */
using DataTime = uint64_t; /**< Timestamp format provided by universal readers. */
using DataPoint = QPair<DataTime, DataVariant>; /**< Combination of the timestamp and the value
                                                   provided by universal readers. */

using ReaderId = uint64_t; /**< ID of the reader. */
Q_DECLARE_METATYPE(ReaderId);

using VariableId = uint64_t; /**< ID of the variable. */
