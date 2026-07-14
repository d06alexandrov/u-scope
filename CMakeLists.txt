cmake_minimum_required(VERSION 3.16)

project(
    u-scope
    VERSION 0.1
    LANGUAGES CXX
)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake-format.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/doxygen.cmake")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 REQUIRED Core)

qt_standard_project_setup()

add_subdirectory(source)
