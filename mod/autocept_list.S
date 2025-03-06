/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

.section .note.GNU-stack,"",@progbits

#if defined(__x86_64__)
#include "autocept_x86_64.S"
#elif defined(__aarch64__)
#include "autocept_aarch64.S"
#else
#error "not supported"
#endif

#include "autocept_generic.S"

// general libc functions
add_interceptor rand
add_interceptor time
add_interceptor sleep
add_interceptor puts
add_interceptor atexit
add_interceptor printf
add_interceptor exit
add_interceptor abort
add_interceptor malloc
add_interceptor free
add_interceptor fwrite
add_interceptor fread

add_interceptor __cxa_guard_abort
add_interceptor __cxa_guard_acquire
add_interceptor __cxa_guard_release
add_interceptor __cxa_atexit

