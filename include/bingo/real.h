/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef BINGO_REAL_H
#define BINGO_REAL_H

// NOLINTBEGIN(clang-diagnostic-unused-variable)

#include <dlfcn.h>
#include <stddef.h>
#include <stdio.h>

#include <bingo/module.h>

BINGO_WEAK void *real_func_impl(const char *fname, const char *lib);
static inline void *
real_func(const char *fname, const char *lib)
{
    if (real_func_impl)
        return real_func_impl(fname, lib);
    return dlsym(RTLD_NEXT, fname);
}

#define REAL_STR(name)          #name
#define REAL_NAME(x)            __real_##x
#define REAL_DECL(T, func, ...) static T (*REAL_NAME(func))(__VA_ARGS__)
#define REAL(func, ...)         REAL_NAME(func)(__VA_ARGS__)

#define REAL_APPLY(A, ...) REAL_##A(__VA_ARGS__)
#define REAL_LIB_INIT(LIB, T, F, ...)                                          \
    REAL_APPLY(DECL, T, F, ##__VA_ARGS__);                                     \
    if (REAL_APPLY(NAME, F) == NULL) {                                         \
        REAL_APPLY(NAME, F) = real_func(REAL_APPLY(STR, F), LIB);              \
    }                                                                          \
    do {                                                                       \
    } while (0)

#define REAL_INIT(T, F, ...) REAL_LIB_INIT(0, T, F, ##__VA_ARGS__)

// NOLINTEND(clang-diagnostic-unused-variable)

#endif /* BINGO_REAL_H */
