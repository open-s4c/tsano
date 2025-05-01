/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <stdlib.h>

#include "core.h"
#include <bingo/capture.h>
#include <bingo/pubsub.h>

void
capture_event(type_id type, void *event)
{
    token_t token = make_token(CAPTURE_EVENT, type);
    int err       = _ps_publish(token, event, 0);
    if ((err & PS_ERROR) == PS_ERROR) {
        exit(EXIT_FAILURE);
    }
}

int
capture_before(type_id type, void *event)
{
    token_t token = make_token(CAPTURE_BEFORE, type);
    int err       = _ps_publish(token, event, 0);
    if ((err & PS_ERROR) == PS_ERROR) {
        exit(EXIT_FAILURE);
    }
    return err == PS_DROP ? PS_DROP : PS_SUCCESS;
}

void
capture_after(type_id type, void *event)
{
    token_t token = make_token(CAPTURE_AFTER, type);
    int err       = _ps_publish(token, event, 0);
    if ((err & PS_ERROR) == PS_ERROR) {
        exit(EXIT_FAILURE);
    }
}
