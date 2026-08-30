#pragma once

#include <QObject>
#include <QStringList>
#include <QtQmlIntegration/qqmlintegration.h>
#include <memory>

struct UniversalReaderDialogConfig;

/**
 * @brief QML-facing session model backing the serial source configuration dialog.
 */
class SerialReaderDialogModel : public QObject
{
    Q_OBJECT
    QML_ELEMENT

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    Q_PROPERTY(QStringList availablePorts READ availablePorts CONSTANT)
    Q_PROPERTY(QString portName MEMBER m_port_name NOTIFY portNameChanged)
    Q_PROPERTY(int baudRate MEMBER m_baud_rate NOTIFY baudRateChanged)
#endif // DOXYGEN_SHOULD_SKIP_THIS

public:
    /**
     * @brief Constructor for the SerialReaderDialogModel.
     *
     * @param parent The parent QObject, default is nullptr.
     */
    explicit SerialReaderDialogModel(QObject *parent = nullptr);

    /**
     * @brief Returns a list of available serial ports on the system.
     *
     * @return List of available serial port names.
     */
    [[nodiscard]] QStringList availablePorts() const;

    /**
     * @brief Build a reader configuration from the model's current property values.
     *
     * @return New SerialReaderDialogConfig.
     */
    [[nodiscard]] std::shared_ptr<UniversalReaderDialogConfig> build_config() const;

signals:
    /**
     * @brief Signal emitted when the port name changes.
     */
    void portNameChanged();

    /**
     * @brief Signal emitted when the baud rate changes.
     */
    void baudRateChanged();

private:
    QString m_port_name{ }; /**< Name of the port. */
    int m_baud_rate{ }; /**< Baud rate of the interface. */
};
