

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
    file-utils.c

    path/path.c
    exec/exec.c
    prompt/prompt.c
    os/os.c
    os/env.c
    os/dir.c
    io/io.c
    io/file.c
    io/reader.c
    io/writer.c

)

add_library(strips ${SOURCES})
target_link_libraries(strips syrup-path syrup-term)