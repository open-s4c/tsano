/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef BINGO_STACKTRACE_H
#define BINGO_STACKTRACE_H
#include <stdint.h>

#define EVENT_STACKTRACE_ENTER 2000
#define EVENT_STACKTRACE_EXIT  2001

typedef struct {
    const char *fname;
    uintptr_t pc;
} stacktrace_event_t;

#endif /* BINGO_STACKTRACE_H */
