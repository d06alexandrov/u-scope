cmake_minimum_required(VERSION 3.24)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/version.txt")
    file(READ "${CMAKE_CURRENT_SOURCE_DIR}/version.txt" APP_VERSION)
    string(STRIP "${APP_VERSION}" APP_VERSION)
else()
    set(APP_VERSION "0.0.0")
endif()

project(
    u-scope
    VERSION ${APP_VERSION}
    LANGUAGES CXX
)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/clang-tidy.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake-format.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/doxygen.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/version-header.cmake")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 REQUIRED Core)

qt_standard_project_setup(REQUIRES 6.8)

add_subdirectory(source)
