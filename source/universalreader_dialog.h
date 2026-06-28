#pragma once

#include "commontypes.hpp"

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
};
