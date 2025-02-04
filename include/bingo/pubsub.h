/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef BINGO_PUBSUB_H
#define BINGO_PUBSUB_H

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include <bingo/macros.h>

/*
 *  event
 */

typedef union {
    const void *ptr;
    uint64_t u64;
    bool bit;
} value_t;

typedef uint32_t kind_t;
typedef struct event {
    kind_t kind;     // category to know how to dispatch
    value_t content; // abstract argument
    void *mbox;      // abtract return arguments
} event_t;


/*
 *  topics and tokens
 */

#define TOPIC_ANY ((topic_t) - 1)

typedef uint16_t topic_t;
typedef struct token {
    uint64_t _v;
} token_t;

topic_t as_topic(token_t token);


/*
 * pubsub interface
 */

// return true to continue publishing chain
typedef bool (*ps_callback_f)(token_t token, event_t event);

token_t ps_advertise(topic_t topic, bool exclusive);
int ps_publish(token_t token, event_t event);
int ps_subscribe(topic_t topic, ps_callback_f cb);

#define PS_SUBSCRIBE(topic, CALLBACK)                                          \
    static bool _ps_callback_##topic(token_t token, event_t event)             \
    {                                                                          \
        CALLBACK;                                                              \
        return true;                                                           \
    }                                                                          \
    static void BINGO_CTOR _ps_subscribe_##topic(void)                         \
    {                                                                          \
        ps_subscribe(topic, _ps_callback_##topic);                             \
    }

#define PS_PUBLISH(topic, event)                                               \
    do {                                                                       \
        int err = ps_publish(ps_advertise(topic, false), event);               \
        assert(err == 0);                                                      \
    } while (0);

#define PS_REPUBLISH(event)                                                    \
    do {                                                                       \
        int err = ps_publish((token), (event));                                \
        assert(err == 0);                                                      \
    } while (0);

#endif /* BINGO_PUBSUB_H */
