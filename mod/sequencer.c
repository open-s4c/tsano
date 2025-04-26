/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <bingo/capture/memaccess.h>
#include <bingo/log.h>
#include <bingo/module.h>
#include <bingo/self.h>
#include <bingo/switcher.h>

REGISTER_CALLBACK(CAPTURE_EVENT, ANY_EVENT, {
    switch (token.event) {
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
    return false;
})

REGISTER_CALLBACK(CAPTURE_BEFORE, ANY_EVENT, {
    switcher_wake(ANY_THREAD, 0);
    return false;
})

REGISTER_CALLBACK(CAPTURE_AFTER, ANY_EVENT, {
    switcher_yield(self_id(self), true);
    return false;
})

BINGO_MODULE_INIT()
