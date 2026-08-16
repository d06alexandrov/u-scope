#include "sourcelist_controller.h"

#include "serialreader_dialog.h"
#include "simulatedreader_dialog.h"

SourceListController::SourceListController(QObject *parent)
    : QObject{ parent }
    , m_source_model(new QStandardItemModel(this))
{
    QHash<int, QByteArray> role_names = m_source_model->roleNames();

    role_names[ItemRoles::ReaderIdRole] = "readerId";
    role_names[ItemRoles::VariableIdRole] = "variableId";

    m_source_model->setItemRoleNames(role_names);
}

Q_INVOKABLE void SourceListController::open_simulated_source_dialog(QObject *parent_widget)
{
    auto *parent_window = qobject_cast<QWidget *>(parent_widget);
    SimulatedReaderDialog dialog(parent_window);

    if (dialog.exec() == QDialog::Accepted) {
        add_reader(dialog.get_config());
    }
}

Q_INVOKABLE void SourceListController::open_serial_source_dialog(QObject *parent_widget)
{
    auto *parent_window = qobject_cast<QWidget *>(parent_widget);
    SerialReaderDialog dialog(parent_window);

    if (dialog.exec() == QDialog::Accepted) {
        add_reader(dialog.get_config());
    }
}

Q_INVOKABLE void SourceListController::delete_source(int reader_id)
{
    for (int i = 0; i < m_source_model->rowCount(); ++i) {
        QStandardItem *item = m_source_model->item(i);

        if (item && (item->data(ItemRoles::ReaderIdRole).toInt() == reader_id)
            && !item->data(ItemRoles::VariableIdRole).isValid()) {
            m_source_model->removeRow(i);
            m_readers_config.remove(static_cast<ReaderId>(reader_id));

            emit request_reader_remove(static_cast<ReaderId>(reader_id));

            // TODO: emit signals to disconnect variables from channels
            break;
        }
    }
}

Q_INVOKABLE void SourceListController::assign_variable_to_channel(int reader_id, int variable_id,
                                                                  int channel_id)
{
    emit request_channel_assignment(static_cast<ReaderId>(reader_id), variable_id, channel_id);
}

ReaderId SourceListController::get_available_reader_idx() const
{
    ReaderId reader_id = 0;

    for (auto it = m_readers_config.lowerBound(reader_id); it != m_readers_config.end(); it++) {
        if (it.key() == reader_id) {
            reader_id++;
        } else if (it.key() > reader_id) {
            break;
        }
    }

    if (reader_id >= readers_max_amount) {
        throw std::range_error("Reader amount limit exceeded");
    }

    return reader_id;
}

void SourceListController::add_reader(const std::shared_ptr<UniversalReaderDialogConfig> &config)
{
    ReaderId new_reader_id = get_available_reader_idx();

    m_readers_config.insert(new_reader_id, config);

    auto *new_item = new QStandardItem(tr("Source %1").arg(new_reader_id));
    new_item->setData(QVariant::fromValue(new_reader_id), ItemRoles::ReaderIdRole);

    m_source_model->appendRow(new_item);

    if (!config->variable_names.isEmpty()) {
        for (const auto [id, name] : config->variable_names.asKeyValueRange()) {
            auto *new_variable_item = new QStandardItem(tr("#%1 %2").arg(id).arg(name));
            new_variable_item->setData(QVariant::fromValue(new_reader_id), ItemRoles::ReaderIdRole);
            new_variable_item->setData(QVariant::fromValue(id), ItemRoles::VariableIdRole);
            new_item->appendRow(new_variable_item);
        }
    }

    emit configure_reader(new_reader_id, config);
}
