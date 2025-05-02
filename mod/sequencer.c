/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <bingo/capture/memaccess.h>
#include <bingo/log.h>
#include <bingo/module.h>
#include <bingo/self.h>
#include <bingo/switcher.h>

REGISTER_CALLBACK(CAPTURE_EVENT, ANY_TYPE, {
    switch (chain.type) {
        case EVENT_THREAD_FINI:
            switcher_wake(ANY_THREAD, 0);
            break;
        case EVENT_THREAD_INIT:
            /* threads call this only ONCE (except the main thread). */
            switcher_yield(self_id(self), true);
            break;
        default:
            break;
    }
    return PS_STOP;
})

REGISTER_CALLBACK(CAPTURE_BEFORE, ANY_TYPE, {
    switcher_wake(ANY_THREAD, 0);
    return PS_STOP;
})

REGISTER_CALLBACK(CAPTURE_AFTER, ANY_TYPE, {
    switcher_yield(self_id(self), true);
    return PS_STOP;
})

BINGO_MODULE_INIT()
