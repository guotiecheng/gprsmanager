#ifndef NC_NEWS_H
#define NC_NEWS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "libna.h"
#include "nc_frame.h"

#define NC_NEWS_MAX_FRAME_SIZE              2048

#define NC_NEWS_SERVER_MAX_CLIENT           128
#define NC_NEWS_SERVER_ADDR                 "127.0.0.1"
#define NC_NEWS_SERVER_PORT                 6900

typedef struct NC_NEWS_T
{
  int           magic;

  unsigned char *rbuf,
                *sbuf;

  unsigned int  rlen,
                used;

  NC_FRAME      rframe,
                sframe;

  NA_TIME_RETRY retry;

  NA_SOCK       sock;
}
NC_NEWS;


// ================
// ** PUBLIC USE **
// ================

int nc_news_init (NC_NEWS *ctx);
int nc_news_term (NC_NEWS *ctx);
int nc_news_recv (NC_NEWS *ctx, char *app_name);
int nc_news_send (NC_NEWS *ctx);


#ifdef __cplusplus
}
#endif

#endif

