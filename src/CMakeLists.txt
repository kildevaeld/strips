

set(SOURCES
    ${PROJECT_SOURCE_DIR}/vendor/which/which.c
    strips.c
    private.c
    utils.c
    helpers.c
    commonjs.c
    commonjs_file.c
    modules.c
    console.c
    types.c
    ## Modules ##
    path/path.c
    exec/exec.c
    prompt/prompt.c
    # OS
    os/os.c
    os/env.c
    os/dir.c
    os/misc.c
    # IO
    io/io.c
    io/file.c
    io/reader.c
    io/writer.c
)

add_library(strips ${SOURCES})
target_link_libraries(strips 
    ${DUKTAPE_LIBRARIES} 
    ${SYRUP_FS_LIBRARIES} 
    ${SYRUP_TERM_LIBRARIES} 
    ${SYRUP_PATH_LIBRARIES}
)

target_include_directories(strips 
    PUBLIC ${PROJECT_SOURCE_DIR}/include 
    PUBLIC ${DUKTAPE_INCLUDE_DIRS} 
    PUBLIC ${SYRUP_FS_INCLUDE_DIRS} 
    PUBLIC ${SYRUP_PATH_INCLUDE_DIRS} 
    PUBLIC ${SYRUP_TERM_INCLUDE_DIRS}
)

if(APPLE)
    target_compile_definitions(strips PRIVATE CS_DLL_EXTENSION=".dylib")
elseif(LINUX)
    target_compile_definitions(strips PRIVATE CS_DLL_EXTENSION=".so")
endif(APPLE)