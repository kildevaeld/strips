#include "helpers.h"
#include <string.h>
#include <strips/io/io.h>
#include <strips/utils.h>

void duk_write_buffer_or_writer(strips_bag_t *bag, char *buffer,
                                size_t in_size) {

  duk_context *ctx = bag->ctx;
  duk_push_ref(bag->ctx, bag->ref);
  if (duk_is_buffer(ctx, -1)) {

    bag->data = duk_resize_buffer(bag->ctx, -1, bag->size + in_size);
    duk_pop(bag->ctx);
    if (bag->data == NULL) {

      printf("not enough memory (realloc returned NULL)\n");
      return;
    }

    memcpy(&(bag->data[bag->size]), buffer, in_size);
    bag->size += in_size;

  } else if (duk_io_is_writerlike(ctx, -1)) {
    duk_get_prop_string(ctx, -1, "write");
    duk_dup(ctx, -2);
    char *buf = duk_push_fixed_buffer(ctx, in_size);
    memcpy(buf, buffer, in_size);
    duk_pcall_method(ctx, 1);
  } else {
    duk_type_error(ctx, "s");
  }
}