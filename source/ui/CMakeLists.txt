# User Interface subsystem
qt_add_library(user_interface STATIC)

target_include_directories(
    user_interface PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
                          ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_sources(
    user_interface PRIVATE # cmake-format: sortable
                           mainwindow.cpp mainwindow.ui sliding_window.hpp channelbar_model.cpp
)

qt_add_qml_module(user_interface
    URI UI
    NO_PLUGIN
    RESOURCE_PREFIX "/qt/qml"
    OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/UI"
    QML_FILES
        ChannelBar.qml
        ScreenRoot.qml
        MainChart.qml
)

target_link_libraries(
    user_interface
    PUBLIC Qt${QT_VERSION_MAJOR}::Core
           Qt${QT_VERSION_MAJOR}::Charts
           Qt${QT_VERSION_MAJOR}::Widgets
           Qt${QT_VERSION_MAJOR}::Qml
           Qt${QT_VERSION_MAJOR}::Quick
           Qt${QT_VERSION_MAJOR}::QuickWidgets
           common_headers
           universal_reader
           serial_reader
           simulated_reader
)
