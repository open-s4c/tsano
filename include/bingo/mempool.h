/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef BINGO_MEMPOOL_H
#define BINGO_MEMPOOL_H

#include <stddef.h>

void *mempool_alloc(size_t size);
void *mempool_realloc(void *ptr, size_t size);
void mempool_free(void *ptr);

#endif /* BINGO_MEMPOOL_H */
