cmake_minimum_required(VERSION 3.16)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

qt_add_library(simulated_reader STATIC 
    simulatedreader_dialog.cpp
    simulatedreader_dialog_form.cpp
    simulatedreader_dialog_form.ui
    simulatedreader_dialog_row.cpp
    simulatedreader_dialog_row.ui
    simulatedreader_dialog.ui
    simulatedreader.cpp
)

target_include_directories(simulated_reader
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(simulated_reader PUBLIC
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    common_headers
    universal_reader
)
