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

/**
 * @brief Dialog for configuring a serial reader.
 */
class SerialReaderDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Constructor of the dialog window.
     *
     * @param parent Parent widget.
     */
    SerialReaderDialog(QWidget *parent = nullptr);

    SerialReaderDialog(const SerialReaderDialog &other) = delete;
    SerialReaderDialog(SerialReaderDialog &&other) = delete;

    /**
     * @brief Constructor of the dialog window.
     */
    ~SerialReaderDialog() override;

    SerialReaderDialog &operator=(const SerialReaderDialog &other) = delete;
    SerialReaderDialog &operator=(SerialReaderDialog &&other) = delete;

    /**
     * @brief Get configuration of the serial reader.
     *
     * @return Pointer to the configuration of the serial reader.
     */
    std::shared_ptr<UniversalReaderDialogConfig> get_config();

private:
    Ui::SerialReaderDialog *ui = nullptr; /**< Pointer to the user interface. */

    /**
     * @brief Update the list of devices.
     */
    void update_devices_list();

signals:
};
