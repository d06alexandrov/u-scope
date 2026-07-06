qt_add_library(serial_reader STATIC 
    serialreader.cpp
)

target_include_directories(serial_reader
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(serial_reader PUBLIC
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::SerialPort
    common_headers
    universal_reader
)
