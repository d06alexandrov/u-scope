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
    /**
     * @brief Sets the AppController singleton instance.
     *
     * @param instance Pointer to the AppController instance.
     */
    static void set_controller_instance(AppController *instance);

    /**
     * @brief Clears the AppController singleton instance.
     */
    static void clear_controller_instance();

    /**
     * @brief Creates (returns) the AppController singleton instance for QML.
     *
     * @param engine Pointer to the QQmlEngine.
     * @param js_engine Pointer to the QJSEngine.
     * @return Pointer to the AppController singleton instance.
     */
    static AppController *create(QQmlEngine *engine, QJSEngine *js_engine);

private:
    inline static AppController *m_instance = nullptr; /**< AppController instance. */
};
