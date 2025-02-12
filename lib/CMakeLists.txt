add_library(bingo SHARED
# Stateless components
 uset.c
# Statful components
mempool.c intercept.c

# pubsub component is stateful and must be the last one to be initialized.
pubsub.c)
target_link_libraries(bingo PRIVATE pthread)
