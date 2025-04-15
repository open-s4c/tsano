/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <assert.h>
#include <stdio.h>

#define BINGO_XTOR_PRIO 202
#include <bingo/module.h>
#include <bingo/pubsub.h>

typedef union {
    struct {
        uint32_t index;
        chain_id chain;
        bool exclusive;
    } details;
    token_t as_token;
} timpl_t;

#define MAX_SUBSCRIPTIONS 512

#define MODE_UNDEF   0
#define MODE_EXCL    1
#define MODE_NONEXCL 2

struct sub {
    chain_id chain;
    ps_callback_f cb;
    int mode;
};

struct event {
    size_t count;
    struct sub subs[MAX_SUBSCRIPTIONS + 1];
};

struct chain {
    struct event events[MAX_EVENTS];
};

static bool _initd;
static struct chain _chains[MAX_CHAINS];

BINGO_HIDE chain_id
chain_from(token_t token)
{
    timpl_t timpl;
    timpl.as_token = token;
    return timpl.details.chain;
}

BINGO_HIDE token_t
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

static inline int
_ps_subscribe_event(chain_id chain, event_id event, ps_callback_f cb)
{
    if (event == 0 || event > MAX_EVENTS)
        return PS_INVALID;

    // calculate indices
    size_t chain_idx = chain - 1;
    size_t event_idx = event - 1;

    struct event *ev = &_chains[chain_idx].events[event_idx];
    size_t subs_idx  = ev->count;
    if (subs_idx >= MAX_SUBSCRIPTIONS)
        return PS_INVALID;

    // increment the idx of next subscription
    ev->count++;

    // register subscription
    ev->subs[subs_idx] = (struct sub){.chain = chain, .cb = cb};

    return PS_SUCCESS;
}

static inline int
_ps_subscribe_chain(chain_id chain, event_id event, ps_callback_f cb)
{
    if (chain == 0 || chain > MAX_CHAINS)
        return PS_INVALID;
    if (event != ANY_EVENT)
        return _ps_subscribe_event(chain, event, cb);

    int err;
    for (size_t i = 1; i <= MAX_EVENTS; i++)
        if ((err = _ps_subscribe_event(chain, i, cb)) != PS_SUCCESS)
            return err;

    return PS_SUCCESS;
}

BINGO_HIDE int
ps_subscribe(chain_id chain, event_id event, ps_callback_f cb)
{
    assert(cb != NULL);
    if (chain != ANY_CHAIN)
        return _ps_subscribe_chain(chain, event, cb);

    int err;
    for (size_t i = 1; i <= MAX_CHAINS; i++)
        if ((err = _ps_subscribe_chain(i, event, cb)) != PS_SUCCESS)
            return err;

    return PS_SUCCESS;
}

BINGO_WEAK BINGO_HIDE int
ps_publish(token_t token, event_id event, const void *arg, void *ret)
{
    timpl_t timpl;
    timpl.as_token   = token;
    chain_id chain   = timpl.details.chain;
    size_t start_idx = timpl.details.index;

    if (!_initd)
        return PS_NOT_READY;
    if (chain == ANY_CHAIN || chain >= MAX_CHAINS)
        return PS_INVALID;
    if (event == ANY_EVENT || event >= MAX_EVENTS)
        return PS_INVALID;
    size_t chain_idx = chain - 1;
    size_t event_idx = event - 1;
    struct event *ev = &_chains[chain_idx].events[event_idx];
    // There is only one subscription group that matches a (chain, event) pair.
    // By design, if a subscriber at index sidx uses the token given to the
    // callback to "republish", then only the subscriptions after sidx will
    // receive the republication.

    for (size_t idx = start_idx; idx < ev->count; idx++) {
        struct sub *subs = &ev->subs[idx];

        // we pass the up-to-date token in the callback in case the
        // subscriber wants to republish to the remainder of this chain.
        timpl.details.index++;

        // to enforce exclusive rights to publish in a chain, we mark the
        // subscriptions with exclusive or non-exclusive modes.
        int token_mode = timpl.details.exclusive ? MODE_EXCL : MODE_NONEXCL;
        if (subs->mode == MODE_UNDEF) {
            subs->mode = token_mode;
        } else if (subs->mode != token_mode) {
            perror("wrong token mode");
            return PS_INVALID;
        }

        // now we call the callback and abort the chain if the subscriber
        // "censors" the event by returning false.
        if (!subs->cb(timpl.as_token, event, arg, ret))
            return PS_SUCCESS;
    }
    return PS_SUCCESS;
}

BINGO_MODULE_INIT({ _initd = true; })
