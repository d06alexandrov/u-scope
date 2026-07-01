#pragma once

#include "commontypes.hpp"
#include "universalreader.h"

#include <QMap>
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
    virtual std::shared_ptr<UniversalReaderConfig> to_reader_config() const = 0;

public:
    QMap<VariableId, QString> variable_names; /**< Names of the variables. */
};
