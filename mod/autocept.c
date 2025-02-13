/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include "autocept.h"
#include "defs.h"
#include <bingo/intercept.h>
#include <bingo/module.h>
#include <bingo/real.h>

BINGO_MODULE_INIT()

// TODO: add function cache

void *
autocept_before(const char *name)
{
    autocept_event_t ev = {.fname = name};
    intercept_before(EVENT_AUTOCEPT, &ev, 0);
    return real_func(name, 0);
}

void
autocept_after(void)
{
    intercept_after(EVENT_AUTOCEPT, 0, 0);
}
