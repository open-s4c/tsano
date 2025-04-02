/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
/*******************************************************************************
 * @file self.c
 * @brief Thread "self-awareness"
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
 ******************************************************************************/
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>

#include <bingo/intercept.h>
#include <bingo/log.h>
#include <bingo/mempool.h>
#include <bingo/pubsub.h>
#include <bingo/self.h>
#include <vsync/atomic.h>
#include <vsync/map/rbtree.h>

typedef struct thread_data {
    int guard;
    thread_id tid;
    vrbtree_t tls;
} tdata_t;

struct tls_item {
    vrbtree_key_t key;
    vrbtree_node_t embed;
    char data[];
};

static vatomic64_t _thread_count = VATOMIC_INIT(0);
static pthread_key_t _key;

/* id and self implementation ----------------------------------------------- */
thread_id
self_id()
{
    tdata_t *td = pthread_getspecific(_key);
    return td ? td->tid : NO_THREAD;
}

static int
_tls_cmp(vrbtree_node_t *node, vrbtree_key_t key)
{
    struct tls_item *item = V_CONTAINER_OF(node, struct tls_item, embed);
    if (item->key < key)
        return -1;
    else if (item->key > key)
        return 1;
    else
        return 0;
}

static void
_tls_retire(vrbtree_node_t *node, void *args)
{
    V_UNUSED(node, args);
}

static void
_tls_init(tdata_t *td)
{
    vrbtree_init(&td->tls, _tls_retire, NULL, _tls_cmp);
}

static void
_tls_fini(tdata_t *td)
{
    // TODO: iterate over all items and mempool_free them
    (void)td;
}

void *
self_tls(const void *global, size_t size)
{
    vrbtree_key_t item_key = (vrbtree_key_t)global;
    tdata_t *td            = pthread_getspecific(_key);

    /* should never be called before the self initialization */
    assert(td != NULL);

    vrbtree_node_t *item = vrbtree_get(&td->tls, item_key);
    if (item == NULL) {
        struct tls_item *i = mempool_alloc(size + sizeof(struct tls_item));
        if (i == NULL)
            return NULL;
        memset(&i->data, 0, size);
        i->key  = item_key;
        item    = &i->embed;
        bool ok = vrbtree_add(&td->tls, item_key, item);
        assert(ok);
    }

    struct tls_item *i = V_CONTAINER_OF(item, struct tls_item, embed);
    return i->data;
}

/* pthread constructor and destructor --------------------------------------- */
static void
_self_destruct(void *arg)
{
    /* In some systems (eg, NetBSD) threads still call interceptors while
     * terminating (inside pthread_exit). Setting _key to NULL instructs the
     * pubsub to ignore further events. No further TLS data will be (re)created
     * because no EVENT_THREAD_INIT will be published for this thread anymore.
     */
    if (arg == NULL)
        return;

    _tls_fini(arg);
    mempool_free(arg);
    if (pthread_setspecific(_key, 0) != 0)
        abort();
}

static tdata_t *
_self_construct(event_t event)
{
    tdata_t *td = pthread_getspecific(_key);
    if (td != NULL || event != EVENT_THREAD_INIT)
        return td;

    /* We only initialize the TLS if the event is a THREAD_INIT event. */
    td        = mempool_alloc(sizeof(tdata_t));
    td->guard = 0;
    td->tid   = vatomic64_inc_get(&_thread_count);
    _tls_init(td);

    if (pthread_setspecific(_key, td) != 0)
        abort();

    return td;
}

/* pubsub handler ----------------------------------------------------------- */
static void
_self_handle(token_t token, event_t event, const void *arg, void *ret)
{
    /* The goal here is to wrap the existing event with a seq_value_t that
     * contains also the thread id calculated here when allocating the _key in
     * the TLS. */
    tdata_t *td = _self_construct(event);
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
PS_SUBSCRIBE(ANY_CHAIN, {
    _self_handle(token, event, arg, ret);
    return false;
})

BINGO_MODULE_INIT({
    if (pthread_key_create(&_key, _self_destruct) != 0)
        abort();

    /* Construct TLS for main thread, but do not publish any event. Downstream
     * handlers should learn the existence of the main thread on demand. */
    _self_construct(EVENT_THREAD_INIT);
})

BINGO_MODULE_FINI({ intercept_at(EVENT_THREAD_FINI, 0, 0); })
