cmake_minimum_required(VERSION 3.20)
project(bingo
        LANGUAGES C
        VERSION 0.1)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(
    -Werror
    -Wall
    -Wunused-variable
    -Wextra
    -Wno-missing-braces
    -Wno-switch
    -Wno-unused-parameter
    -Wno-missing-field-initializers
    -Wno-override-init)

include_directories(include)
add_subdirectory(src)
add_subdirectory(test)
