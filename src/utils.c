#include "private.h"
#include <strips/utils.h>

void duk_dump_context_stdout(duk_context *ctx) {
  duk_push_context_dump(ctx);
  printf("%s\n", duk_safe_to_string(ctx, -1));
  duk_pop(ctx);
}

// like luaL_ref, but assumes storage in "refs" property of heap stash
int duk_ref(duk_context *ctx) {
  int ref;
  /*if (duk_is_undefined(ctx, -1)) {
    duk_pop(ctx);
    return 0;
  }*/
  // Get the "refs" array in the heap stash
  if (!strips_get_entry(ctx, "refs")) {
    return 0;
  }

  // ref = refs[0]
  duk_get_prop_index(ctx, -1, 0);
  ref = duk_get_int(ctx, -1);
  duk_pop(ctx);

  // If there was a free slot, remove it from the list
  if (ref != 0) {
    // refs[0] = refs[ref]
    duk_get_prop_index(ctx, -1, ref);
    duk_put_prop_index(ctx, -2, 0);
  }
  // Otherwise use the end of the list
  else {
    // ref = refs.length;
    ref = duk_get_length(ctx, -1);
  }

  // swap the array and the user value in the stack
  duk_insert(ctx, -2);

  // refs[ref] = value
  duk_put_prop_index(ctx, -2, ref);

  // Remove the refs array from the stack.
  duk_pop(ctx);

  return ref;
}

void duk_push_ref(duk_context *ctx, int ref) {
  if (!ref) {
    duk_push_undefined(ctx);
    return;
  }
  // Get the "refs" array in the heap stash
  if (!strips_get_entry(ctx, "refs")) {
    return;
  }

  duk_get_prop_index(ctx, -1, ref);

  duk_remove(ctx, -2);
}

void duk_unref(duk_context *ctx, int ref) {

  if (!ref)
    return;

  // Get the "refs" array in the heap stash
  if (!strips_get_entry(ctx, "refs")) {
    return;
  }

  // Insert a new link in the freelist

  // refs[ref] = refs[0]
  duk_get_prop_index(ctx, -1, 0);
  duk_put_prop_index(ctx, -2, ref);
  // refs[0] = ref
  duk_push_int(ctx, ref);
  duk_put_prop_index(ctx, -2, 0);

  duk_pop(ctx);
}

void duk_commonjs_wrapl(duk_context *ctx, const char *buffer, size_t len) {
  duk_push_string(ctx,
                  "(function(exports,require,module,__filename,__dirname){");

  duk_push_string(ctx, (buffer[0] == '#' && buffer[1] == '!')
                           ? "//"
                           : "");     /* Shebang support. */
  duk_push_lstring(ctx, buffer, len); /* source */
  duk_push_string(
      ctx,
      "\n})"); /* Newline allows module last line to contain a // comment. */
  duk_concat(ctx, 4);
  duk_push_string(ctx, "wrapped_fn"); // filename
  duk_compile(ctx, DUK_COMPILE_EVAL);
  duk_call(ctx, 0);

  // duk_put_prop_string(ctx, -2, "wrapped_fn");

  // duk_pop(ctx);
}

const char *duk_get_main(duk_context *ctx) {
  duk_push_global_stash(ctx);
  duk_get_prop_string(ctx, -1,
                      "\xff"
                      "mainModule");

  duk_get_prop_string(ctx, -1, "filename");
  const char *c = duk_get_string(ctx, -1);

  duk_pop_3(ctx);

  return c;
}

duk_bool_t duk_is_date(duk_context *ctx, duk_idx_t idx) {
  duk_dup(ctx, idx);
  duk_get_global_string(ctx, "Date");

  duk_bool_t ret = duk_instanceof(ctx, -2, -1);
  duk_pop_2(ctx);
  return ret;
}