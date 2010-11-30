/*******************************************************************************
* This file is licensed under the terms of your license agreement(s) with
* GHL covering this file. Redistribution, except as permitted by the terms of
* your license agreement(s) with GHL, is strictly prohibited.
********************************************************************************/

#include "na_proc.h"

NA_PROC_SIG _na_signal = { 0 };

void na_proc_signal ()
{
  if (NA_MAGIC != _na_signal.magic) {
    _na_signal.magic = NA_MAGIC;

    struct sigaction  sigact,
                      sigign;

    sigemptyset (&sigact.sa_mask);
    sigact.sa_flags = 0;
    sigact.sa_handler = na_proc_handler;

    sigemptyset (&sigign.sa_mask);
    sigign.sa_flags = 0;
    sigign.sa_handler = SIG_IGN;

    sigaction (SIGTERM, &sigact, 0);	// catch terminate signal
    sigaction (SIGINT, &sigact, 0);		// catch interrupt signal
    sigaction (SIGSEGV, &sigact, 0);	// catch segmentation faults

    sigaction (SIGCHLD, &sigign, 0);	// catch child process return
    sigaction (SIGPIPE, &sigign, 0);	// catch broken pipe
  }
}

void na_proc_init (NA_PROC *ctx, void *hook)
{
  // setup signal handler if any
  na_proc_signal ();

  na_proc_term (ctx);

  ctx->hook = hook;
  ctx->init = 1;
}

void na_proc_term (NA_PROC *ctx)
{
  if (!ctx ||
      (NA_MAGIC == ctx->magic && !ctx->init))
    return;

  if (NA_MAGIC == ctx->magic && ctx->init)
    na_proc_end (ctx, SIGINT);

  memset (ctx, 0, sizeof (NA_PROC));
  ctx->magic = NA_MAGIC;
}

int na_proc_run (NA_PROC *ctx, NA_PROC_BODY *body)
{
  if (NA_MAGIC != ctx->magic ||
      !ctx->init ||
      !body)
    return -1;

  if (ctx->id)
    return 0;

  ctx->body = body;
  ctx->token = 1;

  if (pthread_create (&ctx->handle, 0, na_proc_body, (void *) ctx)) {
    return -2;
  }
  // get thread id
  ctx->id = (unsigned long) ctx->handle;
//  if (byte_to_ulong (&ctx->handle, sizeof (ctx->handle), &ctx->id) && ctx->id)
//    return -3;
  return 0;
}

int na_proc_end (NA_PROC *ctx, int signal)
{
  if (NA_MAGIC != ctx->magic ||
      !ctx->init)
    return -1;

  ctx->token = 0;

  if (ctx->id) {
    if (signal) {
      if (pthread_kill (ctx->handle, signal))
        return -2;
    }
    if (pthread_join (ctx->handle, 0))
      return -3;
    ctx->id = 0;
  }

  return 0;
}

int na_proc_token (NA_PROC *ctx, int global)
{
  return ((ctx && NA_MAGIC == ctx->magic && ctx->init && !ctx->token) ||
          (global && !_na_signal.end));
}

void *na_proc_body (void *_ctx)
{
  NA_PROC *ctx = (NA_PROC *) _ctx;

  ctx->body (_ctx);
  pthread_exit (0);

  return 0;
}

void na_proc_handler (int signal)
{
  switch (signal) {
  case SIGINT:
    // only terminate when on console mode (not daemonized)
    if (!_na_signal.forked)
      _na_signal.end = 1;
    break;

  case SIGTERM:
  case SIGSEGV:
    if (_na_signal.signal != signal) {
      _na_signal.signal = signal;
      _na_signal.pid = getpid ();
      _na_signal.id = pthread_self ();
    }

    _na_signal.end = 1;
    break;
  }
}

