/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <bingo/intercept.h>
#include <bingo/pubsub.h>

static token_t _token[] = {
    [INTERCEPT_BEFORE] = 0,
    [INTERCEPT_AFTER]  = 0,
    [INTERCEPT_AT]     = 0,
};

BINGO_MODULE_INIT({
    _token[INTERCEPT_BEFORE] = ps_advertise(INTERCEPT_BEFORE, true);
    _token[INTERCEPT_AFTER]  = ps_advertise(INTERCEPT_AFTER, true);
    _token[INTERCEPT_AT]     = ps_advertise(INTERCEPT_AT, true);
})

static void
_intercept(chain_id chain, kind_t kind, const void *arg, void *ret)
{
    int rv = ps_publish(_token[chain], kind, arg, ret);
    assert(rv == 0);
}

void
intercept_before(kind_t kind, const void *arg, void *ret)
{
    _intercept(INTERCEPT_BEFORE, kind, arg, ret);
}

void
intercept_after(kind_t kind, const void *arg, void *ret)
{
    _intercept(INTERCEPT_AFTER, kind, arg, ret);
}

void
intercept_at(kind_t kind, const void *arg, void *ret)
{
    _intercept(INTERCEPT_AT, kind, arg, ret);
}
