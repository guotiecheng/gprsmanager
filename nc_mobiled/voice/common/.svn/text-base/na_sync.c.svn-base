/*******************************************************************************
* This file is licensed under the terms of your license agreement(s) with
* GHL covering this file. Redistribution, except as permitted by the terms of
* your license agreement(s) with GHL, is strictly prohibited.
********************************************************************************/

#include "na_sync.h"

void na_sync_init (NA_SYNC *ctx)
{
  na_sync_term (ctx);

  pthread_mutexattr_t attr;

  pthread_mutexattr_init (&attr);
  pthread_mutexattr_settype (&attr, PTHREAD_MUTEX_RECURSIVE_NP);
  pthread_mutex_init (&ctx->lock, &attr);
  pthread_mutexattr_destroy (&attr);

  ctx->init = 1;
}

void na_sync_term (NA_SYNC *ctx)
{
  if (!ctx ||
      (NA_MAGIC == ctx->magic && !ctx->init))
    return;

  if (NA_MAGIC == ctx->magic && ctx->init) {
    pthread_mutex_lock (&ctx->lock);
    pthread_mutex_destroy (&ctx->lock);
  }

  memset (ctx, 0, sizeof (NA_SYNC));
  ctx->magic = NA_MAGIC;
}

int na_sync_lock (NA_SYNC *ctx)
{
  if (NA_MAGIC != ctx->magic ||
      !ctx->init ||
      pthread_mutex_lock (&ctx->lock))
    return -1;

  return 0;
}

int na_sync_lock_nb (NA_SYNC *ctx)
{
  if (NA_MAGIC != ctx->magic ||
      !ctx->init ||
      pthread_mutex_trylock (&ctx->lock))
    return -1;

  return 0;
}

int na_sync_unlock (NA_SYNC *ctx)
{
  if (NA_MAGIC != ctx->magic ||
      !ctx->init ||
      pthread_mutex_unlock (&ctx->lock))
    return -1;

  return 0;
}

