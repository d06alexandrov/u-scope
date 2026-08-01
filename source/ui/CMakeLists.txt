# User Interface subsystem
qt_add_library(user_interface STATIC)

target_include_directories(
    user_interface PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
                          ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_sources(
    user_interface PRIVATE # cmake-format: sortable
                           mainwindow.cpp mainwindow.ui sliding_window.hpp
)

target_link_libraries(
    user_interface
    PUBLIC Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Charts
           common_headers universal_reader serial_reader simulated_reader
)
