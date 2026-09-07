#pragma once

#include "packetfields_dialog_model.hpp"

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
    Q_PROPERTY(QString formatMode MEMBER m_format_mode NOTIFY formatModeChanged)
    Q_PROPERTY(bool signedByte MEMBER m_signed_byte NOTIFY signedByteChanged)
    Q_PROPERTY(QString startMagicHex MEMBER m_start_magic_hex NOTIFY startMagicHexChanged)
    Q_PROPERTY(QString endMagicHex MEMBER m_end_magic_hex NOTIFY endMagicHexChanged)
    Q_PROPERTY(int packetLength MEMBER m_packet_length NOTIFY packetLengthChanged)
    Q_PROPERTY(PacketFieldsModel *fields READ fields CONSTANT)
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
     * @brief Returns the model for the fields in the packet.
     *
     * @return Pointer to the PacketFieldsModel.
     */
    [[nodiscard]] PacketFieldsModel *fields() const { return m_fields; }

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

    /**
     * @brief Signal emitted when the format mode changes.
     */
    void formatModeChanged();

    /**
     * @brief Signal emitted when the signed byte option changes.
     */
    void signedByteChanged();

    /**
     * @brief Signal emitted when the start magic hex string changes.
     */
    void startMagicHexChanged();

    /**
     * @brief Signal emitted when the end magic hex string changes.
     */
    void endMagicHexChanged();

    /**
     * @brief Signal emitted when the packet length changes.
     */
    void packetLengthChanged();

private:
    QString m_port_name{ }; /**< Name of the port. */
    int m_baud_rate{ }; /**< Baud rate of the interface. */
    QString m_format_mode{ }; /**< Format of the packet. */

    bool m_signed_byte{ true }; /**< Whether to treat single-byte packets as signed or unsigned. */

    QString m_start_magic_hex{ }; /**< Start of the packet in hex. */
    QString m_end_magic_hex{ }; /**< End of the packet in hex. Could be empty. */
    int m_packet_length{ }; /**< Total length including magics. */

    PacketFieldsModel *m_fields; /**< Model for the fields in the packet. */
};
