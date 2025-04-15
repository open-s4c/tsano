/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef BINGO_COMPILER_H
#define BINGO_COMPILER_H

#ifndef BINGO_XTOR_PRIO
    #define BINGO_XTOR_PRIO
#endif
#define BINGO_CTOR  __attribute__((constructor(BINGO_XTOR_PRIO)))
#define BINGO_DTOR  __attribute__((destructor(BINGO_XTOR_PRIO)))
#define BINGO_WEAK  __attribute__((weak))
#define BINGO_NORET _Noreturn

#ifdef BINGO_LTO
    #define BINGO_HIDE  __attribute__((visibility("hidden")))
#else
    #define BINGO_HIDE
#endif

#endif /* BINGO_COMPILER_H */
