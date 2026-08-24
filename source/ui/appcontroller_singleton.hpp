#pragma once

#include "appcontroller.h"

#include <QQmlEngine>

/**
 * @brief Foreign class to expose the AppController singleton to QML.
 */
struct AppControllerForeign
{
    Q_GADGET
    QML_FOREIGN(AppController)
    QML_SINGLETON
    QML_NAMED_ELEMENT(AppController)

public:
    inline static AppController *m_instance = nullptr; /**< AppController instance. */

    /**
     * @brief Creates (returns) the AppController singleton instance for QML.
     *
     * @param engine Pointer to the QQmlEngine.
     * @param js_engine Pointer to the QJSEngine.
     * @return Pointer to the AppController singleton instance.
     */
    static AppController *create(QQmlEngine *engine, QJSEngine *js_engine);
};
