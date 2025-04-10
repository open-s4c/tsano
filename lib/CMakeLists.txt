add_executable(rbtree_test rbtree_test.c rbtree.c)

set(SRCS
    # Stateless components
    uset.c
    idmap.c
    rbtree.c
    # Stateful components
    mempool.c
    intercept.c
    # pubsub component must be the last core component to be initialized. it
    # guarantees no event is published before the previous components are ready.
    pubsub.c
    # the first module subscribing to all events from pubsub. It implements TLS
    # management with the memory from mempool. It blocks any messages of a
    # thread before they publish an TASK_INIT event. It blocks any messages sent
    # by a thread after it sends a TASK_FINI event.
    self.c
    # switcher is an optional module. It does not influence the execution unless
    # it is actively used by some module.
    switcher.c)

add_library(bingo SHARED ${SRCS})
target_link_libraries(bingo PUBLIC pthread bingo.h)
install(TARGETS bingo DESTINATION lib)

set(MODS
    # pthread library
    ../mod/pthread_create.c
    ../mod/pthread_mutex.c
    ../mod/pthread_cond.c
    # malloc free and friends
    ../mod/malloc.c
    # semaphore
    ../mod/sem.c
    # cxa guards
    ../mod/cxa.cpp
    # tsan and tsan func-entry/exit
    ../mod/tsan.c
    ../mod/stacktrace.c
    # automatic interception
    ../mod/autocept.c)

add_library(bingo.a STATIC ${SRCS} ${MODS})
target_compile_options(bingo.a PRIVATE -fPIC)
target_link_libraries(bingo.a PUBLIC pthread bingo.h)
set_target_properties(bingo.a PROPERTIES SUFFIX "")
install(TARGETS bingo.a DESTINATION lib)
