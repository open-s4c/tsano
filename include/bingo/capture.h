/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 * -----------------------------------------------------------------------------
 * @file capture.h
 * @brief Standard capture functions.
 *
 * Bingo offers three standard capture chains. Events in these chains should
 * only be published via the functions provided in this header.
 *
 * This header also provide macros to easily register callbacks.
 */
#ifndef BINGO_CAPTURE_H
#define BINGO_CAPTURE_H

#include <stdlib.h>

#include <bingo/compiler.h>
#include <bingo/pubsub.h>

/* Standard capture chains. */
enum capture_chains {
    CAPTURE_EVENT  = 1,
    CAPTURE_BEFORE = 2,
    CAPTURE_AFTER  = 3,
};

/* Publish event at a given point during the execution.
 *
 * This can be used to implement annotations, inform callbacks about the
 * execution, or replace system functions. The event type is given by the
 * `event` argument. If `arg` is not NULL, the receivers are expected to
 * cast it to the right type using the event's event.
 *
 * On error, aborts system.
 */
void capture_event(event_id event, const void *arg);

/* Publish event before an action in the user code is executed.
 *
 * This can be used before a syscall is called, or before a cmpxchg operation is
 * performed, etc. If you call capture_before, you are expected to call
 * capture_after subsequently. The event type is given by the `event`
 * argument. If `arg` is not NULL, the receivers are expected to cast it to
 * the right type using the event's event. The mailbox `ret` cat bet set to
 * allow receivers to return values.
 *
 * Returns PS_SUCCESS on sucess, PS_DROP if event should be dropped. On error,
 * aborts system.
 */
int capture_before(event_id event, const void *arg);

/* Publish event after an action in the user code is executed.
 *
 * This can be used after a syscall is called or after a cmpxchg operation is
 * performed, etc. If you call capture_after, you must have called
 * capture_before. The event type is given by the `event` argument. If
 * `arg` is not NULL, the receivers are expected to cast it to the right
 * type using the event's event. The mailbox `ret` cat bet set to allow
 * receivers to return values.
 *
 * On error, aborts system.
 */
void capture_after(event_id event, const void *arg);

/* REGISTER_CALLBACK macro creates a callback handler and subscribes to a chain.
 *
 * On load time, a constructor function registers the handler to the chain. The
 * order in which modules are loaded must be considered when planning for the
 * relation between handlers. The order is either given by linking order (if
 * compilation units are linked together) or by the order of shared libraries in
 * LD_PRELOAD.
 */
#define REGISTER_CALLBACK(CHAIN, EVENT, CALLBACK)                              \
    static int _bingo_callback_##CHAIN##_##EVENT(                              \
        token_t token, const void *arg, self_t *self)                          \
    {                                                                          \
        CALLBACK;                                                              \
        return PS_SUCCESS;                                                     \
    }                                                                          \
    static void BINGO_CTOR _bingo_subscribe_##CHAIN##_##EVENT(void)            \
    {                                                                          \
        if (ps_subscribe(CHAIN, EVENT, _bingo_callback_##CHAIN##_##EVENT) !=   \
            PS_SUCCESS)                                                        \
            exit(EXIT_FAILURE);                                                \
    }

/* EVENT_PAYLOAD casts the event argument `arg` to type of the given variable.
 *
 * This macro is intended to be used with REGISTER_CALLBACK. The user must know
 * the type of the argument and then the following pattern can be used:
 *
 *     REGISTER_CALLBACK(SOME_CHAIN, SOME_EVENT, {
 *         const some_known_type *ev = EVENT_PAYLOAD(ev);
 *         ...
 *         })
 */
#define EVENT_PAYLOAD(var) (__typeof(var))arg

#define INTERPOSE_PC __builtin_extract_return_addr(__builtin_return_address(0))

#endif /* BINGO_CAPTURE_H */
