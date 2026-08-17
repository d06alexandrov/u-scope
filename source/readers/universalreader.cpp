#include "universalreader.h"

#include <QDebug>

UniversalReader::UniversalReader(ReaderId id, std::shared_ptr<UniversalReaderConfig> config)
    : QObject{ nullptr }
    , m_id(id)
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

    for (auto &&[key, buffer] : m_buffer_map.asKeyValueRange()) {
        buffer->clear();
        m_buffer_pool.emplace(std::move(buffer));
    }

    m_buffer_map.clear();

    set_status(Stopped);
}

void UniversalReader::release_buffer(UniversalReaderBufferMap buffer_map)
{
    for (auto &&[key, buffer] : buffer_map.asKeyValueRange()) {
        buffer->clear();
        m_buffer_pool.emplace(std::move(buffer));
    }
}

void UniversalReader::reader_process()
{
    if ((m_status == Running) && (m_timer->isActive())) {
        process();

        if (!m_buffer_map.isEmpty()) {
            emit data_ready(m_id, std::move(m_buffer_map));
            m_buffer_map.clear();
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

void UniversalReader::allocate_buffer_pool(size_t amount, size_t reserved_size)
{
    for (size_t i = 0; i < amount; i++) {
        auto new_buffer = std::make_shared<std::vector<UData::Point>>();
        new_buffer->reserve(reserved_size);
        m_buffer_pool.emplace(std::move(new_buffer));
    }
}

bool UniversalReader::store_data(const VariableId &id, UData::Point &&data)
{
    if (!m_buffer_map.contains(id)) {
        if (m_buffer_pool.empty()) {
            return false;
        }
        auto new_buffer = std::move(m_buffer_pool.top());
        m_buffer_pool.pop();
        m_buffer_map.insert(id, std::move(new_buffer));
    }

    m_buffer_map[id]->emplace_back(std::move(data));

    return true;
}

UniversalReaderConfig *UniversalReader::get_universal_config() const
{
    return m_config.get();
}
