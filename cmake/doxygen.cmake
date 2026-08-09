# Generate the Doxygen documentation
find_package(Doxygen)

if(Doxygen_FOUND)
    set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/doxygen)
    set(DOXYGEN_INPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/source)
    set(DOXYGEN_README_FILE ${CMAKE_CURRENT_SOURCE_DIR}/README.md)
    set(DOXYGEN_IMAGE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_BINARY_DIR}/Doxyfile
        @ONLY
    )

    doxygen_add_docs(doxygen_docs CONFIG_FILE ${CMAKE_BINARY_DIR}/Doxyfile)
endif()
