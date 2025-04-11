/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef BINGO_INTERCEPT_H
#define BINGO_INTERCEPT_H
/*******************************************************************************
 * @file intercept.h
 * @brief Standard interception functions.
 *
 * Bingo offers three standard interception chains (see pubsub.h). Events in
 * these chains can only be published via the functions provided in this header.
 *
 ******************************************************************************/

#include <bingo/pubsub.h>

/* Dispatch event before an action in the user code is executed.
 *
 * This can be used before a syscall is called, or before a cmpxchg operation is
 * performed, etc. If you call intercept_before, you are expected to call
 * intercept_after subsequently. The event type is given by the `event`
 * argument. If `content` is not NULL, the receivers are expected to cast it to
 * the right type using the event's event. The mailbox `ret` cat bet set to
 * allow receivers to return values.
 */
void intercept_before(event_id event, const void *content, void *ret);

/* Dispatch event after an action in the user code is executed.
 *
 * This can be used after a syscall is called or after a cmpxchg operation is
 * performed, etc. If you call intercept_after, you must have called
 * intercept_before. The event type is given by the `event` argument. If
 * `content` is not NULL, the receivers are expected to cast it to the right
 * type using the event's event. The mailbox `ret` cat bet set to allow
 * receivers to return values.
 */
void intercept_after(event_id event, const void *content, void *ret);

/* Dispatch event at a given point during the execution.
 *
 * This can be used to implement annotations, inform handlers about the
 * execution, or replace system functions. The event type is given by the
 * `event` argument. If `content` is not NULL, the receivers are expected to
 * cast it to the right type using the event's event. The mailbox `ret` cat bet
 * set to allow receivers to return values.
 */
void intercept_at(event_id event, const void *content, void *ret);

#endif /* BINGO_INTERCEPT_H */
