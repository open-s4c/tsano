add_library(bingo SHARED pubsub.c mempool.c)
target_link_libraries(bingo PRIVATE pthread)
