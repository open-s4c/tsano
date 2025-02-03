/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <bingo/pubsub.h>
#include <assert.h>

typedef union {
    struct {
        uint32_t index;
        topic_t topic;
        bool exclusive;
    } details;
    token_t as_token;
} timpl_t;

#define MAX_SUBSCRIPTIONS 1024

typedef struct {
    topic_t topic;
    ps_callback_f cb;
    int mode;
} subscription_t;

static int _next_subscription;
static subscription_t _subscriptions[MAX_SUBSCRIPTIONS + 1];

topic_t
as_topic(token_t token)
{
    timpl_t timpl;
    timpl.as_token = token;
    return timpl.details.topic;
}

token_t
ps_advertise(topic_t topic, bool exclusive)
{
    timpl_t timpl = {
        .details =
            {
                .index     = 0,
                .topic     = topic,
                .exclusive = exclusive,
            },
    };
    return timpl.as_token;
}

int
ps_subscribe(topic_t topic, ps_callback_f cb)
{
    assert(_next_subscription < MAX_SUBSCRIPTIONS);
    int idx             = _next_subscription++;
    _subscriptions[idx] = (subscription_t){.topic = topic, .cb = cb};
    return 0;
}

int
ps_publish(token_t token, event_t event)
{
    timpl_t timpl;
    timpl.as_token = token;

    for (int idx = 0, sidx = 0; idx < _next_subscription; idx++) {
        subscription_t subs = _subscriptions[idx];
        if (subs.topic != timpl.details.topic)
            continue;
        if (sidx++ < timpl.details.index)
            continue;

        timpl.details.index++;

        assert(subs.cb);
        if (!subs.cb(timpl.as_token, event))
            break;
    }
    return 0;
}
