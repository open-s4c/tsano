/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef BINGO_STACKTRACE_H
#define BINGO_STACKTRACE_H

#include <bingo/capture.h>

#define _EVENT_START 40
enum stacktrace_events {
    EVENT_STACKTRACE_ENTER = 0 + _EVENT_START,
    EVENT_STACKTRACE_EXIT  = 1 + _EVENT_START,
};
#undef _EVENT_START

typedef struct {
    const void *pc;
    const void *caller;
    const char *fname;
} stacktrace_event_t;

#endif /* BINGO_STACKTRACE_H */
