/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef BINGO_PUBSUB_H
#define BINGO_PUBSUB_H
/*******************************************************************************
 * @file pubsub.h
 * @brief Interruptable chain-based pubsub
 *
 * This component provides a chain-based pubsub. When an event is published to a
 * chain (aka topic), handlers are called back in the order of subscription.
 * Each handler has the option of interruping the chain by returning false.
 *
 * Events are identified by a type `event_t` and may contain contain an
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
 ******************************************************************************/
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include <bingo/macros.h>

/* event_t represents the event type send to a chain. */
typedef uint32_t event_t;

/* chain_id identifies a subscriber group ordered by the subscription time. */
typedef uint16_t chain_id;

/* ANY_CHAIN indicates any chain. */
#define ANY_CHAIN ((chain_id) - 1)

/* The standard chains. */
enum default_chains {
    INTERCEPT_AT     = 0,
    INTERCEPT_BEFORE = 1,
    INTERCEPT_AFTER  = 2,

    _CHAIN_ANY = ANY_CHAIN,
};

/* token_t is an object representing the permission to publish to a chain. */
typedef struct token {
    uint64_t _v;
} token_t;

/* chain_from extracts the chain_id from a token. */
chain_id chain_from(token_t token);


/* ps_callback_f is the interface of the handlers subscribing to a chain.
 * If the handler returns false, the chain is interrupted, ie, the event is
 * stopped from being propagated in this chain. */
typedef bool (*ps_callback_f)(token_t token, event_t event, const void *arg,
                              void *ret);

/* ps_advertise creates a token for a chain.
 *
 * The token can have exclusive publishing rights if `exclusive` is true.
 * Publishing to a chain without the correct token causes `ps_publish` to return
 * an error. */
token_t ps_advertise(chain_id chain, bool exclusive);

/* ps_publish publishes (ie, dispatches) an event to a chain.
 *
 * The chain is identified by the token retrieved with `ps_advertise`. The type
 * of the event is given by `event`. Arguments `arg` and `ret` are input and
 * output arguments, respectively. They can be set to NULL. It is task of the
 * subscribing handler to test for NULL and to cast `arg` and `ret` to correct
 * types based on `event`.
 *
 * Returns 0 if success, otherwise non-zero.
 */
int ps_publish(token_t token, event_t event, const void *arg, void *ret);

/* ps_subscribe subscribes (ie, registers) a handler `cb` in a chain.
 *
 * The call order of `ps_subscribe` determines the relative order in which the
 * handlers are called with dispatched events.
 *
 * Returns 0 if success, otherwise non-zero.
 */

int ps_subscribe(chain_id chain, ps_callback_f cb);

/* PS_SUBSCRIBE macro creates a handler and subscribes to a chain.
 *
 * On load time, a constructor function registers the handler to the chain. The
 * order in which modules are loaded must be considered when planning for the
 * relation between handlers. The order is either given by linking order (if
 * compilation units are linked together) or by the order of shared libraries in
 * LD_PRELOAD.
 */
#define PS_SUBSCRIBE(chain, CALLBACK)                                          \
    static bool _ps_callback_##chain(token_t token, event_t event,             \
                                     const void *arg, void *ret)               \
    {                                                                          \
        CALLBACK;                                                              \
        return true;                                                           \
    }                                                                          \
    static void BINGO_CTOR _ps_subscribe_##chain(void)                         \
    {                                                                          \
        ps_subscribe(chain, _ps_callback_##chain);                             \
    }

/* PS_PUBLISH macro publishes to `chain` with a non-exclusive token. */
#define PS_PUBLISH(chain, event, arg, ret)                                     \
    do {                                                                       \
        int err = ps_publish(ps_advertise(chain, false), event, arg, ret);     \
        assert(err == 0);                                                      \
    } while (0);

/* PS_REPUBLISH macro republishes to the suffix of a chain.
 *
 * This macro can be used inside handlers to republish an event to the current
 * chain using the token given in the callback. Only the suffix of the chain
 * receives the publication. This can be used to create tree chains or to
 * lookahead, by allowing the suffix of the chain to execute before the current
 * handler performs an action (once the republish macro returns).
 */
#define PS_REPUBLISH(event, arg, ret)                                          \
    do {                                                                       \
        int err = ps_publish((token), event, arg, ret);                        \
        assert(err == 0);                                                      \
    } while (0);

#endif /* BINGO_PUBSUB_H */
