#include "appcontroller_singleton.hpp"

void AppControllerForeign::set_controller_instance(AppController *instance)
{
    m_instance = instance;
}

void AppControllerForeign::clear_controller_instance()
{
    if (m_instance) {
        m_instance = nullptr;
    }
}

AppController *AppControllerForeign::create(QQmlEngine *engine, QJSEngine *js_engine)
{
    if (m_instance) {
        QJSEngine::setObjectOwnership(m_instance, QJSEngine::CppOwnership);
    }

    return m_instance;
}
