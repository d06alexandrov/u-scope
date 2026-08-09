# Clang-tidy checks
find_program(CLANG_TIDY clang-tidy)

if(CLANG_TIDY AND NOT "$ENV{CI}")
    set(CMAKE_CXX_CLANG_TIDY
        "${CLANG_TIDY};--config-file=${CMAKE_SOURCE_DIR}/.clang-tidy"
    )
endif()
