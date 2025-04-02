/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <bingo/intercept.h>
#include <bingo/intercept/malloc.h>
#include <bingo/interpose.h>

INTERPOSE(void *, malloc, size_t n)
{
    struct malloc_event ev = {.size = n, .pc = INTERPOSE_PC};
    intercept_before(EVENT_MALLOC, &ev, 0);
    ev.ptr = REAL(malloc, n);
    intercept_after(EVENT_MALLOC, &ev, 0);
    return ev.ptr;
}

INTERPOSE(void *, calloc, size_t n, size_t s)
{
    struct malloc_event ev = {.size = n * s, .pc = INTERPOSE_PC};
    intercept_before(EVENT_CALLOC, &ev, 0);
    ev.ptr = REAL(calloc, n, s);
    intercept_after(EVENT_CALLOC, &ev, 0);
    return ev.ptr;
}

INTERPOSE(void *, realloc, void *p, size_t n)
{
    struct malloc_event ev = {.ptr = p, .size = n, .pc = INTERPOSE_PC};
    intercept_before(EVENT_REALLOC, &ev, 0);
    ev.ptr = REAL(realloc, p, n);
    intercept_after(EVENT_REALLOC, &ev, 0);
    return ev.ptr;
}

INTERPOSE(void, free, void *p)
{
    struct malloc_event ev = {.ptr = p, .pc = INTERPOSE_PC};
    intercept_before(EVENT_FREE, &ev, 0);
    REAL(free, p);
    intercept_after(EVENT_FREE, &ev, 0);
}

INTERPOSE(int, posix_memalign, void **memptr, size_t alignment, size_t size)
{
    struct malloc_event ev = {.ptr  = (void *)memptr,
                              .size = size,
                              .pc   = INTERPOSE_PC};
    intercept_before(EVENT_POSIX_MEMALIGN, &ev, 0);
    ev.ret = REAL(posix_memalign, memptr, alignment, size);
    intercept_after(EVENT_POSIX_MEMALIGN, &ev, 0);
    return ev.ret;
}

INTERPOSE(void *, aligned_alloc, size_t alignment, size_t size)
{
    struct malloc_event ev = {.size = size, .pc = INTERPOSE_PC};
    intercept_before(EVENT_ALIGNED_ALLOC, &ev, 0);
    ev.ptr = REAL(aligned_alloc, alignment, size);
    intercept_after(EVENT_ALIGNED_ALLOC, &ev, 0);
    return ev.ptr;
}

BINGO_MODULE_INIT()
