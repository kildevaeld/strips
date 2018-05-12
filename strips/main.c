#include <strips/modules.h>
#include <strips/strips.h>
//#include <duker/pool.h>
#include <stdio.h>
#include <strips/curl/curl.h>
#include <strips/exec/exec.h>
#include <strips/filesystem/filesystem.h>
#include <strips/io/io.h>

// Run a single file, one time
static int run_single(const char *path) {

  // dukext_config_t config;
  // dukext_config_init(&config);
  // config.max_heap = 1024 << 24;
  // config.module_types = DUKEXT_FILE_TYPE;
  // config.modules = DUKEXT_MOD_FILE | DUKEXT_MOD_PROMPT;

  /*dukext_t *vm;
  if (!(vm = dukext_create(config))) {
    printf("could not init duk\n");
    return 1;
  };*/
  duk_context *ctx = duk_create_heap_default();

  strips_initialize(ctx);

  strips_io_init(ctx);
  strips_curl_init(ctx);
  strips_filesystem_init(ctx);
  strips_exec_init(ctx);

  char *err = NULL;
  duk_ret_t ret = strips_eval_path(ctx, path, &err);

  if (ret != DUK_EXEC_SUCCESS) {
    printf("error %s\n", err);
    goto end;
  }

  // sleep(100);
end:
  duk_destroy_heap(ctx);

  return ret;
}

int main(int argc, const char **argv) {

  if (argc == 1) {
    fprintf(stderr, "usage: strips <path>\n");
    return 1;
  }

  return run_single(argv[1]);
}