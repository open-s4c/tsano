/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <bingo/pubsub.h>
#include <assert.h>
#include <stdio.h>

typedef union {
    struct {
        uint32_t index;
        topic_t topic;
        bool exclusive;
    } details;
    token_t as_token;
} timpl_t;

#define MAX_SUBSCRIPTIONS 1024

#define MODE_UNDEF   0
#define MODE_EXCL    1
#define MODE_NONEXCL 2

typedef struct {
    topic_t topic;
    ps_callback_f cb;
    int mode;
} subscription_t;

static unsigned int _next_subscription;
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

    for (unsigned int idx = 0, sidx = 0; idx < _next_subscription; idx++) {
        // if we find a subscription for the topic, we also check if the
        // subscription index (sidx) matches the token index. The token index
        // indicates where in the subscriber chain, the publication should
        // start.
        subscription_t subs = _subscriptions[idx];
        if (subs.topic != timpl.details.topic && subs.topic != TOPIC_ANY)
            continue;
        if (sidx++ < timpl.details.index)
            continue;

        // we pass the up-to-date token in the callback in case the subscriber
        // wants to republish to the remainder of this chain.
        timpl.details.index++;

        assert(subs.cb);

        // to enforce exclusive rights to publish in a topic, we mark the
        // subscriptions with exclusive or non-exclusive modes.
        int token_mode = timpl.details.exclusive ? MODE_EXCL : MODE_NONEXCL;
        if (subs.mode == MODE_UNDEF) {
            subs.mode = token_mode;
        } else if (subs.mode != token_mode) {
            perror("wrong token mode");
            return 1;
        }

        // now we call the callback and abort the chain if the subscriber
        // "censors" the event by returning false.
        if (!subs.cb(timpl.as_token, event))
            return 0;
    }
    return 0;
}
