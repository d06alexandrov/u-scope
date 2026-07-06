cmake_minimum_required(VERSION 3.16)

# Base class for readers
qt_add_library(universal_reader STATIC 
    universalreader.cpp
)

target_include_directories(universal_reader
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../
)

target_link_libraries(universal_reader PUBLIC common_headers)


# Add serial port readers
add_subdirectory(serial)

# Add simulated readers
add_subdirectory(simulated)
