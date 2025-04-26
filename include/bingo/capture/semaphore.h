/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef BINGO_SEM_H
#define BINGO_SEM_H

#include <bingo/capture.h>

#define _EVENT_START 20
enum sem_events {
    EVENT_SEM_POST      = 0 + _EVENT_START,
    EVENT_SEM_WAIT      = 1 + _EVENT_START,
    EVENT_SEM_TRYWAIT   = 2 + _EVENT_START,
    EVENT_SEM_TIMEDWAIT = 3 + _EVENT_START,
};
#undef _EVENT_START

struct sem_event {
    const void *pc;
    void *sem;
    int ret;
};

#endif /* BINGO_SEM_H */
