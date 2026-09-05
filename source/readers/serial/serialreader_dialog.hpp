#pragma once

#include "serialreader.hpp"
#include "universalreader_dialog.hpp"

#include <QString>

/**
 * @brief Configuration for the @ref SerialReader.
 */
struct SerialReaderDialogConfig : UniversalReaderDialogConfig
{
    QString port_name{ }; /**< Name of the port. */
    int32_t baud_rate{ }; /**< Baud rate of the interface. */

    [[nodiscard]] std::shared_ptr<UniversalReaderConfig> to_reader_config() const override;
};
