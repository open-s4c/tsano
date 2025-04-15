# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: MIT

configure_file(tsano tsano)
configure_file(bingo bingo)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/tsano
              ${CMAKE_CURRENT_BINARY_DIR}/bingo DESTINATION bin)
