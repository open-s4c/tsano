/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <assert.h>
#include <stdio.h>

#include "core.h"
#include <bingo/pubsub.h>

#define MAX_SUBSCRIPTIONS 512

struct sub {
    chain_id chain;
    ps_callback_f cb;
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

static inline token_t
next_index(token_t token)
{
    return (token_t){.details = {
                         .chain = chain_from(token),
                         .event = event_from(token),
                         .index = index_from(token) + 1,
                     }};
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


#ifndef BINGO_PS_EXTERNAL
BINGO_HIDE int
_ps_publish_do(token_t token, const void *arg, self_t *self)
{
    chain_id chain   = chain_from(token);
    event_id event   = event_from(token);
    size_t start_idx = index_from(token);

    size_t chain_idx = chain - 1;
    size_t event_idx = event - 1;
    struct event *ev = &_chains[chain_idx].events[event_idx];

    // There is only one subscription group that matches a (chain, event) pair.
    // By design, if a subscriber at index sidx uses the token given to the
    // callback to "republish", then only the subscriptions after sidx will
    // receive the republication. See republish function */

    for (size_t idx = start_idx; idx < ev->count; idx++) {
        struct sub *subs = &ev->subs[idx];

        // now we call the callback and abort the chain if the subscriber
        // "censors" the event by returning false.
        if (!subs->cb(token, arg, self))
            return PS_SUCCESS;

        // we increment token index to mark current subscriber in case
        // subscriber wants to republish to the remainder of this chain.
        token.opaque = next_index(token).opaque;
    }
    return PS_SUCCESS;
}
#endif


BINGO_HIDE int
_ps_publish(token_t token, const void *arg, self_t *self)
{
    chain_id chain = chain_from(token);
    event_id event = event_from(token);

    if (!_initd)
        return PS_DROP;
    if (chain == ANY_CHAIN || chain >= MAX_CHAINS)
        return PS_INVALID;
    if (event == ANY_EVENT || event >= MAX_EVENTS)
        return PS_INVALID;
    if (chain == CHAIN_NULL)
        return PS_SUCCESS;

#ifdef BINGO_PS_DIRECT_SELF
    assert(index_from(token) == 0);
    _self_handle(token, arg, self);
    return PS_SUCCESS;
#else
    return _ps_publish_do(token, arg, self);
#endif
}

BINGO_HIDE int
_ps_republish(token_t token, const void *arg, self_t *self)
{
    return _ps_publish_do(next_index(token), arg, self);
}

BINGO_HIDE void
_ps_init()
{
    _initd = true;
}

// -----------------------------------------------------------------------------
// public interface
// -----------------------------------------------------------------------------

int
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

int
ps_republish(token_t token, const void *arg, self_t *self)
{
    return _ps_republish(token, arg, self);
}

int
ps_publish(token_t token, const void *arg, self_t *self)
{
    return _ps_publish(token, arg, self);
}
