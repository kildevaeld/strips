#include <strips/strips.h>
#include <strips/utils.h>

#include <stdio.h>
#include <unity.h>

extern void test_refs();
extern void test_modules();

void test_initialiasing() {
  duk_context *ctx = duk_create_heap_default();
  TEST_ASSERT_EQUAL_INT(strips_initialize(ctx), STRIPS_OK);
  TEST_ASSERT_EQUAL_INT(strips_initialize(ctx), STRIPS_ALREADY_INITIALIZED);
}

int main() {
  UNITY_BEGIN();

  RUN_TEST(test_initialiasing);
  RUN_TEST(test_refs);
  RUN_TEST(test_modules);

  return UNITY_END();
}