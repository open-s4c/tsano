/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
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
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>

#include "rbtree.h"
#include <bingo/intercept.h>
#include <bingo/log.h>
#include <bingo/mempool.h>
#include <bingo/pubsub.h>
#include <bingo/self.h>
#include <vsync/atomic.h>

// #define USE_THRMAP
//  #define USE_IDMAP

typedef struct thread_data {
    int guard;
    thread_id tid;
    struct rbtree tree;
} thrdata_t;

struct tls_item {
    uintptr_t key;
    struct rbnode node;
    char data[];
};

static vatomic64_t _thread_count = VATOMIC_INIT(0);
static thrdata_t *_self_construct(event_t event);

static void _self_destruct(void *);
static void _tls_init(thrdata_t *td);

// -----------------------------------------------------------------------------
// tls items
// -----------------------------------------------------------------------------
int
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

    /* initialize thread data */
    memset(&i->data, 0, size);
    i->key        = item_key;
    thrdata_t *td = (thrdata_t *)i->data;
    td->guard     = 0;
    td->tid       = vatomic64_inc_get(&_thread_count);
    _tls_init(td);

    /* add to tree */
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
thread_id
self_id()
{
    thrdata_t *td = _thrdata_get();
    return td ? td->tid : NO_THREAD;
}

void *
self_tls(const void *global, size_t size)
{
    uintptr_t item_key = (uintptr_t)global;
    thrdata_t *td      = _thrdata_get();

    /* should never be called before the self initialization */
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
// pthread constructor and destructor
// -----------------------------------------------------------------------------
static void
_self_destruct(void *arg)
{
    /* In some systems (eg, NetBSD) threads still call interceptors while
     * terminating (inside pthread_exit). Setting _key to NULL instructs the
     * pubsub to ignore further events. No further TLS data will be
     * (re)created because no EVENT_THREAD_INIT will be published for this
     * thread anymore.
     */
    if (arg == NULL)
        return;

    /* TODO: change order of atexit destructors */

    _thrdata_del(arg);
    _tls_fini(arg);
    mempool_free(arg);
}

static thrdata_t *
_self_construct(event_t event)
{
    thrdata_t *td = _thrdata_get();
    if (td != NULL || event != EVENT_THREAD_INIT)
        return td;

    /* We only initialize the TLS if the event is a THREAD_INIT event. */
    return _thrdata_new();
}

// -----------------------------------------------------------------------------
// pubsub handler
// -----------------------------------------------------------------------------
static void
_self_handle(token_t token, event_t event, const void *arg, void *ret)
{
    /* The goal here is to wrap the existing event with a seq_value_t that
     * contains also the thread id calculated here when allocating the _key
     * in the TLS. */
    thrdata_t *td = _self_construct(event);
    if (td == NULL)
        return;

    chain_id chain = chain_from(token);

    if ((chain == INTERCEPT_BEFORE && td->guard++ == 0) ||
        (chain == INTERCEPT_AFTER && td->guard-- == 1) ||
        (chain == INTERCEPT_AT && td->guard == 0)) {
        td->guard++;
        PS_REPUBLISH(event, arg, ret);
        td->guard--;
    }

    /* Destruct thread data */
    if (event == EVENT_THREAD_FINI)
        _self_destruct(td);
}

/* Filter all events guarding from reentries */
#undef BINGO_CTOR
#define BINGO_CTOR __attribute__((constructor(256)))
PS_SUBSCRIBE(ANY_CHAIN, {
    _self_handle(token, event, arg, ret);
    return false;
})

BINGO_MODULE_INIT({
    _thrmap_init();
    /* construct tls for main thread, but do not publish any event.
     * Downstream handlers should learn the existence of the main thread on
     * demand. */
    _self_construct(EVENT_THREAD_INIT);
})

BINGO_MODULE_FINI({
    _thrmap_fini();
    intercept_at(EVENT_THREAD_FINI, 0, 0);
})
