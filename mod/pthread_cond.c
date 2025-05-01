/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <assert.h>
#include <pthread.h>

#include <bingo/capture/pthread.h>
#include <bingo/interpose.h>

INTERPOSE(int, pthread_cond_wait, pthread_cond_t *cond, pthread_mutex_t *mutex)
{
    struct pthread_cond_event ev = {.cond  = cond,
                                    .mutex = mutex,
                                    .pc    = INTERPOSE_PC};

    int err = capture_before(EVENT_COND_WAIT, &ev);
    ev.ret  = REAL(pthread_cond_wait, cond, mutex);
    if (err != PS_DROP)
        capture_after(EVENT_COND_WAIT, &ev);
    return ev.ret;
}

INTERPOSE(int, pthread_cond_timedwait, pthread_cond_t *cond,
          pthread_mutex_t *mutex, const struct timespec *abstime)
{
    struct pthread_cond_event ev = {.cond  = cond,
                                    .mutex = mutex,
                                    .pc    = INTERPOSE_PC};

    int err = capture_before(EVENT_COND_TIMEDWAIT, &ev);
    ev.ret  = REAL(pthread_cond_timedwait, cond, mutex, abstime);
    if (err != PS_DROP)
        capture_after(EVENT_COND_TIMEDWAIT, &ev);
    return ev.ret;
}

INTERPOSE(int, pthread_cond_signal, pthread_cond_t *cond)
{
    struct pthread_cond_event ev = {.cond = cond, .pc = INTERPOSE_PC};

    int err = capture_before(EVENT_COND_SIGNAL, &ev);
    ev.ret  = REAL(pthread_cond_signal, cond);
    if (err != PS_DROP)
        capture_after(EVENT_COND_SIGNAL, &ev);
    return ev.ret;
}

INTERPOSE(int, pthread_cond_broadcast, pthread_cond_t *cond)
{
    struct pthread_cond_event ev = {.cond = cond, .pc = INTERPOSE_PC};

    int err = capture_before(EVENT_COND_BROADCAST, &ev);
    ev.ret  = REAL(pthread_cond_broadcast, cond);
    if (err != PS_DROP)
        capture_after(EVENT_COND_BROADCAST, &ev);
    return ev.ret;
}

BINGO_MODULE_INIT()
