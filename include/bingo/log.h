/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef BINGO_LOG_H
#define BINGO_LOG_H

#include <stdio.h>
#include <unistd.h>
#define LOG_MSG_MAX 1024

#define log_printf(fmt, ...)                                                   \
    do {                                                                       \
        char msg[LOG_MSG_MAX];                                                 \
        int n = snprintf(msg, sizeof(msg), fmt, ##__VA_ARGS__);                \
        write(STDOUT_FILENO, msg, n);                                          \
    } while (0)

#define log_debugf log_printf

#endif /* BINGO_LOG_H */
