add_executable(micro micro.c)
target_link_libraries(micro bingo pthread)

add_executable(micro2 micro2.c)
target_link_libraries(micro2 bingo pthread)
