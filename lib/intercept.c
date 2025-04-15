/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <stdio.h>
#include <stdlib.h>

#define BINGO_XTOR_PRIO 201
#include <bingo/intercept.h>
#include <bingo/module.h>
#include <bingo/pubsub.h>

static token_t _token[] = {
    [INTERCEPT_BEFORE] = 0,
    [INTERCEPT_AFTER]  = 0,
    [INTERCEPT_AT]     = 0,
};

BINGO_MODULE_INIT({
    /* Advertise chains with exlusive tokens. That ensures only the functions in
     * this file can publish to these chain. */
    _token[INTERCEPT_BEFORE] = ps_advertise(INTERCEPT_BEFORE, true);
    _token[INTERCEPT_AFTER]  = ps_advertise(INTERCEPT_AFTER, true);
    _token[INTERCEPT_AT]     = ps_advertise(INTERCEPT_AT, true);
})

static void
_intercept(chain_id chain, event_id event, const void *arg, void *ret)
{
    int err = ps_publish(_token[chain], event, arg, ret);

    if (err != PS_SUCCESS && err != PS_NOT_READY) {
        perror("intercept publish");
        exit(EXIT_FAILURE);
    }
}

BINGO_HIDE void
intercept_before(event_id event, const void *arg, void *ret)
{
    _intercept(INTERCEPT_BEFORE, event, arg, ret);
}

BINGO_HIDE void
intercept_after(event_id event, const void *arg, void *ret)
{
    _intercept(INTERCEPT_AFTER, event, arg, ret);
}

BINGO_HIDE void
intercept_at(event_id event, const void *arg, void *ret)
{
    _intercept(INTERCEPT_AT, event, arg, ret);
}
