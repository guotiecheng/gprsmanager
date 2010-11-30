/*******************************************************************************
* This file is licensed under the terms of your license agreement(s) with
* GHL covering this file. Redistribution, except as permitted by the terms of
* your license agreement(s) with GHL, is strictly prohibited.
********************************************************************************/

#include "na_time.h"

void na_time_sleep (unsigned long ms)
{
  ms *= 1000;
  usleep (ms);
}

unsigned long na_time_now ()
{
  struct timeval now;

  gettimeofday (&now, 0);
  return (now.tv_sec * 1000) + (now.tv_usec / 1000);
}

unsigned long na_time_elapsed (unsigned long start)
{
  unsigned long current = na_time_now ();

  if (current < start)
    return (0xFFFFFFFF - start) + current;

  return current - start;
}

int na_time_retry_init (NA_TIME_RETRY *ctx, unsigned long gap, unsigned long inter, int retry, unsigned int flags)
{
  if (NA_MAGIC != ctx->magic) {
    memset (ctx, 0, sizeof (NA_TIME_RETRY));
    ctx->magic = NA_MAGIC;
  }

  ctx->gap = gap;
  ctx->wait = gap;
  ctx->inter = inter;
  ctx->retry = (0 <= retry) ? retry : 0;
  ctx->flags = flags;

  ctx->init = 1;
  return 0;
}

int na_time_retry (NA_TIME_RETRY *ctx)
{
  if (NA_MAGIC != ctx->magic || !ctx->init) {
    na_time_retry_init (ctx, NA_TIME_RETRY_DEF_GAP, NA_TIME_RETRY_DEF_INTER, NA_TIME_RETRY_DEF_TRY, NA_TIME_RETRY_FLAG_IMMEDIETELY);
  }

  if (!ctx->last) {
    ctx->last = na_time_now ();

    if (NA_TIME_RETRY_FLAG_IMMEDIETELY & ctx->flags)
      return -2;
  }

  if (ctx->wait < na_time_elapsed (ctx->last)) {
    ctx->last = na_time_now ();

    if (ctx->retry) {
      if (ctx->tried++ == ctx->retry) {
        ctx->tried = 0;
        ctx->wait = ctx->gap;
        return -1;
      }
      ctx->wait = ctx->inter;
    }
    else
      ctx->wait = ctx->gap;

    return -2;
  }

  return 0;
}

