# Core processing logic
qt_add_library(core_logic STATIC)

target_include_directories(core_logic PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(
    core_logic PRIVATE # cmake-format: sortable
                       dataprocessor.cpp
)

target_link_libraries(
    core_logic PUBLIC Qt${QT_VERSION_MAJOR}::Core common_headers
                      universal_reader serial_reader simulated_reader
)
