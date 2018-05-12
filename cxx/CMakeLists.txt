
set(SOURCES
    strips.cpp
    converters.cpp
    object.cpp
    value.cpp
)

add_library(strips++ ${SOURCES})
target_link_libraries(strips++ strips)