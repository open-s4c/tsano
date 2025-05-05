/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include "core.h"

#include <bingo/capture.h>
#include <bingo/pubsub.h>
#include <stdlib.h>

BINGO_HIDE_IF void
capture_event(type_id type, void *event)
{
    chain_t chain = as_chain(RAW_CAPTURE_EVENT, type);
    metadata_t md = {0};
    int err       = _ps_publish(chain, event, &md);
    if ((err & PS_ERROR) == PS_ERROR) {
        exit(EXIT_FAILURE);
    }
}

BINGO_HIDE_IF int
capture_before(type_id type, void *event)
{
    chain_t chain = as_chain(RAW_CAPTURE_BEFORE, type);
    metadata_t md = {0};
    int err       = _ps_publish(chain, event, &md);
    if ((err & PS_ERROR) == PS_ERROR) {
        exit(EXIT_FAILURE);
    }
    return err == PS_DROP ? PS_DROP : PS_SUCCESS;
}

BINGO_HIDE_IF void
capture_after(type_id type, void *event)
{
    chain_t chain = as_chain(RAW_CAPTURE_AFTER, type);
    metadata_t md = {0};
    int err       = _ps_publish(chain, event, &md);
    if ((err & PS_ERROR) == PS_ERROR) {
        exit(EXIT_FAILURE);
    }
}
