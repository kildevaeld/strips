
set(SOURCES
   filesystem.c
)

add_library(strips-fs ${SOURCES})
target_link_libraries(strips-fs strips)