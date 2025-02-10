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
 ******************************************************************************/
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include <bingo/macros.h>

/* kind_t represents the event type send to a chain */
typedef uint32_t kind_t;

/* chain_id identifies a subscriber group ordered by the subscription time. */
typedef uint16_t chain_id;

/* ANY_CHAIN indicates any chain. */
#define ANY_CHAIN ((chain_id) - 1)

/* Bingo's pubsub contains 3 default chains */
enum default_chains {
    INTERCEPT_BEFORE,
    INTERCEPT_AFTER,
    INTERCEPT_AT,

    _CHAIN_ANY = ANY_CHAIN,
};

/* token_t is an object representing permission to publish to a chain. */
typedef struct token {
    uint64_t _v;
} token_t;

/* chain_from extracts the chain_id from a token. */
chain_id chain_from(token_t token);


/* ps_callback_f is the interface of the handlers subscribing to a chain.
 * If the callback returns false, the event is stopped from being propagated in
 * this chain. */
typedef bool (*ps_callback_f)(token_t token, kind_t kind, const void *arg,
                              void *ret);

/* ps_advertise creates a token for a given chain with/without exclusive
 * publishing rights. */
token_t ps_advertise(chain_id chain, bool exclusive);

/* ps_publish dispatches an event to a chain.
 *
 * The chain is identified by the token. The type of the event is given by
 * `kind`. Arguments `arg` and `ret` are input and output arguments,
 * respectively. They can be NULL. It is task of the subscriber to test for NULL
 * and to cast `arg` and `ret` to correct types based on `kind`.
 *
 * Returns 0 if success.
 */
int ps_publish(token_t token, kind_t kind, const void *arg, void *ret);

/* ps_subscribe registers a callback `cb` as a handler in a chain.
 *
 * The call order of `ps_subscribe` determines the relative order in the chain
 * for the callback.
 *
 * Returns 0 if success.
 */

int ps_subscribe(chain_id chain, ps_callback_f cb);

#define PS_SUBSCRIBE(chain, CALLBACK)                                          \
    static bool _ps_callback_##chain(token_t token, kind_t kind,               \
                                     const void *arg, void *ret)               \
    {                                                                          \
        CALLBACK;                                                              \
        return true;                                                           \
    }                                                                          \
    static void BINGO_CTOR _ps_subscribe_##chain(void)                         \
    {                                                                          \
        ps_subscribe(chain, _ps_callback_##chain);                             \
    }

#define PS_PUBLISH(chain, kind, arg, ret)                                      \
    do {                                                                       \
        int err = ps_publish(ps_advertise(chain, false), kind, arg, ret);      \
        assert(err == 0);                                                      \
    } while (0);

#define PS_REPUBLISH(kind, arg, ret)                                           \
    do {                                                                       \
        int err = ps_publish((token), kind, arg, ret);                         \
        assert(err == 0);                                                      \
    } while (0);

#endif /* BINGO_PUBSUB_H */
