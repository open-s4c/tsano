/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef BINGO_THREAD_ID_H
#define BINGO_THREAD_ID_H

#include <stdint.h>

typedef uint64_t thread_id;
#define NO_THREAD  ((thread_id)0)
#define ANY_THREAD (~NO_THREAD)

#define EVENT_THREAD_INIT 100
#define EVENT_THREAD_FINI 101

#endif /* BINGO_THREAD_ID_H */
