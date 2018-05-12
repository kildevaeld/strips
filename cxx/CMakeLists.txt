
set(SOURCES
    vm.cpp
    converters.cpp
    object.cpp
    array.cpp
    value.cpp
    reference.cpp
)

add_library(strips++ ${SOURCES})
target_link_libraries(strips++ strips)