/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include "core.h"

#include <assert.h>
#include <bingo/pubsub.h>
#include <stdio.h>

#define MAX_SUBSCRIPTIONS 512

struct sub {
    hook_id hook;
    ps_callback_f cb;
};

struct type {
    size_t count;
    struct sub subs[MAX_SUBSCRIPTIONS + 1];
};

struct hook {
    struct type types[MAX_TYPES];
};

static bool _initd;
static struct hook _hooks[MAX_HOOKS];

static inline int
_ps_subscribe_type(hook_id hook, type_id type, ps_callback_f cb)
{
    if (type == 0 || type > MAX_TYPES)
        return PS_INVALID;

    // calculate indices
    size_t hook_idx = hook - 1;
    size_t type_idx = type - 1;

    struct type *ev = &_hooks[hook_idx].types[type_idx];
    size_t subs_idx = ev->count;
    if (subs_idx >= MAX_SUBSCRIPTIONS)
        return PS_INVALID;

    // increment the idx of next subscription
    ev->count++;

    // register subscription
    ev->subs[subs_idx] = (struct sub){.hook = hook, .cb = cb};

    return PS_SUCCESS;
}

static inline int
_ps_subscribe_hook(hook_id hook, type_id type, ps_callback_f cb)
{
    if (hook == 0 || hook > MAX_HOOKS)
        return PS_INVALID;
    if (type != ANY_TYPE)
        return _ps_subscribe_type(hook, type, cb);

    int err;
    for (size_t i = 1; i <= MAX_TYPES; i++)
        if ((err = _ps_subscribe_type(hook, i, cb)) != PS_SUCCESS)
            return err;

    return PS_SUCCESS;
}


#ifndef BINGO_PS_EXTERNAL
BINGO_HIDE int
_ps_publish_do(chain_t chain, void *event, metadata_t *md)
{
    size_t hook_idx = chain.hook - 1;
    size_t type_idx = chain.type - 1;
    struct type *ev = &_hooks[hook_idx].types[type_idx];

    for (size_t idx = 0; idx < ev->count; idx++) {
        struct sub *subs = &ev->subs[idx];
        // now we call the callback and abort the hook if the subscriber
        // "censors" the type by returning PS_STOP.
        int err = subs->cb(chain, event, md);
        if (err != PS_SUCCESS)
            return err;
    }
    return PS_SUCCESS;
}
#endif


BINGO_HIDE int
_ps_publish(chain_t chain, void *event, metadata_t *md)
{
    if (!_initd)
        return PS_DROP;
    if (chain.hook == ANY_HOOK || chain.hook >= MAX_HOOKS)
        return PS_INVALID;
    if (chain.type == ANY_TYPE || chain.type >= MAX_TYPES)
        return PS_INVALID;

    return _ps_publish_do(chain, event, md);
}

BINGO_HIDE void
_ps_init()
{
    _initd = true;
}

// -----------------------------------------------------------------------------
// public interface
// -----------------------------------------------------------------------------

BINGO_HIDE_IF int
ps_subscribe(hook_id hook, type_id type, ps_callback_f cb)
{
    assert(cb != NULL);
    if (hook != ANY_HOOK)
        return _ps_subscribe_hook(hook, type, cb);

    int err;
    for (size_t i = 1; i <= MAX_HOOKS; i++)
        if ((err = _ps_subscribe_hook(i, type, cb)) != PS_SUCCESS)
            return err;

    return PS_SUCCESS;
}

BINGO_HIDE_IF int
ps_publish(chain_t chain, void *event, metadata_t *self)
{
    return _ps_publish(chain, event, self);
}
