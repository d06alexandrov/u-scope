#include "universalreader.h"

#include "dataprocessor.h"

#include <QDebug>

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
    /* Skip if reader is not uninitialized. */
    if (m_status != Uninitialized) {
        return;
    }

    try {
        setup();

        m_timer = new QTimer(this);
        connect(m_timer, &QTimer::timeout, this, &UniversalReader::reader_process);

        set_status(Initialized);
    } catch (const std::exception &e) {
        qDebug() << tr("Caught error during reader initialization:") << e.what();
        set_status(Uninitialized);
    }
}

void UniversalReader::reader_start(ReaderId id)
{
    /* Skip if signal was sent to another reader.*/
    if (id != m_id) {
        return;
    }

    /* Skip if reader is not in one of the states that allows to start it. */
    if ((m_status != Initialized) && (m_status != Stopped) && (m_status != Error)) {
        return;
    }

    try {
        start();

        m_timer->start(m_config->update_period_ms);
        set_status(Running);
    } catch (const std::exception &e) {
        qDebug() << tr("Caught error:") << e.what();
        set_status(Error);
    }
}
void UniversalReader::reader_stop(ReaderId id)
{
    if (id != m_id) {
        return;
    }

    /* Skip if reader is not running. */
    if (m_status != Running) {
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
