cmake_minimum_required(VERSION 3.23)

find_package(Qt6 REQUIRED COMPONENTS Widgets Quick QuickWidgets Qml Charts SerialPort)

# Add interface library to propagate common header
add_library(common_headers INTERFACE)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/version.hpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/include/version.hpp @ONLY
)

target_sources(
    common_headers
    PUBLIC FILE_SET
           HEADERS
           BASE_DIRS
           ${CMAKE_CURRENT_SOURCE_DIR}/include
           ${CMAKE_CURRENT_BINARY_DIR}/include
           FILES
           ${CMAKE_CURRENT_SOURCE_DIR}/include/commontypes.hpp
           ${CMAKE_CURRENT_BINARY_DIR}/include/version.hpp
)

target_link_libraries(common_headers INTERFACE Qt${QT_VERSION_MAJOR}::Core)

# Add directory with readers library
add_subdirectory(readers)

# Add directory with user interface
add_subdirectory(ui)

# Main application
qt_add_executable(u-scope)

target_sources(
    u-scope PRIVATE # cmake-format: sortable
                    dataprocessor.cpp main.cpp
)

target_link_libraries(
    u-scope PRIVATE Qt${QT_VERSION_MAJOR}::Core common_headers universal_reader
                    serial_reader simulated_reader user_interface
)

set_target_properties(
    u-scope
    PROPERTIES MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
               MACOSX_BUNDLE_SHORT_VERSION_STRING
               ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
               MACOSX_BUNDLE TRUE
               WIN32_EXECUTABLE TRUE
)

# Add translation support
find_package(Qt6 REQUIRED COMPONENTS LinguistTools)
qt_add_translations(
    u-scope
    TS_FILES ${PROJECT_SOURCE_DIR}/i18n/u-scope_en.ts
    RESOURCE_PREFIX "/i18n"
)

if(WIN32)
    install(TARGETS u-scope)
    qt_generate_deploy_qml_app_script(
        TARGET u-scope
        OUTPUT_SCRIPT deploy_script
        NO_UNSUPPORTED_PLATFORM_ERROR NO_TRANSLATIONS
    )
    install(SCRIPT ${deploy_script})
else()
    include(GNUInstallDirs)
    install(
        TARGETS u-scope
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif()
