/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef BINGO_MODULE_H
#define BINGO_MODULE_H

#include <bingo/log.h>

#define BINGO_CTOR  __attribute__((constructor))
#define BINGO_DTOR  __attribute__((destructor))
#define BINGO_WEAK  __attribute__((weak))
#define BINGO_NORET _Noreturn

#define BINGO_MODULE_INIT(CODE)                                                \
    static BINGO_CTOR void _module_init()                                      \
    {                                                                          \
        if (1) {                                                               \
            CODE                                                               \
        }                                                                      \
        log_printf("LOADED %s\n", __FILE__);                                   \
    }

#define BINGO_MODULE_FINI(CODE)                                                \
    static BINGO_DTOR void _module_fini()                                      \
    {                                                                          \
        if (1) {                                                               \
            CODE                                                               \
        }                                                                      \
    }
#endif /* BINGO_MODULE_H */
