/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include "core.h"

const size_t mempool_size = 1024 * 1024 * 200;
static bool _initd;

BINGO_HIDE_IF void
bingo_init()
{
    if (_initd)
        return;
    _initd = true;

    // initialize mempool and self modules
    mempool_init(mempool_size);
    _self_init();

    // now we are ready to intercept events, call _ps_init to enable pubsub
    _ps_init();
}

BINGO_HIDE_IF void
bingo_fini()
{
    if (!_initd)
        return;

    _self_fini();
    mempool_fini();
}

BINGO_MODULE_INIT({ bingo_init(); })
BINGO_MODULE_FINI({ bingo_fini(); })
