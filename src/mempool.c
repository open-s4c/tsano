/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <bingo/mempool.h>
#include <stdlib.h>

int
mempool_init(size_t cap)
{
}
int
mempool_fini(void)
{
}

void *
mempool_malloc(size_t size)
{
    return malloc(size);
}

void *
mempool_calloc(size_t number, size_t size)
{
    return calloc(number, size);
}
void *
mempool_realloc(void *ptr, size_t size)
{
    return realloc(ptr, size);
}
void
mempool_free(void *ptr)
{
    return free(ptr);
}
