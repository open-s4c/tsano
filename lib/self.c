/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 * -----------------------------------------------------------------------------
 * Thread "self-awareness"
 *
 * This module implements TLS management with the memory from mempool.h and
 * guards further modules from receiving events before threads are initialized
 * or after they are finished.
 *
 * The module subscribes to all chains of the pubsub and should be initialized
 * as first module so that it is placed at the start of each chain. The module
 * blocks any events of a thread until the thread publishes a THREAD_INIT event.
 * It also blocks any event of a thread after it sends a THREAD_FINI event.
 *
 * This should be the first module subscribing to all events from pubsub.
 */
#include "core.h"
#include "rbtree.h"

#include <assert.h>
#include <bingo/capture.h>
#include <bingo/log.h>
#include <bingo/mempool.h>
#include <bingo/pubsub.h>
#include <bingo/self.h>
#include <bingo/thread_id.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <vsync/atomic.h>

// #define USE_THRMAP
// #define USE_IDMAP

typedef struct thread_data {
    metadata_t md;
    int guard;
    thread_id tid;
    int count;
    struct rbtree tree;
} thrdata_t;

struct tls_item {
    uintptr_t key;
    struct rbnode node;
    char data[];
};

static vatomic64_t _thread_count = VATOMIC_INIT(0);

static void _self_destruct(void *);
static void _tls_init(thrdata_t *td);

// -----------------------------------------------------------------------------
// tls items
// -----------------------------------------------------------------------------
static int
_tls_cmp(const struct rbnode *a, const struct rbnode *b)
{
    const struct tls_item *ea = container_of(a, struct tls_item, node);
    const struct tls_item *eb = container_of(b, struct tls_item, node);
    return (ea->key > eb->key);
}

static void
_tls_init(thrdata_t *td)
{
    rbtree_init(&td->tree, _tls_cmp);
}

static void
_tls_fini(thrdata_t *td)
{
    // TODO: iterate over all items and mempool_free them
    (void)td;
}

// -----------------------------------------------------------------------------
// thrmap and thrdata
// -----------------------------------------------------------------------------
#ifdef USE_IDMAP
    #include "idmap.h"

    #include <vsync/spinlock/seqlock.h>
    #define MAX_THREADS 4096

static idmap_t _thrmap;
static thrdata_t *_thrdata;
static seqlock_t _seqlock;

static void
_thrmap_init(void)
{
    _thrdata = mempool_alloc(sizeof(thrdata_t) * MAX_THREADS);
    if (!_thrdata)
        log_fatalf("could not allocate thrdata array");
    idmap_init_cap(&_thrmap, MAX_THREADS);
    seqlock_init(&_seqlock);
}
static void
_thrmap_fini(void)
{
    idmap_fini(&_thrmap);
    mempool_free(_thrdata);
}

static uint64_t
_thrid()
{
    return (uint64_t)pthread_self();
}

static thrdata_t *
_thrdata_get(void)
{
    uint64_t id = (uint64_t)_thrid();
    seqvalue_t v;
    size_t idx;
    do {
        v   = seqlock_rbegin(&_seqlock);
        idx = idmap_find(&_thrmap, id);
    } while (!seqlock_rend(&_seqlock, v));

    return idx == idmap_size(&_thrmap) ? NULL : &_thrdata[idx];
}

static thrdata_t *
_thrdata_new(void)
{
    uint64_t id = (uint64_t)_thrid();
    seqlock_acquire(&_seqlock);
    size_t idx = idmap_insert(&_thrmap, id);
    seqlock_release(&_seqlock);

    if (idx == idmap_size(&_thrmap))
        log_fatalf("could not insert thread");

    thrdata_t *td = &_thrdata[idx];
    memset(td, 0, sizeof(thrdata_t));
    td->guard = 0;
    td->tid   = vatomic64_inc_get(&_thread_count);
    _tls_init(td);

    return td;
}

static void
_thrdata_del(thrdata_t *td)
{
    // TODO: remove itself from tree
}

