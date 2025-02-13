/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef BINGO_TASK_ID_H
#define BINGO_TASK_ID_H

#include <stdint.h>

typedef uint64_t task_id;
#define NO_TASK  ((task_id)0)
#define ANY_TASK (~NO_TASK)

#define EVENT_TASK_INIT 100
#define EVENT_TASK_FINI 101

#endif /* BINGO_TASK_ID_H */
