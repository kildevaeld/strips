

set(SOURCES
    ${PROJECT_SOURCE_DIR}/vendor/duktape-2.2.1/src/duktape.c
    ${PROJECT_SOURCE_DIR}/vendor/which/which.c
    strips.c
    private.c
    utils.c
    commonjs.c
    commonjs_module.c
    commonjs_file.c
    modules.c
    console.c
    types.c

    path/path.c
    exec/exec.c

)

add_library(strips ${SOURCES})
target_link_libraries(strips csystem)