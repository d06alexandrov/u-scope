# Interface for Readers
qt_add_library(readers INTERFACE)

# Base class for readers
qt_add_library(universal_reader STATIC)

target_include_directories(universal_reader PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(
    universal_reader
    PUBLIC FILE_SET
           HEADERS
           BASE_DIRS
           ${CMAKE_CURRENT_SOURCE_DIR}/include
           FILES
           include/universalreader.hpp
           include/universalreader_dialog.hpp
           include/reader_registry.hpp
    PRIVATE universalreader.cpp
)

qt_add_qml_module(
    universal_reader
    URI UI.Readers
    RESOURCE_PREFIX "/qt/qml"
    OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/UI/Readers"
    QML_FILES qml/ReaderDialog.qml
)

target_link_libraries(
    universal_reader PUBLIC Qt${QT_VERSION_MAJOR}::Core
                            Qt${QT_VERSION_MAJOR}::Qml common_headers
)

# Add serial port readers
add_subdirectory(serial)

# Add simulated readers
add_subdirectory(simulated)

# Warning: WHOLE_ARCHIVE is required for reader registry
target_link_libraries(
    readers
    INTERFACE universal_reader $<LINK_LIBRARY:WHOLE_ARCHIVE,serial_reader>
              $<LINK_LIBRARY:WHOLE_ARCHIVE,simulated_reader>
)
