/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h> // qsort
#include <string.h>

#include <bingo/log.h>
#include <bingo/mempool.h>
#include <bingo/uset.h>

#define USET_INIT_SIZE 4
#define ASSERT         assert

void
uset_init_cap(uset_t *uset, size_t cap)
{
    ASSERT(uset);
    *uset = (uset_t){
        .items    = (uint64_t *)mempool_alloc(sizeof(uint64_t) * cap),
        .capacity = cap,
        .size     = 0,
    };
}

void
uset_init(uset_t *uset)
{
    uset_init_cap(uset, USET_INIT_SIZE);
}

void
uset_fini(uset_t *uset)
{
    ASSERT(uset);
    if (uset->items)
        mempool_free(uset->items);
    memset(uset, 0, sizeof(uset_t));
}

size_t
uset_size(const uset_t *uset)
{
    ASSERT(uset);
    return uset->size;
}

void
uset_clear(uset_t *uset)
{
    ASSERT(uset);
    uset->size = 0;
}

uint64_t
uset_get(const uset_t *uset, size_t idx)
{
    ASSERT(uset);
    if (uset->size == 0)
        return 0;
    ASSERT(uset->items);

    if (idx >= uset->size)
        return 0;

    return uset->items[idx];
}

void
uset_set(uset_t *uset, size_t idx, uint64_t id)
{
    ASSERT(uset);
    if (uset->size == 0)
        return;

    ASSERT(uset->items);
    if (idx >= uset->size)
        return;

    for (size_t i = 0; i < uset->size; i++) {
        if (uset->items[i] == id && i != idx) {
            uset->items[i] = uset->items[idx];
            break;
        }
    }

    uset->items[idx] = id;
}

bool
uset_remove(uset_t *uset, uint64_t id)
{
    ASSERT(uset);
    ASSERT(uset->size == 0 || uset->items);

    for (size_t i = 0; i < uset->size; i++) {
        if (uset->items[i] != id)
            continue;
        uset->items[i] = uset->items[--uset->size];
        return true;
    }
    return false;
}

bool
uset_has(const uset_t *uset, uint64_t id)
{
    ASSERT(uset);

    for (size_t i = 0; i < uset->size; i++)
        if (uset->items[i] == id)
            return true;
    return false;
}

void
uset_expand(uset_t *uset, size_t cap)
{
    ASSERT(uset);
    if (cap == 0)
        cap = USET_INIT_SIZE;

    if (uset->items == NULL) {
        uset_init_cap(uset, cap);
        return;
    }

    uset_t tmp = {0};
    uset_init_cap(&tmp, cap);
    memcpy(tmp.items, uset->items, uset->size * sizeof(uint64_t));
    mempool_free(uset->items);

    uset->capacity = tmp.capacity;
    uset->items    = tmp.items;
}

bool
uset_insert(uset_t *uset, uint64_t id)
{
    ASSERT(uset);
    ASSERT(uset->size == 0 || uset->items);
    if (uset_has(uset, id)) {
        return false;
    }

    if (uset->size + 1 > uset->capacity) {
        uset_expand(uset, uset->capacity * 2);
    }

    uset->items[uset->size++] = id;
    return true;
}

void
uset_copy(uset_t *dst, const uset_t *src)
{
    ASSERT(dst);
    ASSERT(src);
    if (dst->capacity < src->capacity)
        uset_expand(dst, src->capacity);
    dst->size = src->size;
    memcpy(dst->items, src->items, sizeof(uint64_t) * src->size);
}

void
uset_sort(uset_t *uset, uset_cmp_f cmp)
{
    qsort(uset->items, uset->size, sizeof(uint64_t), cmp);
}

void
uset_intersect(uset_t *uset1, const uset_t *uset2)
{
    ASSERT(uset1);
    ASSERT(uset2);

    for (size_t i = 0; i < uset1->size;) {
        if (uset_has(uset2, uset1->items[i])) {
            i++;
            continue;
        }
        uset1->items[i] = uset1->items[--uset1->size];
    }
}

void
uset_subtract(uset_t *uset1, const uset_t *uset2)
{
    ASSERT(uset1);
    ASSERT(uset2);

    for (size_t i = 0; i < uset1->size;) {
        if (!uset_has(uset2, uset1->items[i])) {
            i++;
            continue;
        }
        uset1->items[i] = uset1->items[--uset1->size];
    }
}

void
uset_union(uset_t *uset1, const uset_t *uset2)
{
    ASSERT(uset1);
    ASSERT(uset2);

    for (size_t i = 0; i < uset2->size; i++) {
        uset_insert(uset1, uset_get(uset2, i));
    }
}

void
uset_filter(uset_t *uset, bool (*predicate)(uint64_t))
{
    ASSERT(uset);
    ASSERT(predicate);
    for (size_t i = 0; i < uset->size;) {
        if (predicate(uset->items[i])) {
            i++;
            continue;
        }
        uset->items[i] = uset->items[--uset->size];
    }
}

bool
uset_contains_all(const uset_t *uset1, const uset_t *uset2)
{
    ASSERT(uset1);
    ASSERT(uset2);

    if (uset2->size == 0)
        return true;

    if (uset1->size == 0)
        return false;

    for (size_t i = 0; i < uset2->size; i++) {
        if (!uset_has(uset1, uset2->items[i])) {
            return false;
        }
    }

    return true;
}

bool
uset_equals(const uset_t *uset1, const uset_t *uset2)
{
    ASSERT(uset1);
    ASSERT(uset2);

    return uset_contains_all(uset1, uset2) && uset_contains_all(uset2, uset1);
}

void
uset_make_first(uset_t *uset, uint64_t id)
{
    ASSERT(uset);

    for (size_t i = 0; i < uset_size(uset); i++) {
        if (uset->items[i] != id)
            continue;
        if (i == 0)
            return;
        uset->items[i] = uset->items[0];
        uset->items[0] = id;
        return;
    }
    ASSERT(0 && "the element does not exist");
}

void
uset_print(const uset_t *uset)
{
    ASSERT(uset);
    log_printf("[");
    for (size_t i = 0; i < uset->size; i++) {
        if (i != 0)
            log_printf(", ");
        log_printf("%lu", uset->items[i]);
    }
    log_printf("]\n");
}


int
uset_cmp_lt(const void *a, const void *b)
{
    const uint64_t ta = *(const uint64_t *)a;
    const uint64_t tb = *(const uint64_t *)b;

    return ta < tb;
}

int
uset_cmp_gt(const void *a, const void *b)
{
    const uint64_t ta = *(const uint64_t *)a;
    const uint64_t tb = *(const uint64_t *)b;

    return ta > tb;
}
