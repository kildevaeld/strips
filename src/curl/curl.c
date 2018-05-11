#include "../script-data.h"
#include <curl/curl.h>
#include <strips/curl/curl.h>
#include <strips/modules.h>
#include <strips/utils.h>

extern void strips_curl_push_client(duk_context *ctx);
// extern void strips_curl_push_form(duk_context *ctx);
// extern void strips_curl_push_header(duk_context *ctx);
extern void strips_curl_push_request(duk_context *ctx);
extern void strips_curl_push_curl_request(duk_context *ctx);

static duk_ret_t strips_curl_module(duk_context *ctx) {
  duk_push_object(ctx);

  // strips_curl_push_client(ctx);
  // strips_curl_push_form(ctx);
  // strips_curl_push_header(ctx);
  // duk_put_prop_string(ctx, -2, "Client");

  strips_curl_push_request(ctx);
  // We store on stash, for easy access
  duk_push_global_stash(ctx);
  duk_dup(ctx, -2);
  duk_put_prop_string(ctx, -2, "CurlRequest");
  duk_pop(ctx);

  duk_put_prop_string(ctx, -2, "Request");

  strips_curl_push_curl_request(ctx);
  duk_put_prop_string(ctx, -2, "req");

  duk_commonjs_wrapl(ctx, (const char *)curl_js, curl_js_len);
  duk_dup(ctx, -2);
  duk_get_global_string(ctx, "require");

  duk_ret_t ret = duk_pcall(ctx, 2);

  if (ret != DUK_EXEC_SUCCESS) {
    duk_throw(ctx);
  }

  duk_pop(ctx);

  return 1;
}

void strips_curl_init(duk_context *ctx) {
  duk_module_add_fn(ctx, "curl", strips_curl_module);
}