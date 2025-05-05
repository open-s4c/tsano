# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: MIT

add_executable(rbtree_test rbtree_test.c rbtree.c)
target_link_libraries(rbtree_test PUBLIC bingo.h)
add_test(rbtree_test rbtree_test)

set(SRCS
    # Stateless components
    core.c
    idmap.c
    rbtree.c
    capture.c
    # Stateful components
    mempool.c
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

add_library(bingo-core SHARED ${SRCS})
target_link_libraries(bingo-core PUBLIC bingo.h)
target_link_libraries(bingo-core PRIVATE pthread)
install(TARGETS bingo-core DESTINATION lib)

set(MODS
    # pthread library
    ../mod/pthread_create.c
    ../mod/pthread_mutex.c
    ../mod/pthread_rwlock.c
    ../mod/pthread_cond.c
    # malloc free and friends
    ../mod/malloc.c
    # semaphore
    ../mod/sem.c
    # cxa guards
    ../mod/cxa.cpp
    # tsan and tsan func-entry/exit
    ../mod/tsan.c
    ../mod/stacktrace.c)

add_library(bingo.lto OBJECT ${SRCS} ${MODS})
target_compile_options(bingo.lto PUBLIC -DBINGO_PS_EXTERNAL
                                        -DBINGO_PS_DIRECT_SELF)
target_link_libraries(bingo.lto PUBLIC bingo.h)

add_library(bingo SHARED ${SRCS} ${MODS})
target_link_libraries(bingo PUBLIC bingo.h)
target_link_libraries(bingo PRIVATE pthread)
install(TARGETS bingo DESTINATION lib)

if(${CMAKE_SYSTEM_NAME} STREQUAL "NetBSD")
  target_compile_options(bingo PRIVATE -fPIC -flto -ffat-lto-objects)
  target_compile_options(bingo.lto PUBLIC -fPIC -flto -ffat-lto-objects)
else()
  # this should work for Linux, macOS and OHOS
  set_property(TARGET bingo PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
  set_property(TARGET bingo.lto PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()
