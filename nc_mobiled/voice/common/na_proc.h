#ifndef NA_PROC_H
#define NA_PROC_H

#include <pthread.h>
#include <signal.h>
#include <sys/wait.h>
#include <dirent.h>

#include <sys/stat.h>
#include <fcntl.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "na_common.h"

typedef void  (NA_PROC_BODY) (void *_ctx);

typedef int   (*NA_PROC_CHK) (void *any);

void *na_proc_body (void *_ctx);

typedef struct NA_PROC_SYSCONFIG_T
{
  char          name [64],
                val [10];
}
NA_PROC_SYSCONFIG;

typedef struct NA_PROC_ENV_T
{
  char          *name,
                *val;
}
NA_PROC_ENV;

typedef struct NA_PROC_TOKEN_T
{
  int           force;

  NA_PROC_CHK   check;

  void          *hook;
}
NA_PROC_TOKEN;

typedef struct NA_PROC_SIG_T
{
  int           magic,
                end,    // flag for termination
                signal, // received signal
                forked;

  unsigned long pid,    // process id
                id;     // thread id
}
NA_PROC_SIG;

typedef struct NA_PROC_T
{
  int             magic,
                  init,
                  token;

  void            *hook;

  unsigned long   id;

	pthread_t       handle;

  NA_PROC_BODY    *body;
}
NA_PROC;

/* ===============================================================
 * Description: initialize structure
 *
 * Parameter  : [IN  ] ctx - structure context
 *              [IN  ] hook - thread body parameter container
 * Return     : void
 * ===============================================================
 */
void na_proc_init (NA_PROC *ctx, void *hook);

/* ===============================================================
 * Description: destruct structure
 *
 * Parameter  : [IN  ] ctx - structure context
 * Return     : void
 * ===============================================================
 */
void na_proc_term (NA_PROC *ctx);

/* ===============================================================
 * Description: create thread context and start thread
 *
 * Parameter  : [IN  ] ctx - structure context
 *              [IN  ] body - thread process function, once function
 *                           exit then the thread will be terminate
 * Return     : 0 - success
 *              else - failure
 * ===============================================================
 */
int na_proc_run (NA_PROC *ctx, NA_PROC_BODY *body);

/* ===============================================================
 * Description: end thread
 *
 * Parameter  : [IN  ] ctx - structure context
 *              [IN  ] signal - signal that thread will receive (optional)
 * Return     : 0 - success
 *              else - failure
 * ===============================================================
 */
int na_proc_end (NA_PROC *ctx, int signal);

/* ===============================================================
 * Description: check thread process token whether allow continue
 *              execution or not
 *
 * Parameter  : [IN  ] ctx - structure context
 * Return     : 0 - owned token
 *              else - requested to end
 * ===============================================================
 */
int na_proc_token (NA_PROC *ctx, int global);

/* ===============================================================
 * Description: setup signal handler
 *
 * Parameter  : void
 * Return     : void
 * ===============================================================
 */
void na_proc_signal ();

/* ===============================================================
 * Description: process signal handler
 *
 * Parameter  : [IN  ] signal - received signal from system
 * Return     : void
 * ===============================================================
 */
void na_proc_handler (int signal);

/* ===============================================================
 * Description: execute shell script or program
 *
 * Parameter  : [IN  ] file - executable shell script or program
 *              [IN  ] argv - process parameter
 *              [IN  ] timeout - the maximum time frame
 *              [IN  ] env - extra environment parameter
 *              [IN  ] token - check thread process token whether
 *                            allow continue execution or not
 * Return     : 0 - success
 *              else - failure return code
 * ===============================================================
 */
int na_proc_shell (char *file, char *argv [], unsigned long timeout, NA_PROC_ENV *env, NA_PROC_TOKEN *token);

/* ===============================================================
 * Description: find process with the specified name
 *
 * Parameter  : [IN  ] name - process name
 *              [IN  ] exclude_me - exclude current process while searching
 * Return     : 0 - not found
 *              else - process id
 * ===============================================================
 */
unsigned long na_proc_find (char *name, int exclude_me);

/* ===============================================================
 * Description: daemonize the current process
 *
 * Parameter  : [IN  ] console - close the stdin, stdout and stderr
 *                              0: close
 *                              1: remain unchange
 * Return     : 0 - success
 *              else - failure
 * ===============================================================
 */
int na_proc_daemonize (int console, char *app, char *argv []);

unsigned long na_proc_id ();

#ifdef __cplusplus
}
#endif

#endif

