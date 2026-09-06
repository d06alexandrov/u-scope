# Add strict compilation flags in a form of interface library

add_library(project_warnings INTERFACE)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(project_warnings INTERFACE -Wall -Wextra -Wpedantic)
elseif(MSVC)
    target_compile_options(project_warnings INTERFACE /W4)
endif()
