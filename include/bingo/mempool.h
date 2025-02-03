/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef BINGO_MEMPOOL_H
#define BINGO_MEMPOOL_H

#include <stddef.h>

int mempool_init(size_t cap);
int mempool_fini(void);

void *mempool_malloc(size_t size);
void *mempool_calloc(size_t number, size_t size);
void *mempool_realloc(void *ptr, size_t size);
void mempool_free(void *ptr);

#endif
