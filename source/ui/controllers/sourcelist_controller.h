#pragma once

#include "commontypes.hpp"

#include <QObject>
#include <QStandardItemModel>
#include <QtQmlIntegration/qqmlintegration.h>

struct UniversalReaderDialogConfig;

/**
 * @brief The class responsible for controlling the source list in the application.
 */
class SourceListController : public QObject
{
    Q_OBJECT
    QML_ANONYMOUS

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    Q_PROPERTY(QVariantList availableSourceModules READ availableSourceModules CONSTANT)
    Q_PROPERTY(QAbstractItemModel *model MEMBER m_source_model CONSTANT)
#endif // DOXYGEN_SHOULD_SKIP_THIS

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

    /**
     * @brief Get the list of available source modules.
     *
     * @return A QVariantList containing the available source modules.
     */
    [[nodiscard]] QVariantList availableSourceModules() const;

    /**
     * @brief Configure a source of a given type.
     *
     * @param type_id The ID of the source type to configure.
     * @param session_model The session model to use for configuration.
     */
    Q_INVOKABLE void configSource(const QString &type_id, QObject *session_model);

    /**
     * @brief Delete a source from the source list.
     *
     * @param reader_id The ID of the source to be deleted.
     */
    Q_INVOKABLE void delete_source(int reader_id);

    /**
     * @brief Assign a variable to a channel.
     *
     * @param reader_id The ID of the reader.
     * @param variable_id The ID of the variable.
     * @param channel_id The ID of the channel.
     */
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
    void configure_reader(ReaderId reader_id, std::shared_ptr<UniversalReaderDialogConfig> config);

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
            m_readers_config; /**< Readers configuration. */

    QStandardItemModel *m_source_model; /**< Source List model. */

    /**
     * @brief Get available reader index.
     *
     * @return Available reader index.
     */
    [[nodiscard]] ReaderId get_available_reader_idx() const;

    /**
     * @brief Add reader to the source list
     *
     * @param config Configuration of the reader.
     */
    void add_reader(const std::shared_ptr<UniversalReaderDialogConfig> &config);
};
