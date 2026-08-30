qt_add_library(simulated_reader STATIC)

target_include_directories(simulated_reader PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(
    simulated_reader
    PRIVATE # cmake-format: sortable
            simulatedreader.cpp simulatedreader_dialog.cpp
            simulatedreader_module.cpp
)

qt_add_qml_module(
    simulated_reader
    URI UI.Simulated
    RESOURCE_PREFIX "/qt/qml"
    OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/UI/Simulated"
    QML_FILES # cmake-format: sortable
              qml/SimulatedReaderDialog.qml qml/SimulatedFormDialog.qml
    SOURCES simulatedreader_dialog_model.cpp simulatedreader_dialog_model.hpp
)

target_link_libraries(
    simulated_reader
    PUBLIC Qt${QT_VERSION_MAJOR}::Core
           common_headers universal_reader
)
