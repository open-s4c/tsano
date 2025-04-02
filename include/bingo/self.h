/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef BINGO_SELF_H
#define BINGO_SELF_H

#include <stddef.h>

#include <bingo/thread_id.h>

thread_id self_id(void);
void *self_tls(const void *global, size_t size);

#define SELF_TLS(global_ptr)                                                   \
    ((__typeof(global_ptr))self_tls((global_ptr), sizeof(*(global_ptr))))

#endif /* BINGO_SELF_H */