#elif defined(USE_THRMAP)
    #include <vsync/spinlock/rwlock.h>
static struct rbtree _thrmap;
static rwlock_t _rwlock;

static void
_thrmap_init(void)
{
    rbtree_init(&_thrmap, _tls_cmp);
    rwlock_init(&_rwlock);
}
static void
_thrmap_fini(void)
{
}

static uint64_t
_thrid()
{
    return (uint64_t)pthread_self();
}

static thrdata_t *
_thrdata_get(void)
{
    uintptr_t item_key = (uintptr_t)_thrid();

    thrdata_t *td = NULL;
    rwlock_read_acquire(&_rwlock);
    struct tls_item key = {.key = item_key};
    struct rbnode *item = rbtree_find(&_thrmap, &key.node);

    if (item)
        td = (thrdata_t *)container_of(item, struct tls_item, node)->data;
    rwlock_read_release(&_rwlock);
    return td;
}

static thrdata_t *
_thrdata_new(void)
{
    uintptr_t item_key = (uintptr_t)_thrid();
    const size_t size  = sizeof(thrdata_t) + sizeof(struct tls_item);
    struct tls_item *i = mempool_alloc(size);
    if (i == NULL)
        log_fatalf("could not allocate thread data");

    // initialize thread data
    memset(&i->data, 0, size);
    i->key        = item_key;
    thrdata_t *td = (thrdata_t *)i->data;
    td->guard     = 0;
    td->tid       = vatomic64_inc_get(&_thread_count);
    _tls_init(td);

    // add to tree
    rwlock_write_acquire(&_rwlock);
    rbtree_insert(&td->tree, &i->node);
    rwlock_write_release(&_rwlock);
    return td;
}

static void
_thrdata_del(thrdata_t *td)
{
    // TODO: remove itself from tree
}

// -----------------------------------------------------------------------------
// thrmap and thrdata using pthread_get/setspecific
// -----------------------------------------------------------------------------
#else
static pthread_key_t _key;

static void
_thrmap_init(void)
{
    if (pthread_key_create(&_key, _self_destruct) != 0)
        abort();
}
static void
_thrmap_fini(void)
{
}

static thrdata_t *
_thrdata_get(void)
{
    return (thrdata_t *)pthread_getspecific(_key);
}
static thrdata_t *
_thrdata_new()
{
    thrdata_t *td = mempool_alloc(sizeof(thrdata_t));
    td->guard     = 0;
    td->count     = 0;
    td->tid       = vatomic64_inc_get(&_thread_count);
    _tls_init(td);

    if (pthread_setspecific(_key, td) != 0)
        log_fatalf("could not set key");
    return td;
}

static void
_thrdata_del(thrdata_t *td)
{
    if (pthread_setspecific(_key, 0) != 0)
        abort();
}
#endif

// -----------------------------------------------------------------------------
// public interface
// -----------------------------------------------------------------------------
BINGO_HIDE thread_id
self_id(metadata_t *md)
{
    thrdata_t *td = (thrdata_t *)md;
    return td ? td->tid : NO_THREAD;
}

BINGO_HIDE void *
self_tls(metadata_t *md, const void *global, size_t size)
{
    uintptr_t item_key = (uintptr_t)global;
    thrdata_t *td      = (thrdata_t *)md;

    // should never be called before the self initialization
    assert(td != NULL);

    struct tls_item key = {.key = item_key};
    struct rbnode *item = rbtree_find(&td->tree, &key.node);

    if (item) {
        struct tls_item *i = container_of(item, struct tls_item, node);
        return i->data;
    }

    struct tls_item *i = mempool_alloc(size + sizeof(struct tls_item));
    if (i == NULL)
        return NULL;
    memset(&i->data, 0, size);
    i->key = item_key;
    rbtree_insert(&td->tree, &i->node);
    return i->data;
}

// -----------------------------------------------------------------------------
// self destructor
// -----------------------------------------------------------------------------

