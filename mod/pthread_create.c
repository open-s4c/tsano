/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <pthread.h>
#include <stdlib.h>

#include <bingo/intercept.h>
#include <bingo/intercept/pthread.h>
#include <bingo/interpose.h>
#include <bingo/mempool.h>
#include <bingo/module.h>
#include <bingo/self.h>

typedef struct {
    void *(*run)(void *);
    void *arg;
} trampoline_t;


/* On NetBSD pthread_exit is a macro mapping to __libc_thread_exit. However, we
 * have to capture the real pthread_exit, hence, we undefine it. */
#undef pthread_exit
BINGO_NORET
INTERPOSE(void, pthread_exit, void *ptr)
{
    intercept_at(EVENT_THREAD_FINI, 0, 0);
    REAL(pthread_exit, ptr);
    exit(1); // unreachable
}

static void *
_trampoline(void *targ)
{
    trampoline_t *t      = (trampoline_t *)targ;
    void *arg            = t->arg;
    void *(*run)(void *) = t->run;
    mempool_free(t);

    intercept_at(EVENT_THREAD_INIT, 0, 0);
    void *ret = run(arg);
    intercept_at(EVENT_THREAD_FINI, 0, 0);
    return ret;
}

INTERPOSE(int, pthread_create, pthread_t *thread, const pthread_attr_t *attr,
          void *(*run)(void *), void *arg)
{
    trampoline_t *t;

    t  = mempool_alloc(sizeof(trampoline_t));
    *t = (trampoline_t){.arg = arg, .run = run};

    struct pthread_create_event ev = {.thread = thread, .pc = INTERPOSE_PC};
    intercept_before(EVENT_THREAD_CREATE, &ev, 0);
    ev.ret = REAL(pthread_create, thread, attr, _trampoline, t);
    intercept_after(EVENT_THREAD_CREATE, &ev, 0);
    return ev.ret;
}

INTERPOSE(int, pthread_join, pthread_t thread, void **ptr)
{
    struct pthread_join_event ev = {.thread = thread, .pc = INTERPOSE_PC};
    intercept_before(EVENT_THREAD_JOIN, &ev, 0);
    ev.ret = REAL(pthread_join, thread, ptr);
    intercept_after(EVENT_THREAD_JOIN, &ev, 0);

    return ev.ret;
}

BINGO_MODULE_INIT()
