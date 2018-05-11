#include "../src/private.h"
#include <strips/modules.h>
#include <strips/strips.h>
#include <strips/utils.h>
#include <unity.h>

static duk_ret_t module(duk_context *ctx) {}

void test_modules() {
  duk_context *ctx = duk_create_heap_default();
  strips_initialize(ctx);

  TEST_ASSERT_EQUAL_INT(STRIPS_OK, duk_module_add_fn(ctx, "test", module));
  TEST_ASSERT_EQUAL_INT(STRIPS_DUPLICATE_MODULE,
                        duk_module_add_fn(ctx, "test", module));

  strips_get_entry(ctx, "modules");
  TEST_ASSERT_EQUAL_INT(true, duk_has_prop_string(ctx, -1, "test"));
  // duk_push_global_stash(ctx);
  // duk_dump_context_stdout(ctx);
}