static void
_self_destruct(void *arg)
{
    // In some systems (eg, NetBSD) threads still call interceptors while
    // terminating (inside pthread_exit). Setting _key to NULL instructs the
    // pubsub to ignore further events. No further TLS data will be
    // (re)created because no EVENT_THREAD_INIT will be published for this
    // thread anymore.
    if (arg == NULL)
        return;

    // TODO: change order of atexit destructors

    _thrdata_del(arg);
    _tls_fini(arg);
    mempool_free(arg);
}

// -----------------------------------------------------------------------------
// pubsub handler
// -----------------------------------------------------------------------------
static void
_self_guard(chain_t chain, void *event, metadata_t *md)
{
    thrdata_t *td = (thrdata_t *)md;
    td->guard++;
    td->count++;
    int err = _ps_publish(chain, event, md);
    td->guard--;
    if (unlikely(err == PS_ERROR))
        abort();
}

static int
_self_handle_before(chain_t chain, void *event, metadata_t *md)
{
    thrdata_t *td = _thrdata_get();
    if (unlikely(td == NULL))
        return PS_DROP;

    td->md = *md;
    if (td->guard++ == 0)
        _self_guard(as_chain(CAPTURE_BEFORE, chain.type), event, &td->md);

    return PS_STOP;
}

static int
_self_handle_after(chain_t chain, void *event, metadata_t *md)
{
    thrdata_t *td = _thrdata_get();
    if (unlikely(td == NULL))
        return PS_DROP;

    td->md = *md;
    if (td->guard-- == 1)
        _self_guard(as_chain(CAPTURE_AFTER, chain.type), event, &td->md);

    return PS_STOP;
}

static int
_self_handle_event(chain_t chain, void *event, metadata_t *md)
{
    thrdata_t *td = _thrdata_get();
    if (td == NULL)
        if (chain.type == EVENT_THREAD_INIT)
            // Only initialize TLS if the event is a THREAD_INIT event.
            td = _thrdata_new();

    if (unlikely(td == NULL))
        return PS_DROP;

    td->md = *md;
    if (unlikely(td->tid == MAIN_THREAD && td->count == 0)) {
        // inform remainder of chain that main thread started
        if (_ps_publish(as_chain(CAPTURE_EVENT, EVENT_THREAD_INIT), 0,
                        &td->md) != PS_SUCCESS)
            abort();
        td->md.hook = md->hook;
    }

    if (td->guard == 0)
        _self_guard(as_chain(CAPTURE_EVENT, chain.type), event, &td->md);

    // Destruct thread data
    if (unlikely(chain.type == EVENT_THREAD_FINI))
        _self_destruct(td);

    return PS_STOP;
}

BINGO_HIDE int
_self_handle(chain_t chain, void *event, metadata_t *md)
{
    switch (chain.hook) {
        case RAW_CAPTURE_BEFORE:
            return _self_handle_before(chain, event, md);
        case RAW_CAPTURE_AFTER:
            return _self_handle_after(chain, event, md);
        case RAW_CAPTURE_EVENT:
            return _self_handle_event(chain, event, md);
    }
    return PS_INVALID;
}

// -----------------------------------------------------------------------------
// init, fini and registration
// -----------------------------------------------------------------------------

BINGO_HIDE void
_self_init()
{
    // First, prepare the map for TLS
    _thrmap_init();

    // Construct TLS for main thread, but do not publish any event yet. On the
    // first event of the main thread (handled by self's callback), we piggyback
    // a EVENT_THREAD_INIT for the main thread.
    (void)_thrdata_new();

    // Subscribe callbacks and abort on failure
    if (0 != ps_subscribe(RAW_CAPTURE_BEFORE, ANY_TYPE, _self_handle_before) ||
        0 != ps_subscribe(RAW_CAPTURE_AFTER, ANY_TYPE, _self_handle_after) ||
        0 != ps_subscribe(RAW_CAPTURE_EVENT, ANY_TYPE, _self_handle_event))
        exit(EXIT_FAILURE);
}

BINGO_HIDE void
_self_fini()
{
    capture_event(EVENT_THREAD_FINI, 0);
    _thrmap_fini();
}
