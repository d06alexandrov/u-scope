# Base class for readers
qt_add_library(universal_reader STATIC)

target_include_directories(
    universal_reader PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
                            ${CMAKE_CURRENT_SOURCE_DIR}/../
)

target_sources(
    universal_reader PRIVATE # cmake-format: sortable
                             universalreader.cpp
)

target_link_libraries(
    universal_reader PUBLIC Qt${QT_VERSION_MAJOR}::Core common_headers
)

# Add serial port readers
add_subdirectory(serial)

# Add simulated readers
add_subdirectory(simulated)
