
set(SOURCES
    main.c
    refs.c
    modules.c
)

add_executable(strips-test ${SOURCES})
target_link_libraries(strips-test strips unity)
add_test(strips-test strips-test)