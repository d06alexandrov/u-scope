# User Interface subsystem
qt_add_library(user_interface STATIC)

target_include_directories(
    user_interface PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
                          ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_sources(
    user_interface
    PRIVATE # cmake-format: sortable
            channelbar_model.cpp mainchart_controller.cpp mainwindow.cpp
            mainwindow.ui overviewchart_controller.cpp timebase_model.cpp
)

qt_add_qml_module(
    user_interface
    URI UI
    NO_PLUGIN
    RESOURCE_PREFIX "/qt/qml"
    OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/UI"
    QML_FILES # cmake-format: sortable
              ChannelBar.qml ScreenRoot.qml MainChart.qml OverviewChart.qml
)

set_source_files_properties(
    ${CMAKE_SOURCE_DIR}/LICENSE PROPERTIES QT_RESOURCE_ALIAS LICENSE
)

qt_add_resources(
    user_interface "ui_resources"
    PREFIX "/ui"
    FILES # cmake-format: sortable
          ${CMAKE_SOURCE_DIR}/LICENSE
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
