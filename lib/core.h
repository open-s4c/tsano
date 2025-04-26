/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef BINGO_CORE_H
#define BINGO_CORE_H

#define BINGO_XTOR_PRIO 200
#include <bingo/module.h>
#include <bingo/pubsub.h>

void _ps_init(void);
int _ps_republish(token_t token, const void *arg, self_t *self);
int _ps_publish(token_t token, const void *arg, self_t *self);
int _ps_publish_do(token_t token, const void *arg, self_t *self);

void _self_init(void);
void _self_fini(void);
void _self_handle(token_t token, const void *arg, self_t *self);

void mempool_init(size_t cap);
void mempool_fini(void);

#endif /* BINGO_CORE_H */
