#ifndef NA_TIME_H
#define NA_TIME_H

#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h>
#include <sys/time.h>

#include "na_common.h"

#define NA_TIME_RETRY_FLAG_IMMEDIETELY    1

#define NA_TIME_RETRY_DEF_GAP             30000
#define NA_TIME_RETRY_DEF_INTER           1000
#define NA_TIME_RETRY_DEF_TRY             3


typedef struct NA_TIME_RETRY_T
{
  int             magic,
                  init,
                  retry,
                  tried;

  unsigned int    flags;

  unsigned long   gap,
                  inter,
                  last,
                  wait;
}
NA_TIME_RETRY;

/* ===============================================================
 * Description: suspend thread execution for an interval
 *
 * Parameter  : [IN  ] ms - interval (ms)
 * Return     : void
 * ===============================================================
 */
void na_time_sleep (unsigned long ms);

/* ===============================================================
 * Description: retrieve the system time tick count
 *
 * Parameter  : none
 * Return     : time tick count (ms)
 * ===============================================================
 */
unsigned long na_time_now ();

/* ===============================================================
 * Description: calculate the duration of start time and
 *              current system time
 *
 * Parameter  : [IN  ] start - start time (get from now function)
 * Return     : void
 * ===============================================================
 */
unsigned long na_time_elapsed (unsigned long start);

/* ===============================================================
 * Description: initialize retry structure context
 *
 * Parameter  : [IN  ] ctx - structure context
 *              [IN  ] gap - time to wait after all retry attempt in millisecond
 *              [IN  ] inter - retry interval in millisecond
 *              [IN  ] retry - number of time to retry
 *              [IN  ] flags - flags can be:
 *                            NA_TIME_RETRY_FLAG_IMMEDIETELY - first time immedietely retry
 * Return     : 0 - success
 *              else - failure
 * ===============================================================
 */
int na_time_retry_init (NA_TIME_RETRY *ctx, unsigned long gap, unsigned long inter, int retry, unsigned int flags);

/* ===============================================================
 * Description: calculate the timeout and retry counter
 *
 * Parameter  : [IN  ] ctx - structure context
 * Return     : 0 - no need retry
 *              -1 - retry exceed
 *              else - do retry
 * ===============================================================
 */
int na_time_retry (NA_TIME_RETRY *ctx);

#ifdef __cplusplus
}
#endif

#endif
