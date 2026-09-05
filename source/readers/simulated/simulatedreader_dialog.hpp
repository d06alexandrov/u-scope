#pragma once

#include "simulatedreader.hpp"
#include "universalreader_dialog.hpp"

#include <QHash>
#include <QString>

/**
 * @brief Configuration for the @ref SimulatedReader.
 */
struct SimulatedReaderDialogConfig : UniversalReaderDialogConfig
{
    QHash<VariableId, SimulatedReaderConfig::Config>
            form_configs; /**< Configurations of the simulated values. */

    [[nodiscard]] std::shared_ptr<UniversalReaderConfig> to_reader_config() const override;

    /**
     * @brief Get a short name for a given simulated form configuration.
     *
     * @param form_conf The simulated form configuration.
     * @return A short name representing the simulated form configuration.
     */
    static QString get_form_config_short_name(const SimulatedReaderConfig::Config &form_conf);
};
