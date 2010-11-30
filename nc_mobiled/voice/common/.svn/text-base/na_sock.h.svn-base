#ifndef NA_SOCK_H
#define NA_SOCK_H

#ifdef __cplusplus
extern "C" {
#endif

#include <linux/sockios.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/un.h>

#include "na_common.h"
#include "na_network.h"

#define NA_SOCK_TYPE_TCP          1
#define NA_SOCK_TYPE_UDP          2

#define NA_SOCK_FLAG_CONNECT      1
#define NA_SOCK_FLAG_CONNECTING   2
#define NA_SOCK_FLAG_NEW_CONNECT  4

#define NA_SOCK_MODE_NONE         0
#define NA_SOCK_MODE_IN           1
#define NA_SOCK_MODE_OUT          2
#define NA_SOCK_MODE_LISTENER     3

typedef struct NA_SOCK_T
{
  int                 magic,
                      init,
                      type,
                      mode,
                      sock,

                      lport,
                      rport,

                      keepalive_interval,
                      keepalive_retry,
                      err_no;

  unsigned int        flags,
                      ssize,
                      rsize;

  char                laddr [256],
                      raddr [256];

  struct sockaddr_in  saddr;

  // flag definitions
  // 0 ... 0 0 0 0 0 0 0 0
  // |     | | | | | | | |___________ connect flag    - 0: not connected, 1: connected
  // |     | | | | | | |_____________ connecting flag - 0: connected, 1: connecting
  // |     | | | | | |_______________ new flag        - 0: not newly connected, 1: newly connected
  // |_____|_|_|_|_|_________________ reserved

}
NA_SOCK;

typedef struct NA_SOCK_FD_T
{
  int     raised;

  void    *hook;

  NA_SOCK *sock;
}
NA_SOCK_FD;

typedef struct NA_SOCK_FDS_T
{
  int                 magic,
                      init;

  NA_ARRAY            fds;
}
NA_SOCK_FDS;


int na_sock_init (NA_SOCK *ctx, int type, unsigned int rsize, unsigned int ssize, int keepalive_interval, int keepalive_retry);
int na_sock_term (NA_SOCK *ctx);
int na_sock_close (NA_SOCK *ctx, int force);

int na_sock_listen (NA_SOCK *ctx, char *addr, int lport, int backlog);
int na_sock_accept (NA_SOCK *ctx, NA_SOCK *nsock, unsigned long timeout);
int na_sock_connect (NA_SOCK *ctx, char *addr, int lport, int rport, unsigned long timeout);

int na_sock_send (NA_SOCK *ctx, unsigned char *sbuf, unsigned int slen);
int na_sock_recv (NA_SOCK *ctx, unsigned char *rbuf, unsigned int rsize, unsigned int *rlen, unsigned long timeout);
int na_sock_connected (NA_SOCK *ctx);
int na_sock_connecting (NA_SOCK *ctx);
int na_sock_set_io (NA_SOCK *ctx, int blocking);


int na_sock_fds_init (NA_SOCK_FDS *ctx, int size);
void na_sock_fds_term (NA_SOCK_FDS *ctx);
int na_sock_fds_proc (NA_SOCK_FDS *ctx);

int na_sock_fds_add (NA_SOCK_FDS *ctx, NA_SOCK *sock, void *hook);
int na_sock_fds_del (NA_SOCK_FDS *ctx, NA_SOCK *sock);

#ifdef __cplusplus
}
#endif

#endif
