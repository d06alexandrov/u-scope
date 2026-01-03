#include "universalreader.h"
#include "dataprocessor.h"

UniversalReader::UniversalReader(uint64_t id, DataProcessor *processor,
                                 std::shared_ptr<UniversalReaderConfig> config)
    : QObject{ nullptr }
    , m_id(id)
    , m_data_processor(processor)
    , m_config(std::move(config))
{
}

void UniversalReader::reader_setup()
{
    setup();

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &UniversalReader::reader_process);

    m_timer->start(m_config->update_period_ms);
}

void UniversalReader::reader_process()
{
    process();

    if (!m_buffer.isEmpty()) {
        m_data_processor->add_variables_data(m_id, m_buffer);
        m_buffer.clear();
    }
}
