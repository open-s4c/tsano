/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <pthread.h>

#include <bingo/log.h>
#include <bingo/pubsub.h>

#define ADD_LOGGER(CHAIN)                                                      \
    PS_SUBSCRIBE(CHAIN, {                                                      \
        char *chain = #CHAIN;                                                  \
        log_printf("%s\t%u[%" PRIx64 "]\n", chain, event,                      \
                   (uint64_t)pthread_self());                                  \
    })

ADD_LOGGER(INTERCEPT_BEFORE)
ADD_LOGGER(INTERCEPT_AFTER)
ADD_LOGGER(INTERCEPT_AT)

BINGO_MODULE_INIT()
