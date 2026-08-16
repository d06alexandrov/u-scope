#pragma once

#include "commontypes.hpp"

#include <QObject>
#include <QStandardItemModel>

struct UniversalReaderDialogConfig;

/**
 * @brief The class responsible for controlling the source list in the application.
 */
class SourceListController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QAbstractItemModel *model MEMBER m_source_model CONSTANT)

public:
    static constexpr ReaderId readers_max_amount = 10; /**< Maximum amount of readers. */

    /**
     * @brief Roles of the items in the source list.
     */
    enum ItemRoles {
        ReaderIdRole = Qt::UserRole + 1, /**< Role for the reader id. */
        VariableIdRole, /**< Role for the variable id. */
    };

    /**
     * @brief Constructor of SourceListController.
     *
     * @param parent Parent QObject, default is nullptr.
     */
    explicit SourceListController(QObject *parent = nullptr);

    Q_INVOKABLE void open_simulated_source_dialog(QObject *parent_widget);
    Q_INVOKABLE void open_serial_source_dialog(QObject *parent_widget);

    Q_INVOKABLE void delete_source(int reader_id);
    Q_INVOKABLE void assign_variable_to_channel(int reader_id, int variable_id, int channel_id);

public slots:

signals:

    /**
     * @brief Send reader configuration to the Data Processor.
     *
     * If the reader exists, it changes the config of it.
     *
     * @param reader_id ID of the reader.
     * @param config Configuration of the reader.
     */
    void configure_reader(ReaderId id, std::shared_ptr<UniversalReaderDialogConfig> config);

    /**
     * @brief Assign reader's variable to a channel.
     *
     * @param reader_id ID of the reader.
     * @param variable_id ID of the variable.
     * @param channel_id ID of the channel.
     */
    void request_channel_assignment(ReaderId reader_id, VariableId variable_id,
                                    ChannelId channel_id);

    /**
     * @brief Request to remove a reader from the Data Processor.
     *
     * @param reader_id ID of the reader to be removed.
     */
    void request_reader_remove(ReaderId reader_id);

protected:
private slots:

private:
    QMap<ReaderId, std::shared_ptr<UniversalReaderDialogConfig>>
            m_readers_config; /**< Readers
                                 configuration. */

    QStandardItemModel *m_source_model;

    /**
     * @brief Get available reader index.
     *
     * @return Available reader index.
     */
    ReaderId get_available_reader_idx() const;

    /**
     * @brief Add reader to the source list
     *
     * @param config Configuration of the reader.
     */
    void add_reader(const std::shared_ptr<UniversalReaderDialogConfig> &config);
};
