/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef BINGO_PTHREAD_H
#define BINGO_PTHREAD_H

#include <pthread.h>

#include <bingo/capture.h>

#define _EVENT_START 10
enum pthread_events {
    EVENT_THREAD_CREATE  = 0 + _EVENT_START,
    EVENT_THREAD_JOIN    = 1 + _EVENT_START,
    EVENT_MUTEX_LOCK     = 2 + _EVENT_START,
    EVENT_MUTEX_UNLOCK   = 3 + _EVENT_START,
    EVENT_MUTEX_TRYLOCK  = 4 + _EVENT_START,
    EVENT_COND_WAIT      = 5 + _EVENT_START,
    EVENT_COND_TIMEDWAIT = 6 + _EVENT_START,
    EVENT_COND_SIGNAL    = 7 + _EVENT_START,
    EVENT_COND_BROADCAST = 8 + _EVENT_START,
};
#undef _EVENT_START

struct pthread_create_event {
    const void *pc;
    const pthread_t *thread;
    int ret;
};

struct pthread_join_event {
    const void *pc;
    const pthread_t thread;
    int ret;
};

struct pthread_mutex_event {
    const void *pc;
    const void *mutex;
    int ret;
};

struct pthread_cond_event {
    const void *pc;
    const void *cond;
    const void *mutex;
    int ret;
};

#endif /* BINGO_PTHREAD_H */
