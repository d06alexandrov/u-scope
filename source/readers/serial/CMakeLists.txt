qt_add_library(serial_reader STATIC)

target_include_directories(serial_reader PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(
    serial_reader
    PRIVATE # cmake-format: sortable
            serialreader.cpp serialreader_dialog.cpp serialreader_dialog.ui serialreader_module.cpp
)

target_link_libraries(
    serial_reader
    PUBLIC Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets
           Qt${QT_VERSION_MAJOR}::SerialPort common_headers universal_reader
)
