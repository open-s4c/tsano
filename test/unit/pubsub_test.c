/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <assert.h>
#include <stdio.h>

#include <bingo/pubsub.h>

#define CHAIN         1
#define ANOTHER_CHAIN 101

int count = 0;
PS_SUBSCRIBE(CHAIN, {
    printf("event from CHAIN\n");
    count++;
})

int
main()
{
    event_t e;
    PS_PUBLISH(CHAIN, e);
    PS_PUBLISH(ANOTHER_CHAIN, e);
    assert(count == 1);
    return 0;
}
