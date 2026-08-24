# User Interface subsystem
qt_add_library(user_interface STATIC)

set(CONTROLLERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/controllers)
set(MODELS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/models)
set(SINGLETON_DIR ${CMAKE_CURRENT_SOURCE_DIR}/singleton)

target_include_directories(
    user_interface
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/..
           ${MODELS_DIR} ${CONTROLLERS_DIR} ${SINGLETON_DIR}
)

target_sources(
    user_interface
    PRIVATE # cmake-format: sortable
            ${CONTROLLERS_DIR}/appcontroller.cpp
            ${CONTROLLERS_DIR}/mainchart_controller.cpp
            ${CONTROLLERS_DIR}/overviewchart_controller.cpp
            ${CONTROLLERS_DIR}/sourcelist_controller.cpp
            ${MODELS_DIR}/channelbar_model.cpp
            ${MODELS_DIR}/timebase_model.cpp
            ${MODELS_DIR}/verticalscale_model.cpp
)

qt_add_qml_module(
    user_interface
    URI UI
    RESOURCE_PREFIX "/qt/qml"
    OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/UI"
    QML_FILES # cmake-format: sortable
              qml/MainWindow.qml
              qml/ChannelBar.qml
              qml/ScreenRoot.qml
              qml/MainChart.qml
              qml/OverviewChart.qml
              qml/SourceList.qml
              qml/ControlPanel.qml
    SOURCES # cmake-format: sortable
            ${SINGLETON_DIR}/appcontroller_singleton.cpp
            ${SINGLETON_DIR}/appcontroller_singleton.hpp
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
    INTERFACE user_interfaceplugin
)
