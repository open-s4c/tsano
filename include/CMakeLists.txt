add_library(bingo.h INTERFACE)
target_include_directories(
	bingo.h INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
                    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
install(DIRECTORY bingo DESTINATION include)
