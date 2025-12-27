qt_add_executable(u-scope
    dataprocessor.cpp
    dataprocessor.h
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    uartconfigdialog.cpp
    uartconfigdialog.h
    uartconfigdialog.ui
)

find_package(Qt6 REQUIRED COMPONENTS Widgets Charts SerialPort)

target_link_libraries(u-scope PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Charts
Qt${QT_VERSION_MAJOR}::SerialPort)

set_target_properties(u-scope PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS u-scope
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
