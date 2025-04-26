/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <bingo/capture/malloc.h>
#include <bingo/interpose.h>

INTERPOSE(void *, malloc, size_t n)
{
    struct malloc_event ev = {.size = n, .pc = INTERPOSE_PC};
    capture_before(EVENT_MALLOC, &ev);
    ev.ptr = REAL(malloc, n);
    capture_after(EVENT_MALLOC, &ev);
    return ev.ptr;
}

INTERPOSE(void *, calloc, size_t n, size_t s)
{
    struct malloc_event ev = {.size = n * s, .pc = INTERPOSE_PC};
    capture_before(EVENT_CALLOC, &ev);
    ev.ptr = REAL(calloc, n, s);
    capture_after(EVENT_CALLOC, &ev);
    return ev.ptr;
}

INTERPOSE(void *, realloc, void *p, size_t n)
{
    struct malloc_event ev = {.ptr = p, .size = n, .pc = INTERPOSE_PC};
    capture_before(EVENT_REALLOC, &ev);
    ev.ptr = REAL(realloc, p, n);
    capture_after(EVENT_REALLOC, &ev);
    return ev.ptr;
}

INTERPOSE(void, free, void *p)
{
    struct malloc_event ev = {.ptr = p, .pc = INTERPOSE_PC};
    capture_before(EVENT_FREE, &ev);
    REAL(free, p);
    capture_after(EVENT_FREE, &ev);
}

INTERPOSE(int, posix_memalign, void **memptr, size_t alignment, size_t size)
{
    struct malloc_event ev = {.ptr  = (void *)memptr,
                              .size = size,
                              .pc   = INTERPOSE_PC};
    capture_before(EVENT_POSIX_MEMALIGN, &ev);
    ev.ret = REAL(posix_memalign, memptr, alignment, size);
    capture_after(EVENT_POSIX_MEMALIGN, &ev);
    return ev.ret;
}

INTERPOSE(void *, aligned_alloc, size_t alignment, size_t size)
{
    struct malloc_event ev = {.size = size, .pc = INTERPOSE_PC};
    capture_before(EVENT_ALIGNED_ALLOC, &ev);
    ev.ptr = REAL(aligned_alloc, alignment, size);
    capture_after(EVENT_ALIGNED_ALLOC, &ev);
    return ev.ptr;
}

BINGO_MODULE_INIT()
