#include <duktape.h>
#include <unistd.h>
#include <limits.h>

duk_ret_t duk_io_getcwd(duk_context *ctx) {
    char *buffer = duk_push_dynamic_buffer(ctx, PATH_MAX);
    char *buf = getcwd(buffer, PATH_MAX);
    if (!buf) {
        duk_type_error(ctx, "could not load cwd");
    }
    duk_buffer_to_string(ctx, -1);
    return 1;
    
}