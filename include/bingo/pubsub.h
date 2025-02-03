/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef BINGO_PUBSUB_H
#define BINGO_PUBSUB_H

#include <stdint.h>
#include <stdbool.h>

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

#define TOPIC_NONE 0
#define TOPIC_ALL  -1

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

#define BINGO_CONSTRUCTOR __attribute__((constructor))

#define PS_SUBSCRIBE(topic, CALLBACK)                                          \
    static bool _ps_callback_##topic(token_t token, event_t event)             \
    {                                                                          \
        (CALLBACK);                                                            \
        return true;                                                           \
    }                                                                          \
    static void BINGO_CONSTRUCTOR _ps_subscribe_##topic(void)                  \
    {                                                                          \
        ps_subscribe(topic, _ps_callback_##topic);                             \
    }

#define PS_PUBLISH(topic, event)                                               \
    do {                                                                       \
        int err = ps_publish(ps_advertise(topic, false), event);               \
        assert(err == 0);                                                      \
    } while (0);

#endif
