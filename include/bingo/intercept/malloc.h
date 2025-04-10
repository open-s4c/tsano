/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef BINGO_MALLOC_H
#define BINGO_MALLOC_H
#include <stddef.h>

#define _EVENT_START 50
enum malloc_events {
    EVENT_MALLOC         = 0 + _EVENT_START,
    EVENT_CALLOC         = 1 + _EVENT_START,
    EVENT_REALLOC        = 2 + _EVENT_START,
    EVENT_FREE           = 3 + _EVENT_START,
    EVENT_POSIX_MEMALIGN = 4 + _EVENT_START,
    EVENT_ALIGNED_ALLOC  = 5 + _EVENT_START,
};
#undef _EVENT_START

struct malloc_event {
    const void *pc;
    size_t size;
    void *ptr;
    int ret;
};

#endif /* BINGO_MALLOC_H */
