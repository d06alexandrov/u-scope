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

    set_status(Initialized);
}

void UniversalReader::reader_start(uint64_t id)
{
    if (id != m_id) {
        return;
    }

    try {
        start();

        m_timer->start(m_config->update_period_ms);
        set_status(Running);
    } catch (const std::exception &e) {
        qDebug() << "Caught error:" << e.what();
        set_status(Error);
    }
}
void UniversalReader::reader_stop(uint64_t id)
{
    if (id != m_id) {
        return;
    }

    m_timer->stop();

    stop();
    m_buffer.clear();

    set_status(Stopped);
}

void UniversalReader::reader_process()
{
    if ((m_status == Running) && (m_timer->isActive())) {
        process();

        if (!m_buffer.isEmpty()) {
            m_data_processor->add_variables_data(m_id, m_buffer);
            m_buffer.clear();
        }
    }
}

void UniversalReader::set_status(Status new_status)
{
    if (m_status != new_status) {
        m_status = new_status;
        emit report_status(m_id, new_status);
    }
}
