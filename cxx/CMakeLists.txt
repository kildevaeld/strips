
set(SOURCES
    vm.cpp
    converters.cpp
    object.cpp
    reference.cpp
    module-resolver.cpp
)

add_library(strips++ ${SOURCES})
target_link_libraries(strips++ strips)
