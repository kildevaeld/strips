#pragma once

#include <duktape.h>
#ifdef __cplusplus
extern "C" {
#endif

void strips_os_init(duk_context *ctx, int argc, char *argv[], char **env);

#ifdef __cplusplus
}
#endif