#include <dirent.h>
#include <duktape.h>
#include <errno.h>
#include <limits.h>
#include <sys/stat.h>
#include <syrup/fs.h>

duk_ret_t duk_io_readdir(duk_context *ctx) {
  const char *path = duk_require_string(ctx, 0);
  duk_require_function(ctx, 1);
  struct dirent *de; // Pointer for directory entry

  // opendir() returns a pointer of DIR type.
  DIR *dr = opendir(path);

  if (dr == NULL) // opendir returns NULL if couldn't open directory
  {
    duk_type_error(ctx, "Could not open current directory: %s", path);
  }

  // Refer http://pubs.opengroup.org/onlinepubs/7990989775/xsh/readdir.html
  // for readdir()
  while ((de = readdir(dr)) != NULL) {

    if (strcmp("..", de->d_name) == 0 || strcmp(".", de->d_name) == 0)
      continue;

    duk_dup(ctx, 1);
    duk_push_string(ctx, de->d_name);
    duk_ret_t ret = duk_pcall(ctx, 1);
    if (ret != DUK_EXEC_SUCCESS) {
      closedir(dr);
      duk_throw(ctx);
    }
    duk_pop(ctx);
  }
  closedir(dr);
  return 0;
}


duk_ret_t duk_io_mkdir(duk_context *ctx) {
  const char *path = duk_require_string(ctx, 0);
  int mod = duk_get_int_default(ctx, 1, 0777);

  int magic = duk_get_current_magic(ctx);

  int ret;
  if (magic) {
    ret = mkdir(path, mod);
  } else {
    ret = sy_mkdir_p(path, mod);
  }

  if (ret != 0) {
    duk_type_error(ctx, "%s", strerror(errno));
  }
  return 0;
}

