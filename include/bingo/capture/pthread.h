/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef BINGO_PTHREAD_H
#define BINGO_PTHREAD_H

#include <bingo/capture.h>
#include <pthread.h>

#define _EVENT_START 10
enum pthread_events {
    EVENT_THREAD_CREATE   = 0 + _EVENT_START,
    EVENT_THREAD_JOIN     = 1 + _EVENT_START,
    EVENT_MUTEX_LOCK      = 2 + _EVENT_START,
    EVENT_MUTEX_TIMEDLOCK = 3 + _EVENT_START,
    EVENT_MUTEX_TRYLOCK   = 4 + _EVENT_START,
    EVENT_MUTEX_UNLOCK    = 5 + _EVENT_START,
    EVENT_COND_WAIT       = 6 + _EVENT_START,
    EVENT_COND_TIMEDWAIT  = 7 + _EVENT_START,
    EVENT_COND_SIGNAL     = 8 + _EVENT_START,
    EVENT_COND_BROADCAST  = 9 + _EVENT_START,
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
    const pthread_mutex_t *mutex;
    int ret;
};

struct pthread_mutex_timed_event {
    const void *pc;
    const pthread_mutex_t *mutex;
    const struct timespec *abstime;
    int ret;
};

struct pthread_cond_event {
    const void *pc;
    const pthread_cond_t *cond;
    const pthread_mutex_t *mutex;
    int ret;
};

struct pthread_rwlock_event {
    const void *pc;
    const pthread_rwlock_t *rwlock;
    int ret;
};

#endif /* BINGO_PTHREAD_H */
