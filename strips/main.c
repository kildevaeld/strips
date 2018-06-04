#include <strips/modules.h>
#include <strips/strips.h>
//#include <duker/pool.h>
#include <stdio.h>
#include <strips/curl/curl.h>
#include <strips/exec/exec.h>
#include <strips/io/io.h>
#include <strips/os/os.h>
#include <strips/path/path.h>
#include <strips/prompt/prompt.h>

// Run a single file, one time
static int run_single(const char *path, int argc, char **argv) {

  duk_context *ctx = duk_create_heap_default();

  strips_initialize(ctx);

  strips_path_init(ctx);
  strips_prompt_init(ctx);
  strips_io_init(ctx);
  strips_curl_init(ctx);
  strips_exec_init(ctx);
  strips_os_init(ctx, argc, argv, NULL);

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

int main(int argc, char **argv) {

  if (argc == 1) {
    fprintf(stderr, "usage: strips <path>\n");
    return 1;
  }

  return run_single(argv[1], argc, argv);
}