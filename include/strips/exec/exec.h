#pragma once
#include <duktape.h>

#ifdef __cplusplus
extern "C" {
#endif

void strips_exec_init(duk_context *vm);

#ifdef __cplusplus
}
#endif