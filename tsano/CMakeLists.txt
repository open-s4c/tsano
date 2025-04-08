add_custom_target(
  tsano.c COMMAND env tmplr ${CMAKE_CURRENT_SOURCE_DIR}/tsano.c.in >
                  ${CMAKE_CURRENT_SOURCE_DIR}/tsano.c)

add_library(tsano SHARED tsano.c)

install(TARGETS tsano DESTINATION lib/bingo)
