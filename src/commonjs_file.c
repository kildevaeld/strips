#include "commonjs_file.h"
#include "file-utils.h"
#include "private.h"
#include <dlfcn.h>
#include <strips/definitions.h>
#include <strips/utils.h>
#include <syrup/path.h>

/*#ifdef CS_PLATFORM_DARWIN
#define CS_LIBRARY_EXT ".dylib"
#elif CS_PLATFORM_POSIX
#define CS_LIBRARY_EXT ".so"
#endif*/

static bool cs_is_dll(const char *filename) {
  size_t iexts;
  sy_path_ext(filename, &iexts);
  return strcmp(filename + iexts, CS_DLL_EXTENSION) == 0;
}

static bool file_exists(char *buffer, size_t len, const char *ext) {
  size_t elen = strlen(ext);
  strcpy(buffer + len, ext);
  buffer[len + elen] = '\0';
  if (!cs_file_exists(buffer)) {
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
    if (!cs_file_exists(buf)) {
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

  } else if (cs_file_exists(full_file)) {
    duk_push_string(ctx, full_file);
    duk_put_prop_index(ctx, array, al++);
  }

  duk_put_prop_string(ctx, 0, "files");

  return 0;
}

static duk_ret_t load_javascript(duk_context *ctx, void *udata) {

  duk_idx_t midx = duk_normalize_index(ctx, -3);
  duk_idx_t iidx = duk_normalize_index(ctx, -4);
  duk_idx_t fidx = duk_normalize_index(ctx, -1);
  const char *file = duk_require_string(ctx, -1);

  int len;
  char *buffer = cs_read_file(file, NULL, 0, &len);
  if (len == 0)
    duk_type_error(ctx, "could not read %s", file);

  int dlen = sy_path_dir(file);

  // Build commonjs wrap
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
  duk_dup(ctx, -2); // filename
  duk_compile(ctx, DUK_COMPILE_EVAL);
  duk_call(ctx, 0);

  /* [ ... module source func ] */

  /* Set name for the wrapper function. */
  duk_push_string(ctx, "name");
  duk_push_string(ctx, "main");
  duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_FORCE);

  (void)duk_get_prop_string(ctx, midx, "exports");  /* exports */
  (void)duk_get_prop_string(ctx, midx, "require");  /* require */
  duk_dup(ctx, midx);                               /* module */
  (void)duk_get_prop_string(ctx, fidx, "filename"); /* __filename */
  duk_push_lstring(ctx, file, dlen);                /* __dirname */
  duk_call(ctx, 5);
  duk_pop(ctx); // ignore return value
  duk_push_true(ctx);
  duk_put_prop_string(ctx, midx, "loaded");

  return 0;
}

static duk_ret_t load_json(duk_context *ctx, void *udata) {
  duk_idx_t midx = duk_normalize_index(ctx, -3);
  duk_idx_t iidx = duk_normalize_index(ctx, -4);

  const char *file = duk_require_string(ctx, -1);

  int size = cs_file_size(file);

  if (size == 0) {
    return 0;
  }

  char *buffer = duk_push_fixed_buffer(ctx, size);
  if (!cs_read_file(file, buffer, size, &size)) {
    duk_type_error(ctx, "could not read %s", file);
  }

  duk_buffer_to_string(ctx, -1);
  duk_json_decode(ctx, -1);

  duk_put_prop_string(ctx, midx, "exports");

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
  duk_ret_t ret;
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
      // JS
    } /*else if (strcmp(n + iexts, ".js") == 0) {
      ret = duk_safe_call(ctx, load_javascript, NULL, 0, 0);
      // JSON (only if only)
    } else if (strcmp(n + iexts, ".json") == 0 && len == 1) {
      ret = duk_safe_call(ctx, load_json, NULL, 0, 0);
    } else {
      duk_type_error(ctx, "could not load %s\n", n);
    }*/
    else {
      const char *file = duk_require_string(ctx, -1);
      int size = cs_file_size(file);
      if (size == 0) {
        duk_push_string(ctx, "");
        break;
      }
      char *buffer = duk_push_fixed_buffer(ctx, size);
      if (!cs_read_file(file, buffer, size, &size)) {
        duk_type_error(ctx, "could not read %s", file);
      }

      duk_push_object(ctx);
      duk_dup(ctx, -2);
      duk_put_prop_string(ctx, -2, "content");
      duk_push_string(ctx, n);
      duk_put_prop_string(ctx, -2, "file");
      /*strips_get_entry(ctx, "find_parser");
      duk_dup(ctx, -2);
      duk_ret_t ret = duk_pcall(ctx, 1);
      if (ret != DUK_EXEC_SUCCESS) {
        duk_throw(ctx);
      } else if (!duk_is_function(ctx, -1)) {
        duk_type_error(ctx, "invalid file format");
      }*/
    }

    if (ret != DUK_EXEC_SUCCESS) {
      duk_throw(ctx);
    }
    duk_pop(ctx);
  }

  return 0;
}
