# Copyright (C) 2025 Huawei Technologies Co., Ltd.                             #
# SPDX-License-Identifier: 0BSD                                                #

cmake_minimum_required(VERSION 3.20)
project(
  tsano
  LANGUAGES C CXX
  VERSION 1.1)

# depends on tmplr: https://github.com/open-s4c/tmplr
include(tmplr.cmake)

add_compile_options(-Wall -Werror -Wextra)

set(TSANO_C ${CMAKE_CURRENT_BINARY_DIR}/tsano.c)
set(TSANO_C_IN ${CMAKE_CURRENT_SOURCE_DIR}/tsano.c.in)

add_custom_command(
  OUTPUT ${TSANO_C}
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}
  COMMAND ${TMPLR_PROGRAM} ${TSANO_C_IN} > ${TSANO_C}
  DEPENDS ${TSANO_C_IN})

add_custom_target(tsano.c DEPENDS ${TSANO_C})
add_custom_target(update-tsano DEPENDS tsano.c)

add_library(tsano SHARED ${TSANO_C} syscall.c)

install(TARGETS tsano DESTINATION lib)
install(
  FILES tsano
  DESTINATION bin
  PERMISSIONS
    OWNER_READ
    OWNER_WRITE
    OWNER_EXECUTE
    GROUP_READ
    GROUP_EXECUTE
    WORLD_READ
    WORLD_EXECUTE)
