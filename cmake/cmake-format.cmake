# Format cmake files
find_program(CMAKE_FORMAT cmake-format)

if(CMAKE_FORMAT)
    file(
        GLOB_RECURSE ALL_CMAKE_FILES
        LIST_DIRECTORIES false
        "${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cmake"
    )

    list(FILTER ALL_CMAKE_FILES EXCLUDE REGEX "^${CMAKE_BINARY_DIR}/|/\\.git/")

    add_custom_target(
        cmake-format
        COMMAND ${CMAKE_COMMAND} -E echo "Formatting CMake files..."
        COMMAND ${CMAKE_FORMAT} -i ${ALL_CMAKE_FILES}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Running cmake-format on all CMake files"
        VERBATIM
    )
endif()
