/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
/*******************************************************************************
 * @file stacktrace.c
 * @brief Publishes func_entry and func_exit events from TSAN instrumentation.
 *
 ******************************************************************************/
#include <stdint.h>

#include <bingo/intercept.h>
#include <bingo/module.h>
#include <bingo/stacktrace.h>

BINGO_MODULE_INIT()

void
__tsan_func_entry(void *pc)
{
    intercept_at(EVENT_STACKTRACE_ENTER, 0, 0);
}
void
__tsan_func_exit(void)
{
    intercept_at(EVENT_STACKTRACE_EXIT, 0, 0);
}

