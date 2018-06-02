#pragma once
#include <duktape.h>
#include <strips/definitions.h>

void duk_write_buffer_or_writer(strips_bag_t *bag, char *buf, size_t size);