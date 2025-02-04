/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef BINGO_TOPICS_H
#define BINGO_TOPICS_H

#include <bingo/pubsub.h>

enum default_topics {
    TOPIC_CAPTURE_BEFORE,
    TOPIC_CAPTURE_AFTER,
    TOPIC_CAPTURE_AT,

    _TOPIC_ANY = TOPIC_ANY,
};

#endif /* BINGO_TOPICS_H */
