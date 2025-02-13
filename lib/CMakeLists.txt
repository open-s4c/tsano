add_library(bingo SHARED
# Stateless components
     uset.c

# Stateful components
    mempool.c intercept.c

# pubsub component must be the last core component to be initialized.
# it guarantees no event is published before the previous components are ready.
    pubsub.c

# the first module subscribing to all events from pubsub. It implements TLS
# management with the memory from mempool. It blocks any messages of a thread
# before they publish an TASK_INIT event. It blocks any messages sent by a
# thread after it sends a TASK_FINI event.
    self.c

# switcher is an optional module. It does not influence the execution unless
# it is actively used by some module.
    switcher.c)

target_link_libraries(bingo PRIVATE pthread)
