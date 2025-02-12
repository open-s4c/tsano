/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef BINGO_SELF_H
#define BINGO_SELF_H

#include <stdint.h>

typedef uint64_t task_id;
#define NO_TASK  ((task_id)0)
#define ANY_TASK (~NO_TASK)

typedef struct {
    task_id tid;
    const void *arg;
} self_event_t;

#endif /* BINGO_SELF_H */
