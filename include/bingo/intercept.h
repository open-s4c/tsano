/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef BINGO_INTERCEPT_H
#define BINGO_INTERCEPT_H

#include <bingo/pubsub.h>

/* Dispatch event before an action in the user code is executed.
 *
 * This can be used before a syscall is called, or before a cmpxchg operation is
 * performed, etc. If you call intercept_before, you are expected to call
 * intercept_after subsequently. The event type is given by the `kind` argument.
 * If `content` is not NULL, the receivers are expected to cast it to the right
 * type using the event's kind. The mailbox `mbox` cat bet set to allow
 * receivers to return values.
 */
void intercept_before(kind_t kind, const void *content, void *mbox);

/* Dispatch event after an action in the user code is executed.
 *
 * This can be used after a syscall is called or after a cmpxchg operation is
 * performed, etc. If you call intercept_after, you must have called
 * intercept_before. The event type is given by the `kind` argument. If
 * `content` is not NULL, the receivers are expected to cast it to the right
 * type using the event's kind. The mailbox `mbox` cat bet set to allow
 * receivers to return values.
 */
void intercept_after(kind_t kind, const void *content, void *mbox);

/* Dispatch event at a given point during the execution.
 *
 * This can be used to implement annotations, inform handlers about the
 * execution, or replace system functions. The event type is given by the `kind`
 * argument. If `content` is not NULL, the receivers are expected to cast it to
 * the right type using the event's kind. The mailbox `mbox` cat bet set to
 * allow receivers to return values.
 */
void intercept_at(kind_t kind, const void *content, void *mbox);

#endif /* BINGO_INTERCEPT_H */
