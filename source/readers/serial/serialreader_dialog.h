#pragma once

#include "serialreader.h"
#include "universalreader_dialog.h"

#include <QDialog>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui {
class SerialReaderDialog;
}
QT_END_NAMESPACE

/**
 * @brief Configuration for the @ref SerialReader.
 */
struct SerialReaderDialogConfig : UniversalReaderDialogConfig
{
    QString port_name{ }; /**< Name of the port. */
    int32_t baud_rate{ }; /**< Baud rate of the interface. */

    [[nodiscard]] std::shared_ptr<UniversalReaderConfig> to_reader_config() const override;
};
