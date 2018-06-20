project(duktape)


set(DUK_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/duktape-2.2.1/src)

set(SOURCES
	${DUK_SOURCE_DIR}/duktape.c
)

include_directories(${DUK_SOURCE_DIR})

add_library(duktape ${SOURCES})
target_include_directories(duktape 
	PUBLIC ${DUK_SOURCE_DIR}
)