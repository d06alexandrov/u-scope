#include "appcontroller.hpp"
#include "version.hpp"

#include <QApplication>
#include <QCoreApplication>
#include <QLocale>
#include <QQmlApplicationEngine>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QCoreApplication::setApplicationName(
            QString::fromUtf8(app_info::app_name.data(), app_info::app_name.size()));
    QCoreApplication::setApplicationVersion(
            QString::fromUtf8(app_info::version.data(), app_info::version.size()));

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "u-scope_" + QLocale(locale).name();
        if (translator.load(baseName, ":/i18n/")) {
            app.installTranslator(&translator);
            break;
        }
    }

    AppController app_controller;

    QQmlApplicationEngine engine;

    engine.loadFromModule("UI", "MainWindow");

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
