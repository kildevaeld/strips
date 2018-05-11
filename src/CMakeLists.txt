

set(SOURCES
    ${PROJECT_SOURCE_DIR}/vendor/duktape-2.2.1/src/duktape.c
    strips.c
    private.c
    utils.c
    commonjs.c
    modules.c
)

add_library(strips ${SOURCES})
target_link_libraries(strips csystem)