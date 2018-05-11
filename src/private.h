#pragma once
#include <duktape.h>
#include <stdbool.h>
#include <strips/definitions.h>

bool strips_push_entry(duk_context *ctx, const char *name);

bool strips_get_entry(duk_context *ctx, const char *name);

bool strips_has_entry(duk_context *ctx, const char *name);

duk_ret_t strips_push_module(duk_context *ctx);