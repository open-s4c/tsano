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
 * Events are identified by a type `event_id` and may contain contain an
 * immutable argument `arg` and a return value `ret`.
 *
 * This pubsub uses the concept of tokens. To publish to a chain, you first have
 * to retrieve a token for the chain. A token might be exlusive, making other
 * publishers to fail in runtime.
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

/* event_id represents the event type send to a chain. */
typedef uint16_t event_id;

/* MAX_EVENTS determines the maximum number of event types. */
#define MAX_EVENTS 512

/* ANY_EVENT indicates any chain. */
#define ANY_EVENT 0

/* chain_id identifies a subscriber group ordered by the subscription time. */
typedef uint16_t chain_id;

/* MAX_CHAINS determines the maximum number of chains */
#define MAX_CHAINS 16

/* ANY_CHAIN indicates any chain. */
#define ANY_CHAIN 0

/* CHAIN_NULL is a special chain that never has subscribers. */
#define CHAIN_NULL 0

typedef struct self self_t;

/* token_t is an object representing the permission to publish to a chain. */
typedef struct token {
    const chain_id chain;
    const event_id event;
    uint32_t index;
} token_t;

/* Initializes a token */
static inline token_t
make_token(chain_id chain, event_id event)
{
    return (token_t){
        .chain = chain,
        .event = event,
        .index = 0,
    };
}

/* Context/self opaque metadata */
typedef struct self self_t;

/* ps_callback_f is the interface of the handlers subscribing to a chain.
 * If the handler returns false, the chain is interrupted, ie, the event is
 * stopped from being propagated in this chain. */
typedef bool (*ps_callback_f)(token_t token, const void *arg, self_t *self);

#define PS_SUCCESS   0
#define PS_NOT_READY 1
#define PS_INVALID   2
#define PS_ERROR     (-(PS_INVALID | 0))

/* ps_publish publishes (ie, dispatches) an event to a chain.
 *
 * The chain is identified by the token retrieved with `ps_advertise`. The type
 * of the event is given by `event`. Arguments `arg` and `ret` are input and
 * output arguments, respectively. They can be set to NULL. It is thread of the
 * subscribing handler to test for NULL and to cast `arg` and `ret` to correct
 * types based on `event`.
 *
 * Returns 0 if success, otherwise non-zero.
 */
int ps_publish(token_t token, const void *arg, self_t *self);

/* ps_republish republishes to the suffix of a chain.
 *
 * This function can be used inside handlers to republish an event to the
 * current chain using the token given in the callback. Only the suffix of the
 * chain receives the publication. This can be used to create tree chains or to
 * lookahead, by allowing the suffix of the chain to execute before the current
 * handler performs an action (once the republish macro returns).
 */
int ps_republish(token_t token, const void *arg, self_t *self);

/* ps_subscribe subscribes a callback in a chain for an event.
 *
 * The call order of `ps_subscribe` determines the relative order in which the
 * callbacks are called with published events.
 *
 * Note: ps_subscribe should only be called during initialization of the system.
 *
 * Returns 0 if success, otherwise non-zero.
 */
int ps_subscribe(chain_id chain, event_id event, ps_callback_f cb);

#endif /* BINGO_PUBSUB_H */