int na_proc_shell (char *file, char *argv [], unsigned long timeout, NA_PROC_ENV *env, NA_PROC_TOKEN *token)
{
  int           rv, status;

  unsigned long start;

  pid_t         child,
                proc;

  if (-1 == (child = vfork ())) {
    rv = errno ? errno : -1;
    goto cleanup;
  }

  if (!child) { // child here
    for (rv = getdtablesize (); 3 <= rv; rv--) {
      CLOSE (rv); // close all descriptor except stdin (0), stdout (1), strerr (2)
    }
    setpgid (0, 0); // set a new group ID

    // set environment variable
    while (env && env->name && env->val) {
      setenv (env->name, env->val, 1);
      env++;
    }

    printf ("execute %s ", file);

    if (argv) {
      rv = 1;
      while (argv [rv]) {
        printf ("%s ", argv [rv]);
        rv++;
      }
    }
    printf ("\n");

    if (!argv)
      rv = system (file);
    else {
      execvp (file, argv);

      // we should not reach here, unless got failure
      rv = errno ? errno : -2;
    }

    _exit (rv);
  }

  // parent here
  start = na_time_now ();

  do {
    if (-1 == (proc = waitpid (child, &rv, WNOHANG | WUNTRACED))) {
      rv = errno ? errno : -3;
      goto cleanup;
    }

    if (proc) {
      if (WIFEXITED (rv)) {
        if (!(status = WEXITSTATUS (rv)))
          rv = 0;
        else {
          rv = status;
        }
      }
      else
        rv = -5;
      goto cleanup;
    }

    na_time_sleep (1);

    if (token) {
      if (!token->force &&
          !na_proc_token (0, 1))
        break;

      if (token->check &&
          token->check (token->hook))
        break;
    }
  }
  while (timeout > na_time_elapsed (start));

  // timeout occur, we need to kill it
  killpg (child, SIGTERM);
  waitpid (child, 0, 0);
  rv = -6;

cleanup:

  return rv;
}

unsigned long na_proc_find (char *name, int exclude_me)
{
  unsigned long match = 0;

  char          buf [NAME_MAX + 15],
                *frm,
                *end;

  DIR           *entry;

  struct dirent *dir;

  pid_t         pid   = 0,
                me    = 0;

  FILE          *fd   = 0;

  if (exclude_me)
    me = getpid ();

  if (!(entry = opendir ("/proc")))
    return 0;

  while ((dir = readdir (entry))) {
    if (dir->d_name && na_is_numeric (dir->d_name) && DT_DIR == dir->d_type) {
      pid = atol (dir->d_name);

      if (exclude_me && pid == me)
        continue;

      sprintf (buf, "/proc/%s/stat", dir->d_name);

      FOPEN (fd, buf, "rb");
      if (!fd)
        continue;

      while (fgets (buf, sizeof (buf) - 1, fd)) {
        if ((frm = strchr (buf, '(')) && (end = strchr (++frm, ')'))) {
          *end = 0;
          if (!strcmp (frm, name)) {
            match = (unsigned long) pid;
            break;
          }
        }
      }

      FCLOSE (fd);
    }
  }

  closedir (entry);

  return match;
}

int na_proc_kill (char *name, int signal)
{
  unsigned long pid;

  if ((pid = na_proc_find (name, 1))) {
    if (!signal)
      signal = SIGTERM;

    killpg (pid, signal);
    waitpid (pid, 0, 0);
  }

  return 0;
}

int na_proc_daemonize (int console, char *app, char *argv [])
{
  int   index,
        fd;

  pid_t child;

  if (1 != getppid ()) {
    child = fork ();

    if (0 < child) _exit (0); // parent exists

    // fork error
    if (0 > child) {
      if (38 == errno) {
        // try vfork
        child = vfork ();
        if (0 < child) _exit (0); // parent exists
        if (0 > child) _exit (1); // fork error
        // child (daemon) continues

        child = vfork ();
        if (0 < child) _exit (0); // parent exists
        if (0 > child) _exit (1); // fork error
        // child (daemon) continues

        execv (app, argv);
      }
      _exit (1);
    }
  }

  setsid ();  // obtain a new process group

  // close any open file descriptor
  for (index = getdtablesize (); 3 < index; index--) {
    CLOSE (index);
  }

  if (!console) {
    for (index = 2; 0 <= index; index--) {
      fd = index;
      CLOSE (fd);
    }
    OPEN (fd, "/dev/null", O_RDWR);
    dup (fd); dup (fd); // handle standard I/O
    umask (027); // set newly created file permissions
  }

  chdir ("/"); // change running directory

  // setup signal handler
  na_proc_signal ();

  _na_signal.forked = 1;

  return 1;
}

unsigned long na_proc_id ()
{
  return (unsigned long) getpid ();
}
