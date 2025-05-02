/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 * -----------------------------------------------------------------------------
 * @file pubsub.h
 * @brief Interruptable chain-based pubsub
 *
 * This component provides a chain-based pubsub. When an event is published to a
 * chain (aka topic), handlers are called back in the order of subscription.
 * Each handler has the option of interruping the chain by returning false.
 *
 * Events are have a type (`type_id`) and have an argument `arg`. A chain is a
 * pair given by a `hook_id` and a `type_id`. This pubsub uses the concept of
 * chains. To publish to a chain, you first have to retrieve a chain for the
 * chain. A chain might be exlusive, making other publishers to fail in runtime.
 *
 * ## Naming
 *
 * For a lack of better name, this component is called `pubsub`. It combines
 * aspects of several GoF design patterns such as pubsub, observer, and
 * chain-of-responsibility pattern. However, it violates some requirements of
 * each aspect such that no name seem to perfectly match it.
 *
 */
#ifndef BINGO_PUBSUB_H
#define BINGO_PUBSUB_H
#include <stdbool.h>
#include <stdint.h>

/* type_id represents the type of an event. */
typedef uint16_t type_id;

/* MAX_TYPES determines the maximum number of event types. */
#define MAX_TYPES 512

/* ANY_TYPE indicates any event type. */
#define ANY_TYPE 0

/* hook_id identifies a subscriber group ordered by the subscription time. */
typedef uint16_t hook_id;

/* MAX_HOOKS determines the maximum number of hooks */
#define MAX_HOOKS 16

/* ANY_HOOK indicates any hook. */
#define ANY_HOOK 0

typedef struct self self_t;

/* chain_t is a pair of hook and type and works as a pubsub topic. */
typedef struct chain {
    hook_id hook;
    type_id type;
} chain_t;

/* Initializes a chain object */
static inline chain_t
as_chain(hook_id hook, type_id type)
{
    return (chain_t){
        .hook = hook,
        .type = type,
    };
}

/* Context/self opaque metadata */
typedef struct self self_t;

#define PS_SUCCESS 0
#define PS_STOP    1
#define PS_DROP    2
/* Errors start at 8. Further details are set in higher bits */
#define PS_ERROR   8
#define PS_INVALID (PS_ERROR | (PS_ERROR << 1))

/* ps_callback_f is the subscriber interface.
 *
 * Callbacks can return the following codes:
 * - PS_SUCCESS: event handled successfully
 * - PS_STOP: event handled successfully, but chain should be interrupted
 * - PS_DROP: interrupt chain, discard event
 * - PS_ERROR: error occurred, abort system
 */
typedef int (*ps_callback_f)(chain_t chain, void *event, self_t *self);

/* ps_publish publishes (ie, dispatches) an event to a chain.
 *
 * The chain is identified by a hook and a event type. The type of the event is
 * given by `event`. Arguments `arg` and `ret` are input and output arguments,
 * respectively. They can be set to NULL. It is thread of the subscribing
 * handler to test for NULL and to cast `arg` and `ret` to correct types based
 * on `event`.
 *
 * Returns one of the PS_ error codes above.
 */
int ps_publish(chain_t chain, void *event, self_t *self);

/* ps_subscribe subscribes a callback in a chain for an event.
 *
 * The call order of `ps_subscribe` determines the relative order in which the
 * callbacks are called with published events.
 *
 * Note: ps_subscribe should only be called during initialization of the system.
 *
 * Returns 0 if success, otherwise non-zero.
 */
int ps_subscribe(hook_id hook, type_id type, ps_callback_f cb);

#endif /* BINGO_PUBSUB_H */
