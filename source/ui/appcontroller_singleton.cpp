#include "appcontroller_singleton.hpp"

AppController *AppControllerForeign::create(QQmlEngine *engine, QJSEngine *js_engine)
{
    QJSEngine::setObjectOwnership(m_instance, QJSEngine::CppOwnership);

    return m_instance;
}
