qt_add_library(simulated_reader STATIC)

target_include_directories(simulated_reader PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(
    simulated_reader
    PRIVATE # cmake-format: sortable
            simulatedreader.cpp
            simulatedreader_dialog.cpp
            simulatedreader_dialog.ui
            simulatedreader_dialog_form.cpp
            simulatedreader_dialog_form.ui
            simulatedreader_dialog_row.cpp
            simulatedreader_dialog_row.ui
            simulatedreader_module.cpp
)

target_link_libraries(
    simulated_reader
    PUBLIC Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets
           common_headers universal_reader
)
