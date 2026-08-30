qt_add_library(serial_reader STATIC)

target_include_directories(serial_reader PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(
    serial_reader
    PRIVATE # cmake-format: sortable
            serialreader.cpp serialreader_dialog.cpp serialreader_module.cpp
)

qt_add_qml_module(
    serial_reader
    URI UI.Readers.Serial
    RESOURCE_PREFIX "/qt/qml"
    OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/UI/Readers/Serial"
    QML_FILES # cmake-format: sortable
              qml/SerialReaderDialog.qml
    SOURCES serialreader_dialog_model.cpp serialreader_dialog_model.hpp
    DEPENDENCIES TARGET universal_reader
)

target_link_libraries(
    serial_reader
    PUBLIC Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Qml
           Qt${QT_VERSION_MAJOR}::SerialPort common_headers universal_reader
)
