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
_ps_publish_do(token_t token, void *event, self_t *self)
{
    hook_id hook     = hook_from(token);
    type_id type     = type_from(token);
    size_t start_idx = index_from(token);

    size_t hook_idx = hook - 1;
    size_t type_idx = type - 1;
    struct type *ev = &_hooks[hook_idx].types[type_idx];

    // There is only one subscription group that matches a (hook, type) pair.
    // By design, if a subscriber at index sidx uses the token given to the
    // callback to "republish", then only the subscriptions after sidx will
    // receive the republication. See republish function */

    for (size_t idx = start_idx; idx < ev->count; idx++) {
        struct sub *subs = &ev->subs[idx];

        // now we call the callback and abort the hook if the subscriber
        // "censors" the type by returning PS_STOP.
        int err = subs->cb(token, event, self);
        if (err != PS_SUCCESS)
            return err;

        // we increment token index to mark current subscriber in case
        // subscriber wants to republish to the remainder of this hook.
        token.opaque = next_index(token).opaque;
    }
    return PS_SUCCESS;
}
#endif


BINGO_HIDE int
_ps_publish(token_t token, void *event, self_t *self)
{
    hook_id hook = hook_from(token);
    type_id type = type_from(token);

    if (!_initd)
        return PS_DROP;
    if (hook == ANY_HOOK || hook >= MAX_HOOKS)
        return PS_INVALID;
    if (type == ANY_TYPE || type >= MAX_TYPES)
        return PS_INVALID;

#ifdef BINGO_PS_DIRECT_SELF
    assert(index_from(token) == 0);
    return _self_handle(token, event, self);
#else
    return _ps_publish_do(token, event, self);
#endif
}

BINGO_HIDE int
_ps_republish(token_t token, void *event, self_t *self)
{
    return _ps_publish_do(next_index(token), event, self);
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

int
ps_republish(token_t token, void *event, self_t *self)
{
    return _ps_republish(token, event, self);
}

int
ps_publish(token_t token, void *event, self_t *self)
{
    return _ps_publish(token, event, self);
}
