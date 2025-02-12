/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <assert.h>
#include <stdio.h>

#include <bingo/pubsub.h>

typedef union {
    struct {
        uint32_t index;
        chain_id chain;
        bool exclusive;
    } details;
    token_t as_token;
} timpl_t;

#define MAX_SUBSCRIPTIONS 1024

#define MODE_UNDEF   0
#define MODE_EXCL    1
#define MODE_NONEXCL 2

typedef struct {
    chain_id chain;
    ps_callback_f cb;
    int mode;
} subscription_t;

static bool _initd;
static unsigned int _next_subscription;
static subscription_t _subscriptions[MAX_SUBSCRIPTIONS + 1];

chain_id
chain_from(token_t token)
{
    timpl_t timpl;
    timpl.as_token = token;
    return timpl.details.chain;
}

token_t
ps_advertise(chain_id chain, bool exclusive)
{
    timpl_t timpl = {
        .details =
            {
                .index     = 0,
                .chain     = chain,
                .exclusive = exclusive,
            },
    };
    return timpl.as_token;
}

int
ps_subscribe(chain_id chain, ps_callback_f cb)
{
    assert(_next_subscription < MAX_SUBSCRIPTIONS);
    int idx             = _next_subscription++;
    _subscriptions[idx] = (subscription_t){.chain = chain, .cb = cb};
    return PS_SUCCESS;
}

int
ps_publish(token_t token, event_t event, const void *arg, void *ret)
{
    timpl_t timpl;
    timpl.as_token = token;

    if (!_initd)
        return PS_NOT_READY;

    for (unsigned int idx = 0, sidx = 0; idx < _next_subscription; idx++) {
        // if we find a subscription for the chain, we also check if the
        // subscription index (sidx) matches the token index. The token index
        // indicates where in the subscriber chain, the publication should
        // start.
        subscription_t subs = _subscriptions[idx];
        if (subs.chain != timpl.details.chain && subs.chain != ANY_CHAIN)
            continue;
        if (sidx++ < timpl.details.index)
            continue;

        // we pass the up-to-date token in the callback in case the subscriber
        // wants to republish to the remainder of this chain.
        timpl.details.index++;

        assert(subs.cb);

        // to enforce exclusive rights to publish in a chain, we mark the
        // subscriptions with exclusive or non-exclusive modes.
        int token_mode = timpl.details.exclusive ? MODE_EXCL : MODE_NONEXCL;
        if (subs.mode == MODE_UNDEF) {
            subs.mode = token_mode;
        } else if (subs.mode != token_mode) {
            perror("wrong token mode");
            return PS_INVALID;
        }

        // now we call the callback and abort the chain if the subscriber
        // "censors" the event by returning false.
        if (!subs.cb(timpl.as_token, event, arg, ret))
            return PS_SUCCESS;
    }
    return PS_SUCCESS;
}

BINGO_MODULE_INIT({ _initd = true; })
