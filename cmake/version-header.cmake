# Generate version header file
file(
    GLOB_RECURSE VERSION_IN_PATH
    LIST_DIRECTORIES false
    ${CMAKE_CURRENT_SOURCE_DIR}/version.hpp.in
)

if(VERSION_IN_PATH)
    list(GET VERSION_IN_PATH 0 VERSION_IN_PATH)
endif()

if(NOT VERSION_IN_PATH)
    message(
        FATAL_ERROR
            "Could not find version.hpp.in starting from ${CMAKE_CURRENT_SOURCE_DIR}"
    )
endif()

file(RELATIVE_PATH VERSION_REL_PATH ${CMAKE_CURRENT_SOURCE_DIR}
     ${VERSION_IN_PATH}
)

get_filename_component(VERSION_REL_DIR ${VERSION_REL_PATH} DIRECTORY)

set(VERSION_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/${VERSION_REL_DIR}")

configure_file(${VERSION_IN_PATH} "${VERSION_INCLUDE_DIR}/version.hpp" @ONLY)
