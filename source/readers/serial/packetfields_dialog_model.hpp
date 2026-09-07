#pragma once

#include "commontypes.hpp"
#include "serialreader.hpp"

#include <QAbstractListModel>
#include <QSet>
#include <QtQmlIntegration/qqmlintegration.h>
#include <vector>

/**
 * @brief QML-facing model for the list of fields in a fixed-layout packet.
 */
class PacketFieldsModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ANONYMOUS

public:
    /**
     * @brief Roles for the packet fields model.
     */
    enum Roles {
        VariableIdRole = Qt::UserRole + 1, /**< Role for the variable ID. */
        LabelRole, /**< Role for the variable label. */
        NameRole, /**< Role for the field name. */
        OffsetRole, /**< Role for the byte offset within the packet. */
        TypeRole, /**< Role for the field type, as a string (e.g. "Int16"). */
        EndiannessRole, /**< Role for the endianness, as a string ("Little"/"Big"). */
    };

    /**
     * @brief Constructor for the PacketFieldsModel.
     *
     * @param parent The parent QObject, default is nullptr.
     */
    explicit PacketFieldsModel(QObject *parent = nullptr);

    /**
     * @brief Returns the number of fields in the model.
     *
     * @param parent Parent index.
     * @return Number of fields.
     */
    [[nodiscard]] int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    /**
     * @brief Returns the data for a given index and role.
     *
     * @param index Index of the field to retrieve data for.
     * @param role Role to retrieve data for.
     * @return Data for the given index and role.
     */
    [[nodiscard]] QVariant data(const QModelIndex &index,
                                int role = Qt::DisplayRole) const override;

    /**
     * @brief Returns the role names for the model.
     *
     * @return Hash of role names.
     */
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    /**
     * @brief Add a new field to the end of the list.
     *
     * @param name Field name.
     * @param offset Byte offset within the packet.
     * @param type Field type, as a string.
     * @param endianness Endianness, as a string.
     */
    Q_INVOKABLE void addField(const QString &name, int offset, const QString &type,
                              const QString &endianness);

    /**
     * @brief Modify an existing field in the list.
     *
     * @param variable_id Variable ID of the field to modify.
     * @param name New field name.
     * @param offset New byte offset within the packet.
     * @param type New field type, as a string.
     * @param endianness New endianness, as a string.
     */
    Q_INVOKABLE void modifyField(int variable_id, const QString &name, int offset,
                                 const QString &type, const QString &endianness);

    /**
     * @brief Remove an existing field from the list.
     *
     * @param variable_id Variable ID of the field to remove.
     */
    Q_INVOKABLE void removeField(int variable_id);

    /**
     * @brief Get a field's current parameters, for pre-filling the edit popup.
     *
     * @param variable_id Variable ID of the field to retrieve.
     * @return Map with "name", "offset", "type", and "endianness" keys.
     */
    [[nodiscard]] Q_INVOKABLE QVariantMap fieldAt(int variable_id) const;

    /**
     * @brief Get the current field configurations.
     *
     * @return Configurations of the fields.
     */
    [[nodiscard]] const QHash<VariableId, SerialReaderConfig::FieldConfig> &field_configs() const
    {
        return m_field_configs;
    }

private:
    QSet<VariableId> m_original_variable_ids{ }; /**< Set of original variable IDs. */
    QSet<VariableId> m_current_variable_ids{ }; /**< Set of currently used variable IDs. */
    QHash<VariableId, SerialReaderConfig::FieldConfig>
            m_field_configs{ }; /**< Configurations of the fields. */
    QList<VariableId> m_order{ }; /**< Order of the fields in the displayed list. */

    /**
     * @brief Get the size in bytes of a given field type.
     *
     * @param type Field type.
     * @return Size in bytes.
     */
    [[nodiscard]] static int type_size(SerialReaderConfig::FieldType type);

    /**
     * @brief Convert between field type and string representation.
     *
     * @param type Field type.
     * @return String representation of the field type.
     */
    [[nodiscard]] static QString type_to_string(SerialReaderConfig::FieldType type);

    /**
     * @brief Convert between string representation and field type.
     *
     * @param str String representation of the field type.
     * @return Field type.
     */
    [[nodiscard]] static SerialReaderConfig::FieldType type_from_string(const QString &str);

    /**
     * @brief Convert between endianness and string representation.
     *
     * @param endianness Endianness.
     * @return String representation of the endianness.
     */
    [[nodiscard]] static QString endianness_to_string(SerialReaderConfig::Endianness endianness);

    /**
     * @brief Convert between string representation and endianness.
     *
     * @param str String representation of the endianness.
     * @return Endianness.
     */
    [[nodiscard]] static SerialReaderConfig::Endianness endianness_from_string(const QString &str);

    /**
     * @brief Build a label for a field, combining its name, type, and endianness.
     *
     * @param field Field configuration.
     * @return Label string.
     */
    [[nodiscard]] static QString build_label(const SerialReaderConfig::FieldConfig &field);
};
