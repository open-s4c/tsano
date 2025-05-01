/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <stdlib.h>

#include "core.h"
#include <bingo/capture.h>
#include <bingo/pubsub.h>

void
capture_event(event_id event, const void *arg)
{
    token_t token = make_token(CAPTURE_EVENT, event);
    int err       = _ps_publish(token, arg, 0);
    if ((err & PS_ERROR) == PS_ERROR) {
        exit(EXIT_FAILURE);
    }
}

int
capture_before(event_id event, const void *arg)
{
    token_t token = make_token(CAPTURE_BEFORE, event);
    int err       = _ps_publish(token, arg, 0);
    if ((err & PS_ERROR) == PS_ERROR) {
        exit(EXIT_FAILURE);
    }
    return err == PS_DROP ? PS_DROP : PS_SUCCESS;
}

void
capture_after(event_id event, const void *arg)
{
    token_t token = make_token(CAPTURE_AFTER, event);
    int err       = _ps_publish(token, arg, 0);
    if ((err & PS_ERROR) == PS_ERROR) {
        exit(EXIT_FAILURE);
    }
}
