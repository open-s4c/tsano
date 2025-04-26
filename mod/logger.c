/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <pthread.h>

#include <bingo/capture.h>
#include <bingo/log.h>
#include <bingo/module.h>

REGISTER_CALLBACK(
    ANY_CHAIN, ANY_EVENT,
    {
        // log_printf("%" PRIx64 "\t%u[%" PRIx64 "]\n", token._v, event,
        //         (uint64_t)pthread_self());
    })


BINGO_MODULE_INIT()
