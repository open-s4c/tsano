/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef BINGO_TOPICS_H
#define BINGO_TOPICS_H

#include <bingo/pubsub.h>

enum default_topics {
    INTERCEPT_BEFORE,
    INTERCEPT_AFTER,
    CAPTURE,

    _TOPIC_ANY = ANY_TOPIC,
};

#endif /* BINGO_TOPICS_H */
