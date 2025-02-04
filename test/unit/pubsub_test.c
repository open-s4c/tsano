/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <assert.h>
#include <stdio.h>
#include <bingo/pubsub.h>

#define TOPIC         1
#define ANOTHER_TOPIC 101

int count = 0;
PS_SUBSCRIBE(TOPIC, {
    printf("event from TOPIC\n");
    count++;
})

int
main()
{
    event_t e;
    PS_PUBLISH(TOPIC, e);
    PS_PUBLISH(ANOTHER_TOPIC, e);
    assert(count == 1);
    return 0;
}
