/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <bingo/intercept/memaccess.h>
#include <bingo/log.h>
#include <bingo/pubsub.h>
#include <bingo/self.h>
#include <bingo/switcher.h>

PS_SUBSCRIBE(INTERCEPT_AT, {
    switch (event) {
        case EVENT_THREAD_FINI:
            switcher_wake(ANY_THREAD, 0);
            break;
        case EVENT_THREAD_INIT:
            /* threads call this only ONCE (except the main thread). */
            switcher_yield(self_id(), true);
            break;
        default:
            break;
    }
    return false;
})

PS_SUBSCRIBE(INTERCEPT_BEFORE, {
    switcher_wake(ANY_THREAD, 0);
    return false;
})

PS_SUBSCRIBE(INTERCEPT_AFTER, {
    switcher_yield(self_id(), true);
    return false;
})

BINGO_MODULE_INIT()
