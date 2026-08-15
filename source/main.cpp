#include "mainwindow.h"
#include "version.hpp"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setApplicationName(
            QString::fromUtf8(app_info::app_name.data(), app_info::app_name.size()));
    QCoreApplication::setApplicationVersion(
            QString::fromUtf8(app_info::version.data(), app_info::version.size()));

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "u-scope_" + QLocale(locale).name();
        if (translator.load(baseName, ":/i18n/")) {
            a.installTranslator(&translator);
            break;
        }
    }
    MainWindow w;
    w.show();
    return a.exec();
}
