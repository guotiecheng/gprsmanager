/*******************************************************************************
* This file is licensed under the terms of your license agreement(s) with
* GHL covering this file. Redistribution, except as permitted by the terms of
* your license agreement(s) with GHL, is strictly prohibited.
********************************************************************************/

#include "nc_news.h"


/* ==========================================================================================
 * Description: this function is the structure initializer, everytime before use structure
 *              must be initialize using this function. this function will initialize the
 *              communication library.
 *
 * Parameter  : [IN  ] ctx - structure context
 *
 * Return     : 0 - success
 *              else - failure
 * ==========================================================================================
 */
int nc_news_init (NC_NEWS *ctx)
{
  nc_news_term (ctx);

  if (na_sock_init (&ctx->sock, NA_SOCK_TYPE_TCP, 0, 0, 60, 2))
    return -1;

  MEM_ALLOC (ctx->sbuf, NC_NEWS_MAX_FRAME_SIZE, (unsigned char *));
  MEM_ALLOC (ctx->rbuf, NC_NEWS_MAX_FRAME_SIZE, (unsigned char *));

  if (!ctx->sbuf || !ctx->rbuf)
    return -2;

  return 0;
}

/* ==========================================================================================
 * Description: this function is the news structure destructor, everytime after use
 *              news structure must be destroy using this function.
 *              this function will destroy the communication library, after that it will set
 *              the magic number to it. the magic number is use to indicate the structure has
 *              been properly initialized, so that any termination can be done correctly.
 *
 * Parameter  : [IN  ] ctx - structure context
 *
 * Return     : 0 - success
 *              else - failure
 * ==========================================================================================
 */
int nc_news_term (NC_NEWS *ctx)
{
  if (NA_MAGIC == ctx->magic) {
    MEM_CLEAN (ctx->sbuf);
    MEM_CLEAN (ctx->rbuf);

    na_sock_term (&ctx->sock);
  }

  memset (ctx, 0, sizeof (NC_NEWS));
  ctx->magic = NA_MAGIC;

  return 0;
}

/* ==========================================================================================
 * Description: this function is use to retrieve broadcast news from the daemon, the retrieved
 *              news is available in structure.
 *
 * Parameter  : [IN  ] ctx - structure context
 *              [IN  ] app_name - filter news by application name (optional). if application
 *                                name is NULL then the filtering is off
 *
 * Return     : 0 - success
 *              else - failure
 * ==========================================================================================
 */
int nc_news_recv (NC_NEWS *ctx, char *app_name)
{
  int rv;
  unsigned int  rlen;

  if (NA_MAGIC != ctx->magic)
    return -1;

  if (ctx->used) {
    ctx->rlen -= ctx->used;
    ctx->used = 0;

    memmove (ctx->rbuf, ctx->rbuf + ctx->used, ctx->rlen);
    memset (&ctx->rframe, 0, sizeof (NC_FRAME));
  }

  if (!na_sock_connected (&ctx->sock)) {
    rv = -1;
    if (na_sock_connecting (&ctx->sock) ||
        na_time_retry (&ctx->retry)) {
      if (!na_sock_connect (&ctx->sock, NC_NEWS_SERVER_ADDR, 0, NC_NEWS_SERVER_PORT, 0))
        rv = 0;
    }

    if (rv)
      return rv;
  }

  if ((rv = na_sock_recv (&ctx->sock, ctx->rbuf + ctx->rlen, NC_NEWS_MAX_FRAME_SIZE - ctx->rlen, &rlen, 1))) {
    na_sock_close (&ctx->sock, 0);
    ctx->rlen = 0;
    ctx->used = 0;
  }
  else if (!rv && rlen)
    ctx->rlen += rlen;

  if (ctx->rlen) {
    if (nc_frame_parse (&ctx->rframe, ctx->rbuf, ctx->rlen, &ctx->used))
      return -2;

    if (!app_name ||
        !ctx->rframe.dst_app ||
        !ctx->rframe.dst_app [0] ||
        !strcmp (app_name, ctx->rframe.dst_app))
      return 0;
  }

  return -3;
}

/* ==========================================================================================
 * Description: this function is use to send broadcast news to the daemon.
 *
 * Parameter  : [IN  ] ctx - structure context
 *
 * Return     : 0 - success
 *              else - failure
 * ==========================================================================================
 */
int nc_news_send (NC_NEWS *ctx)
{
  unsigned int  hlen;

  unsigned char hbuf [NC_FRAME_LEN];

  if (NA_MAGIC != ctx->magic)
    return -1;

  if (nc_frame_build (&ctx->sframe, hbuf, sizeof (hbuf), &hlen))
    return -2;

  // move existing message forward so that header can be insert infront
  memmove (ctx->sframe.raw_frm + hlen, ctx->sframe.raw_frm, ctx->sframe.raw_len);

  // insert header
  memcpy (ctx->sframe.raw_frm, hbuf, hlen);
  ctx->sframe.raw_len += hlen;

  if (na_sock_connect (&ctx->sock, NC_NEWS_SERVER_ADDR, 0, NC_NEWS_SERVER_PORT, 500) ||
      na_sock_send (&ctx->sock, ctx->sframe.raw_frm, ctx->sframe.raw_len))
    return -3;

  return 0;
}

