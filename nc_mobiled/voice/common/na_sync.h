#ifndef NA_SYNC_H
#define NA_SYNC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "na_common.h"

extern int pthread_mutexattr_settype (pthread_mutexattr_t *attr, int kind);

typedef struct NA_SYNC_T
{
  int             magic,
                  init;

  pthread_mutex_t lock;
}
NA_SYNC;

/* ===============================================================
 * Description: initialize structure
 *
 * Parameter  : [IN  ] ctx - structure context
 * Return     : void
 * ===============================================================
 */
void na_sync_init (NA_SYNC *ctx);

/* ===============================================================
 * Description: destruct structure
 *
 * Parameter  : [IN  ] ctx - structure context
 * Return     : void
 * ===============================================================
 */
void na_sync_term (NA_SYNC *ctx);

/* ===============================================================
 * Description: lock the context with blocking mode
 *
 * Parameter  : [IN  ] ctx - structure context
 * Return     : 0 - success
 *              else - failure
 * ===============================================================
 */
int na_sync_lock (NA_SYNC *ctx);

/* ===============================================================
 * Description: lock the context with non-blocking mode
 *
 * Parameter  : [IN  ] ctx - structure context
 * Return     : 0 - success
 *              else - failure
 * ===============================================================
 */
int na_sync_lock_nb (NA_SYNC *ctx);

/* ===============================================================
 * Description: unlock the context
 *
 * Parameter  : [IN  ] ctx - structure context
 * Return     : 0 - success
 *              else - failure
 * ===============================================================
 */
int na_sync_unlock (NA_SYNC *ctx);

#ifdef __cplusplus
}
#endif

#endif

