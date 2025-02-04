/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef BINGO_MACROS_H
#define BINGO_MACROS_H

#define BINGO_CTOR __attribute__((constructor))
#define BINGO_DTOR __attribute__((destructor))
#define BINGO_WEAK __attribute__((weak))

#define BINGO_PLUGIN_INIT(CODE)                                                \
    static BINGO_CTOR void _plugin_init()                                      \
    {                                                                          \
        if (1) {                                                               \
            CODE                                                               \
        }                                                                      \
        printf("LOADED %s\n", __FILE__);                                       \
    }

#define BINGO_PLUGIN_FINI(CODE)                                                \
    static BINGO_CTOR void _plugin_finif()                                     \
    {                                                                          \
        if (1) {                                                               \
            CODE                                                               \
        }                                                                      \
    }
#endif /* BINGO_MACROS_H */
