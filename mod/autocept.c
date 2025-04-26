/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#if !defined(__APPLE__)
    #include <dlfcn.h>
#endif

#include "autocept.h"
#include "defs.h"
#include <bingo/capture.h>
#include <bingo/module.h>

BINGO_MODULE_INIT()

// TODO: add function cache

void *
autocept_before(const char *name)
{
    autocept_event_t ev = {.fname = name};
    (void)capture_before(EVENT_AUTOCEPT, &ev);
#if defined(__APPLE__)
    return 0;
#else
    return dlsym(RTLD_NEXT, name);
#endif
}

void
autocept_after(void)
{
    capture_after(EVENT_AUTOCEPT, 0);
}
