/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#if defined(__APPLE__)
#define SYM(name) _##name
#else
#define SYM(name) name
#endif

#if defined(__APPLE__)
.macro .size lab, other
.endmacro
.macro .type lab, other
.endmacro
.macro .section lab
.endmacro
#endif

/* Callback functions before and after executing an intercepted function. */
.extern SYM(autocept_before)
.extern SYM(autocept_after)

/*
 * @param name symbol name of intercepted function
 */
.macro add_interceptor name
        .section .rodata
.func_\name:
        .string "\name"
        .text
        .align  4
        .global SYM(\name)
        .type   SYM(\name), @function
SYM(\name):
        /* Push callee-save AND caller-save registers onto the stack. */
        prologue
        push_caller_save

        /* BEFORE_CALL: start intercept and find pointer to intercepted
         * function, return with function pointer. */
        arg_func_name \name
        call_func SYM(autocept_before)

        /* CALL: restore caller-save registers and perform real call. */
        pop_caller_save
        call_func_ptr

        /* AFTER_CALL: Save return value and finish book keeping. */
        push_retval
        arg_func_name \name
        call_func SYM(autocept_after)

        /* Restore return value from CALL and callee-save registers. */
        pop_retval
        epilogue
        return

.end_\name:
        .size \name, .- \name
.endm

