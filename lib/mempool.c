/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BINGO_XTOR_PRIO 200
#include <bingo/mempool.h>
#include <bingo/module.h>
#include <vsync/spinlock/caslock.h>

const size_t mempool_size = 1024 * 1024 * 200;
static size_t _sizes[]    = {32,
                             128,
                             512,
                             1024,
                             2048,
                             8192,
                             1 * 1024 * 1024,
                             4 * 1024 * 1024,
                             8 * 1024 * 1024};
#define NSTACKS (sizeof(_sizes) / sizeof(size_t))

unsigned int
_bucketize(size_t size)
{
    unsigned int i = 0;
    for (; i < NSTACKS && size > _sizes[i]; i++)
        ;
    assert(i < NSTACKS);
    return i;
}

typedef struct entry {
    struct entry *next;
    size_t size;
    char data[];
} entry_t;

typedef struct alloc {
    size_t capacity;
    size_t next;
    char *memory;
} alloc_t;

typedef struct mempool {
    caslock_t lock;
    size_t allocated;
    struct alloc pool;
    entry_t *stack[NSTACKS];
} mempool_t;

static mempool_t _mp;

static void
_mempool_init(size_t cap)
{
    _mp.allocated = 0;
    memset(&_mp.stack, 0, sizeof(entry_t *) * NSTACKS);
    _mp.pool.memory = malloc(cap);
    assert(_mp.pool.memory);
    memset(_mp.pool.memory, 0, cap);
    _mp.pool.capacity = cap;
    _mp.pool.next     = 0;
    caslock_init(&_mp.lock);
}

static void
_mempool_fini()
{
    if (_mp.pool.memory)
        free(_mp.pool.memory);
}

void *
mempool_alloc(size_t n)
{
    mempool_t *mp   = &_mp;
    entry_t *e      = NULL;
    size_t size     = n + sizeof(entry_t);
    unsigned bucket = _bucketize(size);
    size            = _sizes[bucket];
    entry_t **stack = &mp->stack[bucket];
    assert(stack);

    // Mempool is used from rogue thread, serialization is necessary
    caslock_acquire(&mp->lock);

    if (*stack) {
        e       = *stack;
        *stack  = e->next;
        e->next = NULL;
        mp->allocated += size;
    }

    if (e == NULL && mp->pool.capacity >= mp->pool.next + size) {
        e       = (entry_t *)(mp->pool.memory + mp->pool.next);
        e->next = NULL;
        e->size = n;
        mp->pool.next += size;
        mp->allocated += size;
    }
    caslock_release(&mp->lock);
    return e ? e->data : NULL;
}

void *
mempool_realloc(void *ptr, size_t size)
{
    void *p = mempool_alloc(size);
    if (!p || !ptr)
        return p;
    entry_t *e = (entry_t *)ptr - 1;
    size       = e->size < size ? e->size : size;
    memcpy(p, ptr, size);
    free(ptr);
    return p;
}

void
mempool_free(void *ptr)
{
    mempool_t *mp = &_mp;
    assert(ptr);
    entry_t *e      = (entry_t *)ptr - 1;
    size_t size     = e->size + sizeof(entry_t);
    unsigned bucket = _bucketize(size);
    size            = _sizes[bucket];
    entry_t **stack = &mp->stack[bucket];

    // Mempool is used from rogue thread, serialization is necessary
    caslock_acquire(&mp->lock);
    mp->allocated -= size;
    assert(stack);
    e->next = *stack;
    *stack  = e;
    caslock_release(&mp->lock);
}

BINGO_MODULE_INIT({ _mempool_init(mempool_size); })

BINGO_MODULE_FINI({
    // log_debugf("allocated memory on fini: %lu\n", _mp.allocated);
    _mempool_fini();
})
