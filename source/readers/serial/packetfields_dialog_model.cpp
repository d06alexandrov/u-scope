#include "packetfields_dialog_model.hpp"

#include "id_allocator.hpp"

PacketFieldsModel::PacketFieldsModel(QObject *parent)
    : QAbstractListModel{ parent }
{
}

int PacketFieldsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return static_cast<int>(m_order.size());
}

QVariant PacketFieldsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= static_cast<int>(m_order.size())) {
        return { };
    }

    const VariableId variable_id = m_order.at(index.row());

    const auto &field = m_field_configs.value(variable_id);

    switch (role) {
    case VariableIdRole:
        return static_cast<int>(variable_id);
    case LabelRole:
        return build_label(field);
    case NameRole:
        return field.name;
    case OffsetRole:
        return field.offset;
    case TypeRole:
        return type_to_string(field.type);
    case EndiannessRole:
        return endianness_to_string(field.endianness);
    default:
        return { };
    }
}

QHash<int, QByteArray> PacketFieldsModel::roleNames() const
{
    return {
        { VariableIdRole, "variableId" }, { LabelRole, "label" }, { NameRole, "name" },
        { OffsetRole, "offset" },         { TypeRole, "type" },   { EndiannessRole, "endianness" },
    };
}

void PacketFieldsModel::addField(const QString &name, int offset, const QString &type,
                                 const QString &endianness)
{
    const auto new_id =
            UData::get_available_id<VariableId>(m_original_variable_ids + m_current_variable_ids);

    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_current_variable_ids.insert(new_id);
    m_field_configs[new_id] = SerialReaderConfig::FieldConfig{
        .name = name,
        .offset = offset,
        .type = type_from_string(type),
        .endianness = endianness_from_string(endianness),
    };
    m_order.append(new_id);
    endInsertRows();
}

void PacketFieldsModel::modifyField(int variable_id, const QString &name, int offset,
                                    const QString &type, const QString &endianness)
{
    if (variable_id < 0) {
        return;
    }

    const auto id = static_cast<VariableId>(variable_id);

    if (!m_field_configs.contains(id)) {
        return;
    }

    auto &field = m_field_configs[id];

    field.name = name;
    field.offset = offset;
    field.type = type_from_string(type);
    field.endianness = endianness_from_string(endianness);

    if (const auto row = static_cast<int>(m_order.indexOf(id)); row >= 0) {
        const QModelIndex idx = index(row);
        emit dataChanged(idx, idx, { LabelRole });
    }
}

void PacketFieldsModel::removeField(int variable_id)
{
    if (variable_id < 0) {
        return;
    }

    const auto id = static_cast<VariableId>(variable_id);
    const int row = static_cast<int>(m_order.indexOf(id));

    if (row < 0) {
        return;
    }

    beginRemoveRows(QModelIndex(), row, row);
    m_order.removeAt(row);
    m_field_configs.remove(id);
    m_current_variable_ids.remove(id);
    endRemoveRows();
}

QVariantMap PacketFieldsModel::fieldAt(int variable_id) const
{
    if (variable_id < 0) {
        return { };
    }

    const auto id = static_cast<VariableId>(variable_id);

    if (!m_field_configs.contains(id)) {
        return { };
    }

    const auto &field = m_field_configs[id];

    return QVariantMap{
        { "name", field.name },
        { "offset", field.offset },
        { "type", type_to_string(field.type) },
        { "endianness", endianness_to_string(field.endianness) },
    };
}

int PacketFieldsModel::type_size(SerialReaderConfig::FieldType type)
{
    switch (type) {
    case SerialReaderConfig::FieldType::Int8:
    case SerialReaderConfig::FieldType::UInt8:
        return 1;
    case SerialReaderConfig::FieldType::Int16:
    case SerialReaderConfig::FieldType::UInt16:
        return 2;
    case SerialReaderConfig::FieldType::Int32:
    case SerialReaderConfig::FieldType::UInt32:
        return 4;
    default:
        return 0;
    }
}

QString PacketFieldsModel::type_to_string(SerialReaderConfig::FieldType type)
{
    switch (type) {
    case SerialReaderConfig::FieldType::Int8:
        return QStringLiteral("Int8");
    case SerialReaderConfig::FieldType::UInt8:
        return QStringLiteral("UInt8");
    case SerialReaderConfig::FieldType::Int16:
        return QStringLiteral("Int16");
    case SerialReaderConfig::FieldType::UInt16:
        return QStringLiteral("UInt16");
    case SerialReaderConfig::FieldType::Int32:
        return QStringLiteral("Int32");
    case SerialReaderConfig::FieldType::UInt32:
        return QStringLiteral("UInt32");
    default:
        return QStringLiteral("Int8");
    }
}

SerialReaderConfig::FieldType PacketFieldsModel::type_from_string(const QString &str)
{
    if (str == "Int8") {
        return SerialReaderConfig::FieldType::Int8;
    }
    if (str == "UInt8") {
        return SerialReaderConfig::FieldType::UInt8;
    }
    if (str == "Int16") {
        return SerialReaderConfig::FieldType::Int16;
    }
    if (str == "UInt16") {
        return SerialReaderConfig::FieldType::UInt16;
    }
    if (str == "Int32") {
        return SerialReaderConfig::FieldType::Int32;
    }
    if (str == "UInt32") {
        return SerialReaderConfig::FieldType::UInt32;
    }

    return SerialReaderConfig::FieldType::Int8;
}

QString PacketFieldsModel::endianness_to_string(SerialReaderConfig::Endianness endianness)
{
    return (endianness == SerialReaderConfig::Endianness::Big) ? QStringLiteral("Big")
                                                               : QStringLiteral("Little");
}

SerialReaderConfig::Endianness PacketFieldsModel::endianness_from_string(const QString &str)
{
    return (str == "Big") ? SerialReaderConfig::Endianness::Big
                          : SerialReaderConfig::Endianness::Little;
}

QString PacketFieldsModel::build_label(const SerialReaderConfig::FieldConfig &field)
{
    const QString endian_abbrev = (field.endianness == SerialReaderConfig::Endianness::Big)
            ? QStringLiteral("BE")
            : QStringLiteral("LE");
    const bool needs_endianness = type_size(field.type) > 1;

    return needs_endianness ? QStringLiteral("%1 @%2 (%3, %4)")
                                      .arg(field.name)
                                      .arg(field.offset)
                                      .arg(type_to_string(field.type), endian_abbrev)
                            : QStringLiteral("%1 @%2 (%3)")
                                      .arg(field.name)
                                      .arg(field.offset)
                                      .arg(type_to_string(field.type));
}
