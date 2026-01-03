#include "universalreader.h"
#include "dataprocessor.h"

UniversalReader::UniversalReader(uint64_t id, DataProcessor *processor)
    : QObject{ nullptr }
    , m_id(id)
    , m_data_processor(processor)
{
}

void UniversalReader::reader_setup()
{
    setup();

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &UniversalReader::reader_process);

    /** TODO: make this interval configurable. */
    m_timer->start(10);
}

void UniversalReader::reader_process()
{
    process();

    if (!m_buffer.isEmpty()) {
        m_data_processor->add_variables_data(m_id, m_buffer);
        m_buffer.clear();
    }
}
