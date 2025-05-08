/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <assert.h>
#include <pthread.h>
#include <unistd.h>

#include <bingo/capture.h>
#include <bingo/capture/memaccess.h>
#include <bingo/log.h>
#include <bingo/now.h>
#include <bingo/self.h>
#include <vsync/atomic.h>

vatomic32_t stop;
vatomic32_t start;
vatomic64_t count;

#define EXPECTED 128

bool received = 0;
REGISTER_CALLBACK(CAPTURE_EVENT, EVENT_MA_AWRITE, {
    memaccess_t *ma = (memaccess_t *)event;
    assert(ma->argu64 == EXPECTED);
    received = 1;
})

int
main(void)
{
    memaccess_t ma = {.argu64 = EXPECTED};
    capture_event(EVENT_MA_AWRITE, &ma);

    assert(received);
    return 0;
}
