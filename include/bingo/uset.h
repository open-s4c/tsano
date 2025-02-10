/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef BINGO_USET_H
#define BINGO_USET_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct uset {
    size_t capacity;
    size_t size;
    uint64_t *items;
} uset_t;

void uset_init_cap(uset_t *uset, size_t cap);
void uset_init(uset_t *uset);
void uset_fini(uset_t *uset);

size_t uset_size(const uset_t *uset);
void uset_clear(uset_t *uset);
uint64_t uset_get(const uset_t *uset, size_t idx);
void uset_set(uset_t *uset, size_t idx, uint64_t id);
bool uset_remove(uset_t *uset, uint64_t id);
bool uset_has(const uset_t *uset, uint64_t id);
void uset_expand(uset_t *uset, size_t cap);
bool uset_insert(uset_t *uset, uint64_t id);
void uset_copy(uset_t *dst, const uset_t *src);
bool uset_equals(const uset_t *uset1, const uset_t *uset2);

typedef int (*uset_cmp_f)(const void *a, const void *b);
int uset_cmp_lt(const void *a, const void *b);
int uset_cmp_gt(const void *a, const void *b);
void uset_sort(uset_t *uset, uset_cmp_f cmp);

void uset_union(uset_t *uset1, const uset_t *uset2);
void uset_intersect(uset_t *uset1, const uset_t *uset2);
void uset_subtract(uset_t *uset1, const uset_t *uset2);
void uset_filter(uset_t *uset, bool (*predicate)(uint64_t));
bool uset_contains_all(const uset_t *uset1, const uset_t *uset2);

void uset_make_first(uset_t *uset, uint64_t id);
void uset_print(const uset_t *uset);

#endif /* BINGO_USET_H */
