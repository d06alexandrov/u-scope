# Generate the Doxygen documentation
find_package(Doxygen)

if(Doxygen_FOUND)
    set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/doxygen)

    doxygen_add_docs(doxygen_docs ${CMAKE_CURRENT_SOURCE_DIR}/source)
endif()
