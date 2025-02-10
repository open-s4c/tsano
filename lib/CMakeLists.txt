add_library(bingo SHARED pubsub.c mempool.c intercept.c uset.c)
target_link_libraries(bingo PRIVATE pthread)
