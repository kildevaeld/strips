#include <dirent.h>
#include <errno.h>
#include <stdlib.h>
#include <strips/exec/exec.h>
#include <strips/modules.h>
#include <sys/wait.h>
#include <unistd.h>
#include <which.h>

static duk_ret_t duk_exec_exec(duk_context *ctx) {

  const char *cmdname = duk_require_string(ctx, 0);

  int top = duk_get_top(ctx);
  char *args[top + 1];

  for (int i = 1; i < top; i++) {
    args[i] = duk_get_string(ctx, i);
  }
  args[0] = cmdname;
  args[top] = NULL;

  int filedes[2];
  if (pipe(filedes) == -1) {
    perror("pipe");
    exit(1);
  }

  char *cmdpath = which(cmdname);
  if (!cmdpath) {
    duk_type_error(ctx, "failed to look up path");
  }

  pid_t pid = fork();
  if (pid == -1) {
    perror("fork");
    exit(1);
  } else if (pid == 0) {
    while ((dup2(filedes[1], STDOUT_FILENO) == -1) && (errno == EINTR)) {
    }
    close(filedes[1]);
    close(filedes[0]);
    execvp(cmdpath, args);
    // execl(cmdpath, cmdname, (char *)0);
    perror("execl");
    _exit(1);
  }
  close(filedes[1]);

  char buffer[4096];

  char *out = duk_push_dynamic_buffer(ctx, 0);
  int out_size = 0;

  while (1) {
    ssize_t count = read(filedes[0], buffer, sizeof(buffer));
    if (count == -1) {
      if (errno == EINTR) {
        continue;
      } else {
        perror("read");
        exit(1);
      }
    } else if (count == 0) {
      break;
    } else {
      int idx = out_size;
      if (count > out_size) {
        out_size += count;
        out = duk_resize_buffer(ctx, -1, out_size);
      }
      memcpy(out + idx, buffer, count);
    }
  }
  close(filedes[0]);
  wait(0);

  return 1;
}

static duk_ret_t duk_exec_module(duk_context *ctx) {
  duk_push_object(ctx);

  duk_push_c_function(ctx, duk_exec_exec, DUK_VARARGS);
  duk_put_prop_string(ctx, -2, "exec");

  return 1;
}

void strips_exec_init(duk_context *vm) {
  duk_module_add_fn(vm, "exec", duk_exec_module);
}