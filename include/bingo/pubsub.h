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
 * tokens. To publish to a chain, you first have to retrieve a token for the
 * chain. A token might be exlusive, making other publishers to fail in runtime.
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

/* token_t is an object representing the permission to publish to a chain. */
typedef union token {
    const struct {
        hook_id hook;
        type_id type;
        uint32_t index;
    } details;
    uint64_t opaque;
} token_t;

/* Initializes a token */
static inline token_t
make_token(hook_id hook, type_id type)
{
    return (token_t){.details = {
                         .hook  = hook,
                         .type  = type,
                         .index = 0,
                     }};
}

static inline hook_id
hook_from(token_t token)
{
    return token.details.hook;
}

static inline type_id
type_from(token_t token)
{
    return token.details.type;
}

static inline uint32_t
index_from(token_t token)
{
    return token.details.index;
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
typedef int (*ps_callback_f)(token_t token, void *event, self_t *self);

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
int ps_publish(token_t token, void *event, self_t *self);

/* ps_republish republishes to the suffix of a chain.
 *
 * This function can be used inside handlers to republish an event to the
 * current chain using the token given in the callback. Only the suffix of the
 * chain receives the publication. This can be used to create tree chains or to
 * lookahead, by allowing the suffix of the chain to execute before the current
 * handler performs an action (once the republish macro returns).
 */
int ps_republish(token_t token, void *event, self_t *self);

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
