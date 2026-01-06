cmake_minimum_required(VERSION 3.16)

project(u-scope VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED Core)

qt_standard_project_setup()

add_subdirectory(source)

# Add translation support
find_package(Qt6 REQUIRED COMPONENTS LinguistTools)

# Define your translation files
# This command automatically creates targets to update .ts and generate .qm files
qt_add_translations(u-scope
    TS_FILES
        i18n/u-scope_en.ts
    RESOURCE_PREFIX "/i18n"
)

# Generation of the Doxygen documentation
find_package(Doxygen)

if(Doxygen_FOUND)
  set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doxygen)

  doxygen_add_docs(doxygen_docs ${PROJECT_SOURCE_DIR})
endif()
