cmake_minimum_required(VERSION 3.20)
project(bingo
        LANGUAGES C CXX ASM
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
    $<$<COMPILE_LANGUAGE:C>:-Wno-override-init>)

if(CMAKE_SYSTEM_NAME MATCHES "NetBSD")
    add_compile_options(-D_XOPEN_SOURCE=700)
endif()

find_package(libvsync 4.2 CONFIG REQUIRED)
add_library(vsync INTERFACE IMPORTED)
link_libraries(vsync)

include_directories(include)
add_subdirectory(lib)
add_subdirectory(mod)
add_subdirectory(tsano)
add_subdirectory(test)

# Installation

include(GNUInstallDirs)
include(CheckSymbolExists)
include(cmake/export.cmake)
install(DIRECTORY include/bingo DESTINATION include)
install(TARGETS bingo EXPORT ${PROJECT_TARGETS})
