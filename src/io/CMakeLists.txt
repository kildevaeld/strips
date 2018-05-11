
set(SOURCES
   io.c
   reader.c
   writer.c
   file.c
)

add_library(strips-io ${SOURCES})
target_link_libraries(strips-io dukext)