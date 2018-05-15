#pragma once
#include <duktape.h>

#ifdef __cplusplus
extern "C" {
#endif

void strips_prompt_init(duk_context *ctx);

#ifdef __cplusplus
}
#endif