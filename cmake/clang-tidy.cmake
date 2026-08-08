# Clang-tidy checks
find_program(CLANG_TIDY clang-tidy)

if(CLANG_TIDY)
    set(CMAKE_CXX_CLANG_TIDY
        "${CLANG_TIDY};--checks=-*,cppcoreguidelines-*,modernize-*"
    )
endif()
