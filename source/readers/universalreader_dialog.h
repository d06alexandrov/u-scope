#pragma once

#include "commontypes.hpp"
#include "universalreader.h"

#include <QMap>
#include <QSet>
#include <memory>

/**
 * @brief Configuration for the dialog of the @ref UniversalReader.
 */
struct UniversalReaderDialogConfig
{
    virtual ~UniversalReaderDialogConfig() = default;
    /**
     * @brief Convert into Reader Config
     *
     * @return pointer to the copy of the config
     */
    [[nodiscard]] virtual std::shared_ptr<UniversalReaderConfig> to_reader_config() const = 0;

public:
    QMap<VariableId, QString> variable_names; /**< Names of the variables. */

    /**
     * @brief Get an available variable ID that is not already in use.
     *
     * This function returns the first available variable ID that is not assigned at the moment.
     *
     * @note Variable IDs are starting from 0.
     *
     * @param current_ids The set of currently used variable IDs.
     * @return An available variable ID.
     */
    static VariableId
    get_available_variable_idx(const QSet<VariableId> &current_ids = QSet<VariableId>())
    {
        VariableId new_variable_id = current_ids.size();

        while (current_ids.contains(new_variable_id)) {
            new_variable_id--;
        }

        return new_variable_id;
    }
};
