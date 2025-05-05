/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <assert.h>
#include <bingo/capture/pthread.h>
#include <bingo/interpose.h>
#include <pthread.h>

INTERPOSE(int, pthread_rwlock_rdlock, pthread_rwlock_t *rwlock)
{
    struct pthread_rwlock_event ev = {.rwlock = rwlock, .pc = INTERPOSE_PC};

    int err = capture_before(EVENT_RWLOCK_RDLOCK, &ev);
    ev.ret  = REAL(pthread_rwlock_rdlock, rwlock);
    if (err != PS_DROP)
        capture_after(EVENT_RWLOCK_RDLOCK, &ev);
    return ev.ret;
}

INTERPOSE(int, pthread_rwlock_timedrdlock, pthread_rwlock_t *rwlock,
          const struct timespec *abstime)
{
    struct pthread_rwlock_event ev = {
        .pc      = INTERPOSE_PC,
        .rwlock  = rwlock,
        .abstime = abstime,
    };

    int err = capture_before(EVENT_RWLOCK_TIMEDRDLOCK, &ev);
    ev.ret  = REAL(pthread_rwlock_timedrdlock, rwlock, abstime);
    if (err != PS_DROP)
        capture_after(EVENT_RWLOCK_TIMEDRDLOCK, &ev);
    return ev.ret;
}

INTERPOSE(int, pthread_rwlock_tryrdlock, pthread_rwlock_t *rwlock)
{
    struct pthread_rwlock_event ev = {.rwlock = rwlock, .pc = INTERPOSE_PC};

    int err = capture_before(EVENT_RWLOCK_TRYRDLOCK, &ev);
    ev.ret  = REAL(pthread_rwlock_tryrdlock, rwlock);
    if (err != PS_DROP)
        capture_after(EVENT_RWLOCK_TRYRDLOCK, &ev);
    return ev.ret;
}

INTERPOSE(int, pthread_rwlock_wrlock, pthread_rwlock_t *rwlock)
{
    struct pthread_rwlock_event ev = {.rwlock = rwlock, .pc = INTERPOSE_PC};

    int err = capture_before(EVENT_RWLOCK_WRLOCK, &ev);
    ev.ret  = REAL(pthread_rwlock_wrlock, rwlock);
    if (err != PS_DROP)
        capture_after(EVENT_RWLOCK_WRLOCK, &ev);
    return ev.ret;
}

INTERPOSE(int, pthread_rwlock_timedwrlock, pthread_rwlock_t *rwlock,
          const struct timespec *abstime)
{
    struct pthread_rwlock_event ev = {
        .pc      = INTERPOSE_PC,
        .rwlock  = rwlock,
        .abstime = abstime,
    };

    int err = capture_before(EVENT_RWLOCK_TIMEDWRLOCK, &ev);
    ev.ret  = REAL(pthread_rwlock_timedwrlock, rwlock, abstime);
    if (err != PS_DROP)
        capture_after(EVENT_RWLOCK_TIMEDWRLOCK, &ev);
    return ev.ret;
}

INTERPOSE(int, pthread_rwlock_trywrlock, pthread_rwlock_t *rwlock)
{
    struct pthread_rwlock_event ev = {.rwlock = rwlock, .pc = INTERPOSE_PC};

    int err = capture_before(EVENT_RWLOCK_TRYWRLOCK, &ev);
    ev.ret  = REAL(pthread_rwlock_trywrlock, rwlock);
    if (err != PS_DROP)
        capture_after(EVENT_RWLOCK_TRYWRLOCK, &ev);
    return ev.ret;
}

INTERPOSE(int, pthread_rwlock_unlock, pthread_rwlock_t *rwlock)
{
    struct pthread_rwlock_event ev = {.rwlock = rwlock, .pc = INTERPOSE_PC};

    int err = capture_before(EVENT_RWLOCK_UNLOCK, &ev);
    ev.ret  = REAL(pthread_rwlock_unlock, rwlock);
    if (err != PS_DROP)
        capture_after(EVENT_RWLOCK_UNLOCK, &ev);
    return ev.ret;
}

BINGO_MODULE_INIT()
