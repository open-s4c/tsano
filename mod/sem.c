/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <semaphore.h>

#include <bingo/capture/semaphore.h>
#include <bingo/interpose.h>

INTERPOSE(int, sem_post, sem_t *sem)
{
    struct sem_event ev = {.sem = sem, .pc = INTERPOSE_PC};

    int err = capture_before(EVENT_SEM_POST, &ev);
    ev.ret  = REAL(sem_post, sem);
    if (err != PS_DROP)
        capture_after(EVENT_SEM_POST, &ev);
    return ev.ret;
}

INTERPOSE(int, sem_wait, sem_t *sem)
{
    struct sem_event ev = {.sem = sem, .pc = INTERPOSE_PC};

    int err = capture_before(EVENT_SEM_WAIT, &ev);
    ev.ret  = REAL(sem_wait, sem);
    if (err != PS_DROP)
        capture_after(EVENT_SEM_WAIT, &ev);
    return ev.ret;
}

INTERPOSE(int, sem_trywait, sem_t *sem)
{
    struct sem_event ev = {.sem = sem, .pc = INTERPOSE_PC};

    int err = capture_before(EVENT_SEM_TRYWAIT, &ev);
    ev.ret  = REAL(sem_trywait, sem);
    if (err != PS_DROP)
        capture_after(EVENT_SEM_TRYWAIT, &ev);
    return ev.ret;
}

#if defined(__linux__)
INTERPOSE(int, sem_timedwait, sem_t *sem, const struct timespec *timeout)
{
    struct sem_event ev = {.sem = sem, .pc = INTERPOSE_PC};

    int err = capture_before(EVENT_SEM_TIMEDWAIT, &ev);
    ev.ret  = REAL(sem_timedwait, sem, timeout);
    if (err != PS_DROP)
        capture_after(EVENT_SEM_TIMEDWAIT, &ev);
    return ev.ret;
}
#endif

BINGO_MODULE_INIT()
