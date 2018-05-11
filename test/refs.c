#include "../src/private.h"
#include <strips/strips.h>
#include <strips/utils.h>
#include <unity.h>

void test_refs() {
  duk_context *ctx = duk_create_heap_default();
  strips_initialize(ctx);

  duk_push_string(ctx, "Hello, World");
  int ref = duk_ref(ctx);
  strips_get_entry(ctx, "refs");
  TEST_ASSERT_EQUAL_INT(2, duk_get_length(ctx, -1));
  duk_get_prop_index(ctx, -1, 1);
  const char *n = duk_get_string(ctx, -1);
  TEST_ASSERT_EQUAL_STRING("Hello, World", n);
  duk_pop(ctx);
  duk_unref(ctx, ref);
}