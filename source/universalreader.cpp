#include "universalreader.h"
#include "dataprocessor.h"

UniversalReader::UniversalReader(uint64_t id, DataProcessor *processor)
    : QObject{ nullptr }
    , m_id(id)
    , m_data_processor(processor)
{
}
