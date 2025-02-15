/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef BINGO_MEMACCESS_H
#define BINGO_MEMACCESS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define _EVENT_MA_START 10000

#define EVENT_MA_READ         (0 + _EVENT_MA_START)
#define EVENT_MA_WRITE        (1 + _EVENT_MA_START)
#define EVENT_MA_AREAD        (2 + _EVENT_MA_START)
#define EVENT_MA_AWRITE       (3 + _EVENT_MA_START)
#define EVENT_MA_RMW          (4 + _EVENT_MA_START)
#define EVENT_MA_XCHG         (5 + _EVENT_MA_START)
#define EVENT_MA_CMPXCHG      (6 + _EVENT_MA_START)
#define EVENT_MA_CMPXCHG_WEAK (7 + _EVENT_MA_START)
#define EVENT_MA_FENCE        (8 + _EVENT_MA_START)

typedef struct memaccess {
    const char *func;
    uintptr_t pc;
    uintptr_t addr;
    size_t size;
    union {
        uint8_t argu8;
        uint16_t argu16;
        uint32_t argu32;
        uint64_t argu64;
        __uint128_t argu128;
    };
    bool failed; // optional argument to indicate operation failed
} memaccess_t;

#endif /* BINGO_MEMACCESS_H */
