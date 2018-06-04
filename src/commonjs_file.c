#include "commonjs_file.h"
#include "private.h"
#include <dlfcn.h>
#include <strips/definitions.h>
#include <strips/utils.h>
#include <syrup/fs.h>
#include <syrup/path.h>

static bool cs_is_dll(const char *filename) {
  size_t iexts;
  sy_path_ext(filename, &iexts);
  return strcmp(filename + iexts, CS_DLL_EXTENSION) == 0;
}

static bool file_exists(char *buffer, size_t len, const char *ext) {
  size_t elen = strlen(ext);
  strcpy(buffer + len, ext);
  buffer[len + elen] = '\0';
  if (!sy_file_exists(buffer)) {
    char buf[len + elen + 4 + 1];

    size_t bidx, didx;

    int blen = sy_path_base(buffer, &bidx);
    if (blen == 0) {
      return false;
    }

    didx = sy_path_dir(buffer);
    memcpy(buf, buffer, didx);
    memcpy(buf + didx, "/lib", 4);
    memcpy(buf + didx + 4, buffer + bidx, blen);
    buf[len + elen + 3] = '\0';
    if (!sy_file_exists(buf)) {
      return false;
    }
    strcpy(buffer, buf);
  }
  return true;
}

duk_ret_t cjs_resolve_file(duk_context *ctx) {

  duk_get_prop_string(ctx, 0, "protocol");
  duk_get_prop_index(ctx, -1, 2);
  const char *full_file = duk_require_string(ctx, -1);
  duk_pop_2(ctx);

  duk_idx_t array = duk_push_array(ctx);

  size_t ext_idx = -1;
  int len = sy_path_ext(full_file, &ext_idx);

  int al = 0;
  if (len == 0) {
    len = strlen(full_file);
    char buf[len + 15];
    strcpy(buf, full_file);
    if (file_exists(buf, len, CS_DLL_EXTENSION)) {
      duk_push_string(ctx, buf);
      duk_put_prop_index(ctx, array, al++);
      strcpy(buf, full_file);
    }
    if (file_exists(buf, len, ".js")) {
      duk_push_string(ctx, buf);
      duk_put_prop_index(ctx, array, al++);
    }

  } else if (sy_file_exists(full_file)) {
    duk_push_string(ctx, full_file);
    duk_put_prop_index(ctx, array, al++);
  }

  duk_put_prop_string(ctx, 0, "files");

  return 0;
}

static duk_ret_t load_dll(duk_context *ctx, void *udata) {
  duk_idx_t midx = duk_normalize_index(ctx, -3);
  duk_idx_t iidx = duk_normalize_index(ctx, -4);

  const char *file = duk_require_string(ctx, -1);

  void *handle = dlopen(file, RTLD_LAZY);
  if (!handle)
    duk_type_error(ctx, "could not load native");

  size_t i, xi;
  int bret = sy_path_base(file, &i);
  int eret = sy_path_ext(file + i, &xi);

  if (strncmp(file + i, "lib", 3) == 0) {
    bret -= 3;
    i += 3;
  }

  int name_ln = bret - eret + 8;
  char name[name_ln + 1];
  strcpy(name, "dukopen_");
  strncpy(name + 8, file + i, xi);
  name[name_ln] = '\0';

  dlerror();
  strips_module_initializer module_init =
      (strips_module_initializer)dlsym(handle, name);
  const char *dlsym_error = dlerror();
  if (dlsym_error) {
    dlclose(handle);
    duk_type_error(ctx, "cannot load module '%s'@%s: %s", file, name,
                   dlsym_error);
  }

  duk_push_c_function(ctx, module_init, 0);
  duk_ret_t ret = duk_pcall(ctx, 0);
  if (ret != DUK_EXEC_SUCCESS)
    duk_throw(ctx);

  if (!duk_is_object_coercible(ctx, -1)) {
    duk_type_error(ctx, "invalid return type from %s@%s", file, name);
  }

  duk_enum(ctx, -1, DUK_ENUM_OWN_PROPERTIES_ONLY);
  duk_get_prop_string(ctx, midx, "exports");
  // extends exports with return valiue
  while (duk_next(ctx, -2 /*enum_idx*/, 1 /*get_value*/)) {
    duk_put_prop_string(ctx, -3, duk_get_string(ctx, -2));
    duk_pop(ctx);
  }

  duk_pop_2(ctx);

  return 0;
}

duk_ret_t cjs_load_file(duk_context *ctx) {

  // [ info module ]

  duk_get_prop_string(ctx, 0, "files");

  if (duk_is_undefined(ctx, -1)) {
    duk_type_error(ctx, "invalid paramters");
  }

  int len = duk_get_length(ctx, -1);
  size_t iexts;
  duk_ret_t ret = DUK_EXEC_SUCCESS;
  duk_size_t aidx = duk_normalize_index(ctx, -1);
  for (int i = 0; i < len; i++) {

    duk_get_prop_index(ctx, -1, i);
    const char *n = duk_require_string(ctx, -1);

    if (!sy_path_ext(n, &iexts)) {
      duk_type_error(ctx, "file '%s' has no extension", n);
    }

    // DLL
    if (strcmp(n + iexts, CS_DLL_EXTENSION) == 0) {
      ret = duk_safe_call(ctx, load_dll, NULL, 0, 0);
      if (ret != DUK_EXEC_SUCCESS) {
        duk_throw(ctx);
      }
      duk_push_object(ctx);
      duk_dup(ctx, -2);
      duk_put_prop_string(ctx, -2, "content");
      duk_push_string(ctx, n);
      duk_put_prop_string(ctx, -2, "file");

      duk_put_prop_index(ctx, aidx, i);
      duk_pop(ctx);

    } else {

      const char *file = duk_require_string(ctx, -1);
      int size = sy_file_size(file);
      if (size == 0) {
        duk_push_string(ctx, "");
        break;
      }
      char *buffer = duk_push_fixed_buffer(ctx, size);
      if (!sy_read_file(file, buffer, size, &size)) {
        duk_type_error(ctx, "could not read %s", file);
      }

      duk_push_object(ctx);
      duk_dup(ctx, -2);
      duk_put_prop_string(ctx, -2, "content");
      duk_push_string(ctx, n);
      duk_put_prop_string(ctx, -2, "file");

      duk_put_prop_index(ctx, aidx, i);
      duk_pop(ctx);
    }

    if (ret != DUK_EXEC_SUCCESS) {
      duk_throw(ctx);
    }

    duk_pop(ctx);
  }

  return 0;
}
