/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

extern "C" {
#include <bingo/capture/cxa.h>
#include <bingo/interpose.h>

int __cxa_guard_acquire(void *addr);
int __cxa_guard_release(void *addr);
void __cxa_guard_abort(void *addr);
}

INTERPOSE(int, __cxa_guard_acquire, void *addr)
{
    struct cxa_event ev = {.pc = INTERPOSE_PC, .addr = addr};
    capture_before(EVENT_CXA_GUARD_ACQUIRE, &ev);
    ev.ret = REAL(__cxa_guard_acquire, addr);
    capture_after(EVENT_CXA_GUARD_ACQUIRE, &ev);
    return ev.ret;
}

INTERPOSE(int, __cxa_guard_release, void *addr)
{
    struct cxa_event ev = {.pc = INTERPOSE_PC, .addr = addr};
    capture_before(EVENT_CXA_GUARD_RELEASE, &ev);
    ev.ret = REAL(__cxa_guard_release, addr);
    capture_after(EVENT_CXA_GUARD_RELEASE, &ev);
    return ev.ret;
}

INTERPOSE(void, __cxa_guard_abort, void *addr)
{
    struct cxa_event ev = {.pc = INTERPOSE_PC, .addr = addr};
    capture_before(EVENT_CXA_GUARD_ABORT, &ev);
    REAL(__cxa_guard_abort, addr);
    capture_after(EVENT_CXA_GUARD_ABORT, &ev);
}

BINGO_MODULE_INIT()
