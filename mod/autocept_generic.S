/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

/* Callback functions before and after executing an intercepted function. */
.extern autocept_before
.extern autocept_after

/*
 * @param name symbol name of intercepted function
 */
.macro add_interceptor name
        .section .rodata
.func_\name:
        .string "\name"

        .section .text
        .align 4
        .globl \name
        .type  \name, @function
\name:
        /* Push callee-save AND caller-save registers onto the stack. */
        prologue
        push_caller_save

        /* BEFORE_CALL: start intercept and find pointer to intercepted
         * function, return with function pointer. */
        arg_func_name \name
        call_func autocept_before

        /* CALL: restore caller-save registers and perform real call. */
        pop_caller_save
        call_func_ptr

        /* AFTER_CALL: Save return value and finish book keeping. */
        push_retval
        arg_func_name \name
        call_func autocept_after

        /* Restore return value from CALL and callee-save registers. */
        pop_retval
        epilogue
        return

.end_\name:
        .size \name, .- \name
.endm

