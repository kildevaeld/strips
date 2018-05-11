#include <dirent.h>
#include <strips/filesystem/filesystem.h>
#include <strips/modules.h>

static duk_ret_t strips_fs_readdir(duk_context *ctx) {
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
    duk_pcall(ctx, 1);
    duk_pop(ctx);
  }
  closedir(dr);
  return 0;
}

static duk_ret_t strips_fs_module(duk_context *ctx) {
  duk_push_object(ctx);

  duk_push_c_function(ctx, strips_fs_readdir, 2);
  duk_put_prop_string(ctx, -2, "readdir");

  return 1;
}

void strips_filesystem_init(duk_context *ctx) {
  duk_module_add_fn(ctx, "filesystem", strips_fs_module);
}
