/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <assert.h>
#include <pthread.h>

#include <bingo/capture/pthread.h>
#include <bingo/interpose.h>

INTERPOSE(int, pthread_mutex_lock, pthread_mutex_t *mutex)
{
    struct pthread_mutex_event ev = {.mutex = mutex, .pc = INTERPOSE_PC};

    int err = capture_before(EVENT_MUTEX_LOCK, &ev);
    ev.ret  = REAL(pthread_mutex_lock, mutex);
    if (err != PS_DROP)
        capture_after(EVENT_MUTEX_LOCK, &ev);
    return ev.ret;
}

INTERPOSE(int, pthread_mutex_trylock, pthread_mutex_t *mutex)
{
    struct pthread_mutex_event ev = {.mutex = mutex, .pc = INTERPOSE_PC};

    int err = capture_before(EVENT_MUTEX_TRYLOCK, &ev);
    ev.ret  = REAL(pthread_mutex_trylock, mutex);
    if (err != PS_DROP)
        capture_after(EVENT_MUTEX_TRYLOCK, &ev);
    return ev.ret;
}

INTERPOSE(int, pthread_mutex_unlock, pthread_mutex_t *mutex)
{
    struct pthread_mutex_event ev = {.mutex = mutex, .pc = INTERPOSE_PC};

    int err = capture_before(EVENT_MUTEX_UNLOCK, &ev);
    ev.ret  = REAL(pthread_mutex_unlock, mutex);
    if (err != PS_DROP)
        capture_after(EVENT_MUTEX_UNLOCK, &ev);
    return ev.ret;
}

BINGO_MODULE_INIT()
