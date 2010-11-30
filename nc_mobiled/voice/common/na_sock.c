/*******************************************************************************
* This file is licensed under the terms of your license agreement(s) with
* GHL covering this file. Redistribution, except as permitted by the terms of
* your license agreement(s) with GHL, is strictly prohibited.
********************************************************************************/

#include "na_sock.h"

/* ===============================================================
 * Description: initialize structure
 *
 * Parameter  : [IN  ] ctx - structure context
 *              [IN  ] type - type of socket, it can be as below
 *                           NA_SOCK_TYPE_TCP - TCP/IP socket
 *                           NA_SOCK_TYPE_UDP - UDP/IP socket
 *              [IN  ] keepalive_interval - socket internal keepalive
 *                                         interval (only for TCP/IP)
 *              [IN  ] keepalive_retry - socket internal keepalive
 *                                      retry counter (only for TCP/IP)
 * Return     : 0 - success
 *              else - failure
 * ===============================================================
 */
int na_sock_init (NA_SOCK *ctx, int type, unsigned int rsize, unsigned int ssize, int keepalive_interval, int keepalive_retry)
{
  if (NA_SOCK_TYPE_TCP != type && NA_SOCK_TYPE_UDP != type)
    return -1;

  na_sock_term (ctx);

  ctx->type = type;
  ctx->rsize = rsize;
  ctx->ssize = ssize;
  ctx->keepalive_interval = keepalive_interval;
  ctx->keepalive_retry = keepalive_retry;
  ctx->init = 1;

  return 0;
}

/* ===============================================================
 * Description: destruct structure
 *
 * Parameter  : [IN  ] ctx - structure context
 * Return     : 0 - success
 *              else - failure
 * ===============================================================
 */
int na_sock_term (NA_SOCK *ctx)
{
  return na_sock_close (ctx, 0);
}

/* ===============================================================
 * Description: close and release socket resources
 *
 * Parameter  : [IN  ] ctx - structure context
 * Return     : 0 - success
 *              else - failure
 * ===============================================================
 */
int na_sock_close (NA_SOCK *ctx, int force)
{
  int           rv,
                init                = 0,
                type                = 0,
                rsize               = 0,
                ssize               = 0,
                keepalive_interval  = 0,
                keepalive_retry     = 0;

  struct linger ling;

  if (!ctx ||
      (NA_MAGIC == ctx->magic && !ctx->init))
    return -1;

  if (NA_MAGIC == ctx->magic && ctx->init) {
    if (0 < ctx->sock) {
      if (NA_SOCK_TYPE_TCP == ctx->type &&
          !force) {
        ling.l_onoff = -1;
        ling.l_linger = 0;

        // to prevent connection stated as TIME_WAIT
        setsockopt (ctx->sock, SOL_SOCKET, SO_LINGER, (char *) &ling, sizeof (struct linger));

        if (NA_SOCK_MODE_IN == ctx->mode || NA_SOCK_MODE_OUT == ctx->mode)
          shutdown (ctx->sock, SHUT_RDWR);
      }

      SOCK_CLOSE_EX (rv, ctx->sock);
      if (rv) {
        ctx->err_no = errno;
        return -2;
      }
    }

    init = ctx->init;
    type = ctx->type;
    rsize = ctx->rsize;
    ssize = ctx->ssize;
    keepalive_interval = ctx->keepalive_interval;
    keepalive_retry = ctx->keepalive_retry;
  }

  memset (ctx, 0, sizeof (NA_SOCK));

  ctx->type = type;
  ctx->rsize = rsize;
  ctx->ssize = ssize;
  ctx->keepalive_interval = keepalive_interval;
  ctx->keepalive_retry = keepalive_retry;
  ctx->magic = NA_MAGIC;
  ctx->init = init;

  return 0;
}

/* ===============================================================
 * Description: get all interface info
 *
 * Parameter  : [IN  ] ctx - structure context
 *              [OUT ] iflist - all the interface store into list
 * Return     : 0 - success
 *              else - failure
 * ===============================================================
 */
int na_sock_listen (NA_SOCK *ctx, char *addr, int lport, int backlog)
{
  int             rv,
                  sock  = -1,
                  len,
                  opt;

  struct hostent  *host;

  ctx->err_no = 0;

  if (NA_MAGIC != ctx->magic ||
      !ctx->init ||
      0 >= lport ||
      0 >= backlog)
    return -1;

  if (NA_SOCK_FLAG_CONNECT & ctx->flags)
    return 0;

  if (NA_SOCK_TYPE_TCP == ctx->type) {
    SOCK_ALLOC (sock, AF_INET, SOCK_STREAM, 0);
    if (0 >= sock) {
      rv = -2;
      goto cleanup;
    }
  }

  na_sock_close (ctx, 0);  // close any opened socket

  ctx->sock = sock;
  sock = -1;

  if (addr && 0 != *addr && strcmp (addr, "0.0.0.0")) {
    ctx->saddr.sin_family = AF_INET;

    if (!inet_aton (addr, &ctx->saddr.sin_addr)) {
      if ((host = gethostbyname (addr))) {  // resolve domain name into ip address
        ctx->saddr.sin_family = host->h_addrtype;

        if (host->h_length > (int) sizeof (ctx->saddr.sin_addr))
          host->h_length = sizeof (ctx->saddr.sin_addr);

        memcpy (&ctx->saddr.sin_addr, host->h_addr, host->h_length);
      }
      else {
        rv = -3;
        goto cleanup;
      }
    }
  }
  else {
    ctx->saddr.sin_family = AF_INET;
    ctx->saddr.sin_addr.s_addr = htonl(INADDR_ANY);
  }

  ctx->saddr.sin_port = htons ((u_short) lport);

  memset (ctx->saddr.sin_zero, 0, sizeof (ctx->saddr.sin_zero));

  // set reuse local address flag
  len = sizeof (opt);
  opt = 1;
  if (setsockopt (ctx->sock, SOL_SOCKET, SO_REUSEADDR, (void *) &opt, len)) {
    rv = -4;
    goto cleanup;
  }

  // bind socket to local interface
  len = sizeof (ctx->saddr);
  if (bind (ctx->sock, (struct sockaddr *) &ctx->saddr, len)) {
    rv = -5;
    goto cleanup;
  }

  if (NA_SOCK_TYPE_TCP == ctx->type) {
    // listen on local interface
    if (listen (ctx->sock, backlog)) {
      rv = -6;
      goto cleanup;
    }
  }

  ctx->lport = lport;
  strncpy (ctx->laddr, inet_ntoa (ctx->saddr.sin_addr), sizeof (ctx->laddr) - 1);

  ctx->flags |= NA_SOCK_FLAG_CONNECT;
  ctx->mode = NA_SOCK_MODE_LISTENER;
  rv = 0;

cleanup:
  if (rv) {
    ctx->err_no = errno;
    SOCK_CLOSE (ctx->sock);
  }

  SOCK_CLOSE (sock);

  return rv;
}

int na_sock_accept (NA_SOCK *ctx, NA_SOCK *nsock, unsigned long timeout)
{
  int                 rv,
                      sock      = -1,
                      opt,
                      len;

  fd_set              read_fds,
                      excp_fds;

  struct timeval      time;

  struct sockaddr_in  naddr;

  ctx->err_no = 0;

  if (NA_MAGIC != ctx->magic ||
      !ctx->init ||
      NA_SOCK_TYPE_TCP != ctx->type)
    return -1;

  if (0 >= ctx->sock ||
      !(NA_SOCK_FLAG_CONNECT & ctx->flags))
    return -2;

  if (0xFFFFFFFF != timeout) {
    FD_ZERO (&read_fds);
    FD_ZERO (&excp_fds);

    FD_SET (ctx->sock, &read_fds);
    FD_SET (ctx->sock, &read_fds);

    if (0 != timeout) {
      time.tv_sec = timeout / 1000;
      time.tv_usec = 1000 * (timeout % 1000);
    }
    else {
      time.tv_sec = 0;
      time.tv_usec = 0;
    }

    if (0 > (rv = select (ctx->sock + 1, &read_fds, 0, &excp_fds, &time)) ||
        FD_ISSET (ctx->sock, &excp_fds)) {
      if (EINTR == (ctx->err_no = errno))
        rv = -3;  // catched interrupted signal
      else
        rv = -4;
      goto cleanup;
    }
    else if (!rv || !FD_ISSET (ctx->sock, &read_fds)) {
      rv = 0; // no incoming connection
      goto cleanup;
    }
  }

  len = sizeof (naddr);
  SOCK_ACCEPT (sock, ctx->sock, (struct sockaddr *) &naddr, &len);

  if (0 >= sock) {
    if (EINTR == (ctx->err_no = errno))
      rv = -3;  // catched interrupted signal
    else
      rv = -4;
    goto cleanup;
  }

  // no new socket structure provided then we just accept and close it immediately
  if (!nsock) {
    rv = 0;
    goto cleanup;
  }

  if (na_sock_init (nsock, ctx->type, ctx->rsize, ctx->ssize, ctx->keepalive_interval, ctx->keepalive_retry))
    return -5;

  memcpy (&nsock->saddr, &naddr, len);
  nsock->sock = sock;
  sock = -1;

  // set recv buffer size
  if (ctx->rsize) {
    opt = (int) ctx->rsize;
    if (setsockopt (nsock->sock, SOL_SOCKET, SO_RCVBUF, (char *) &opt, sizeof (opt))) {
      rv = -6;
      goto cleanup;
    }
  }

  // set send buffer size
  if (ctx->ssize) {
    opt = (int) ctx->ssize;
    if (setsockopt (nsock->sock, SOL_SOCKET, SO_SNDBUF, (char *) &opt, sizeof (opt))) {
      rv = -6;
      goto cleanup;
    }
  }

  // set keepalive flag
  opt = 1;
  if (setsockopt (nsock->sock, SOL_SOCKET, SO_KEEPALIVE, (char *) &opt, sizeof (opt))) {
    rv = -7;
    goto cleanup;
  }

  if (0 < ctx->keepalive_interval || 0 < ctx->keepalive_retry) {
    opt = 1;  // 1 second
    if (setsockopt (nsock->sock, SOL_TCP, TCP_KEEPIDLE, (char *) &opt, sizeof (opt))) {
      rv = -7;
      goto cleanup;
    }

    if (0 < ctx->keepalive_interval) {
      opt = ctx->keepalive_interval;
      if (setsockopt (nsock->sock, SOL_TCP, TCP_KEEPINTVL, (char *) &opt, sizeof (opt))) {
        rv = -7;
        goto cleanup;
      }
    }

    if (0 < ctx->keepalive_retry) {
      opt = ctx->keepalive_retry;
      if (setsockopt (nsock->sock, SOL_TCP, TCP_KEEPCNT, (char *) &opt, sizeof (opt))) {
        rv = -7;
        goto cleanup;
      }
    }
  }

  // set socket to non-blocking
  opt = 1;
  if (ioctl (nsock->sock, FIONBIO, &opt)) {
    rv = -8;
    goto cleanup;
  }

  // get local address
  len = sizeof (naddr);
  if (getsockname (nsock->sock, (struct sockaddr *) &naddr, (socklen_t *) &len)) {
    strncpy (nsock->laddr, ctx->laddr, sizeof (nsock->laddr) - 1);
    nsock->lport = ctx->lport;
  }
  else {
    strncpy (nsock->laddr, inet_ntoa (naddr.sin_addr), sizeof (nsock->laddr) - 1);
    nsock->lport = ntohs (naddr.sin_port);
  }

  // get remote address
  strncpy (nsock->raddr, inet_ntoa (nsock->saddr.sin_addr), sizeof (nsock->raddr) - 1);
  nsock->rport = ntohs (nsock->saddr.sin_port);

  nsock->mode = NA_SOCK_MODE_IN;
  nsock->flags |= NA_SOCK_FLAG_CONNECT;
  nsock->flags |= NA_SOCK_FLAG_NEW_CONNECT;

  rv = 0;

cleanup:
  SOCK_CLOSE (sock);

  return rv;
}

int na_sock_connect (NA_SOCK *ctx, char *addr, int lport, int rport, unsigned long timeout)
{
  int             rv,
                  sock    = -1,
                  opt,
                  len;

  char            *ptr;

  struct hostent  *host;

  fd_set          sendfds;

  struct timeval  time;

  if (NA_MAGIC != ctx->magic ||
      !ctx->init ||
      !addr ||
      0 == *addr ||
      0 > lport ||
      0 >= rport ||
      NA_SOCK_TYPE_TCP != ctx->type)
    return -1;

  if (NA_SOCK_FLAG_CONNECT & ctx->flags) {
    // remove the new connect flag
    if (NA_SOCK_FLAG_NEW_CONNECT & ctx->flags)
      ctx->flags &= (~NA_SOCK_FLAG_NEW_CONNECT);
    return 0;
  }

  if (NA_SOCK_FLAG_CONNECTING & ctx->flags)
    goto connecting;

  SOCK_ALLOC (sock, AF_INET, SOCK_STREAM, 0);
  if (0 >= sock)
    return -2;

  na_sock_close (ctx, 0);

  ctx->sock = sock;
  sock = -1;

  ctx->saddr.sin_family = AF_INET;

  if (0 < lport) {
    // set reuse local port
    opt = 1;
    len = sizeof (opt);
    if (setsockopt (ctx->sock, SOL_SOCKET, SO_REUSEADDR, (void *) &opt, len)) {
      ctx->err_no = errno;
      rv = -3;
      goto cleanup;
    }

    // bind local port
    memset (&ctx->saddr.sin_zero, 0, sizeof (ctx->saddr.sin_zero));
    ctx->saddr.sin_port = htons ((u_short) lport);
    len = sizeof (ctx->saddr);

    if (bind (ctx->sock, (struct sockaddr *) &ctx->saddr, len)) {
      ctx->err_no = errno;
      rv = -4;
      goto cleanup;
    }
  }

  // convert dot value into ip address
  if (!inet_aton (addr, &ctx->saddr.sin_addr)) {
    // resolve domain name into ip address
    if ((host = gethostbyname (addr))) {
      ctx->saddr.sin_family = host->h_addrtype;

      if (host->h_length > (int) sizeof (ctx->saddr.sin_addr))
        host->h_length = sizeof (ctx->saddr.sin_addr);
      memcpy (&ctx->saddr.sin_addr, host->h_addr, host->h_length);
    }
    else {
      rv = -5;
      goto cleanup;
    }
  }

  ctx->saddr.sin_port = htons ((u_short) rport);

  // set receive buffer size
  if (0 < ctx->rsize) {
    opt = ctx->rsize;
    len = sizeof (opt);
    if (setsockopt (ctx->sock, SOL_SOCKET, SO_RCVBUF, (char *) &opt, len)) {
      rv = -6;
      goto cleanup;
    }
  }

  // set send buffer size
  if (0 < ctx->ssize) {
    opt = ctx->ssize;
    len = sizeof (opt);
    if (setsockopt (ctx->sock, SOL_SOCKET, SO_SNDBUF, (char *) &opt, len)) {
      rv = -6;
      goto cleanup;
    }
  }

  // set keepalive flag
  opt = 1;
  len = sizeof (opt);
  if (setsockopt (ctx->sock, SOL_SOCKET, SO_KEEPALIVE, (char *) &opt, len)) {
    rv = -6;
    goto cleanup;
  }

  if (0 < ctx->keepalive_interval) {
    opt = ctx->keepalive_interval;
    len = sizeof (opt);
    if (setsockopt (ctx->sock, SOL_TCP, TCP_KEEPIDLE, (char *) &opt, len)) {
      rv = -6;
      goto cleanup;
    }
  }

  if (0 < ctx->keepalive_retry) {
    opt = ctx->keepalive_retry;
    len = sizeof (opt);
    if (setsockopt (ctx->sock, SOL_TCP, TCP_KEEPCNT, (char *) &opt, len)) {
      rv = -6;
      goto cleanup;
    }
  }

  // allocate memory to store remote host name or ip address
  // CAUTION: To prevent self copy (passed in local module IP variable)
  if (ctx->raddr != addr) {
    memset (ctx->raddr, 0, sizeof (ctx->raddr));
    strncpy (ctx->raddr, addr, sizeof (ctx->raddr) - 1);
  }

  ctx->rport = rport;

  // set socket to non-blocking
  opt = 1;
  if (ioctl (ctx->sock, FIONBIO, &opt)) {
    rv = -7;
    goto cleanup;
  }

  // establish connection to remote host
  if (!connect (ctx->sock, (struct sockaddr *) &ctx->saddr, sizeof (struct sockaddr_in))) {
    ctx->flags |= NA_SOCK_FLAG_CONNECT;
  }
  else {
    ctx->err_no = errno;

    if (EINPROGRESS != ctx->err_no) {
      rv = -8;
      goto cleanup;
    }

    ctx->flags |= NA_SOCK_FLAG_CONNECTING;
  }

connecting:

  FD_ZERO (&sendfds);
  FD_SET (ctx->sock, &sendfds);

  if (timeout) {
    time.tv_sec = timeout / 1000;
    time.tv_usec = 1000 * (timeout % 1000);
  }
  else {
    time.tv_sec = 0;
    time.tv_usec = 1;
  }

  if (0 > (rv = select (ctx->sock + 1, 0, &sendfds, 0, &time))) {
    ctx->err_no = errno;

    rv = -8;  // connection timeout
    goto cleanup;
  }
  else if (!rv) {
    if (timeout)
      rv = -8;  // connection timeout
    else
      rv = 1; // connection pending
    goto cleanup;
  }
  else if (0 < rv) {
    if (!FD_ISSET (ctx->sock, &sendfds)) {
      rv = -8;
      goto cleanup;
    }

    opt = 1;
    len = sizeof (opt);
    if (!getsockopt (ctx->sock, SOL_SOCKET, SO_ERROR, &opt, (socklen_t *) &len)) {
      if ((ctx->err_no = opt)) {
        ctx->err_no = errno;
        rv = -8;
        goto cleanup;
      }
    }
  }

  ctx->flags |= NA_SOCK_FLAG_CONNECT;
  ctx->flags |= NA_SOCK_FLAG_NEW_CONNECT;

  // ===============
  // ** Connected **
  // ===============

  ctx->lport = lport;

  if (NA_SOCK_FLAG_CONNECT & ctx->flags) {
    // get local port
    len = sizeof (ctx->saddr);
    if (!getsockname (ctx->sock, (struct sockaddr *) &ctx->saddr, (socklen_t *) &len))
      ctx->lport = ntohs (ctx->saddr.sin_port);

    if ((ptr = inet_ntoa (ctx->saddr.sin_addr))) {
      memset (ctx->laddr, 0, sizeof (ctx->laddr));
      strncpy (ctx->laddr, ptr, sizeof (ctx->laddr) - 1);
    }

    ctx->mode = NA_SOCK_MODE_OUT;
  }

  rv = 0;

cleanup:
  SOCK_CLOSE (sock);

  if (0 > rv)
    ctx->flags = 0;

  return rv;
}

int na_sock_send (NA_SOCK *ctx, unsigned char *sbuf, unsigned int slen)
{
  int   rv;

  if (NA_MAGIC != ctx->magic ||
      !ctx->init ||
      !sbuf ||
      !(NA_SOCK_FLAG_CONNECT & ctx->flags) ||
      0 >= ctx->sock)
    return -1;

  // remove the new connect flag
  if (NA_SOCK_FLAG_NEW_CONNECT & ctx->flags)
    ctx->flags &= (~NA_SOCK_FLAG_NEW_CONNECT);

  while (slen &&
         na_proc_token (0, 1)) {
    rv = send (ctx->sock, (char *) sbuf, slen, 0);

    if (0 >= rv || rv > slen) {
      ctx->err_no = errno;

      ctx->flags = 0;
      return -2;
    }

    slen -= rv;
    sbuf += rv;
  }

  return 0;
}

int na_sock_recv (NA_SOCK *ctx, unsigned char *rbuf, unsigned int rsize, unsigned int *rlen, unsigned long timeout)
{
  int             rv,
                  flags;

  fd_set          readfds,
                  excpfds;

  struct timeval  time;

  if (NA_MAGIC != ctx->magic ||
      !ctx->init ||
      !rbuf ||
      0 >= ctx->sock)
    return -1;

  if (rlen)
    *rlen = 0;

  if (!(NA_SOCK_FLAG_CONNECT & ctx->flags ||
        NA_SOCK_FLAG_CONNECTING & ctx->flags)) {
    rv = 0;
    goto cleanup;
  }

  // remove the new connect flag
  if (NA_SOCK_FLAG_NEW_CONNECT & ctx->flags)
    ctx->flags &= (~NA_SOCK_FLAG_NEW_CONNECT);

  if (0xFFFFFFFF != timeout) {
    FD_ZERO (&readfds);
    FD_ZERO (&excpfds);

    FD_SET (ctx->sock, &readfds);
    FD_SET (ctx->sock, &excpfds);

    time.tv_sec = timeout / 1000;
    time.tv_usec = 1000 * (timeout % 1000);

    if (0 > (rv = select (ctx->sock + 1, &readfds, 0, &excpfds, &time)) ||
        FD_ISSET (ctx->sock, &excpfds)) {
      if (EINTR == (ctx->err_no = errno))
        rv = -2;  // catched interrupted signal
      else
        rv = -3;
      goto cleanup;
    }
    else if (!rv || !FD_ISSET (ctx->sock, &readfds)) {
      rv = 0; // no incoming data
      goto cleanup;
    }
  }

  flags = (rlen) ? 0 : MSG_PEEK;

  if (0 < (rv = recv (ctx->sock, (char *) rbuf, rsize, flags))) {
    if (rlen)
      *rlen = rv;
    rv = 0;
  }
  else {
    switch ((ctx->err_no = errno)) {
    case EWOULDBLOCK:
      rv = 0; // no incoming data
      break;

    case EINTR:
      rv = -2; // catched interrupted signal
      break;

    default:
      if (!rv)
        rv = -5;  // disconnected
      else
        rv = -3;  // error
    }
  }

cleanup:
  if (rv)
    ctx->flags = 0;

  return rv;
}

int na_sock_connected (NA_SOCK *ctx)
{
  if (NA_MAGIC == ctx->magic &&
      ctx->init &&
      NA_SOCK_TYPE_TCP == ctx->type) {
    if (NA_SOCK_FLAG_CONNECT & ctx->flags) {
      if (NA_SOCK_FLAG_NEW_CONNECT & ctx->flags)
        return 2;
      return 1;
    }
  }

  return 0;
}

int na_sock_connecting (NA_SOCK *ctx)
{
  if (NA_MAGIC == ctx->magic &&
      ctx->init &&
      NA_SOCK_TYPE_TCP == ctx->type &&
      NA_SOCK_FLAG_CONNECTING & ctx->flags)
    return 1;

  return 0;
}

int na_sock_set_io (NA_SOCK *ctx, int blocking)
{
  int opt;

  if (NA_MAGIC != ctx->magic ||
      !ctx->init ||
      0 >= ctx->sock)
    return -1;

  opt = (blocking) ? 0 : 1;  // set socket to blocking (0) / non-blocking (1) mode
  if (ioctl (ctx->sock, FIONBIO, &opt))
    return -2;

  return 0;
}

int na_sock_fds_init (NA_SOCK_FDS *ctx, int size)
{
  na_sock_fds_term (ctx);

  if (na_array_init (&ctx->fds, size, NA_ARRAY_INDEX, NA_ARRAY_FLAG_NO_LOCK, 0))
    return -1;

  ctx->init = 1;
  return 0;
}

void na_sock_fds_term (NA_SOCK_FDS *ctx)
{
  if (!ctx ||
      (NA_MAGIC == ctx->magic && !ctx->init))
    return;

  if (NA_MAGIC == ctx->magic && ctx->init)
    na_array_term (&ctx->fds);

  memset (ctx, 0, sizeof (NA_SOCK_FDS));
  ctx->magic = NA_MAGIC;
}

int na_sock_fds_proc (NA_SOCK_FDS *ctx)
{
  int             rv,
                  fdcount,
                  fdmax;

  fd_set          readfds,
                  excpfds;

  struct timeval  time;

  NA_ARRAY_ITEM   *item,
                  *last,
                  *next   = 0;

  NA_SOCK_FD      *fd;

  if (NA_MAGIC != ctx->magic ||
      !ctx->init)
    return -1;
  rv = -2;

  if (ctx->fds.count) {
    do {
      FD_ZERO (&readfds);
      FD_ZERO (&excpfds);

      fdcount = 0;
      fdmax = 0;
      item = next;

      while (255 > fdcount && (item = na_array_get_next (&ctx->fds, item))) {
        fd = (NA_SOCK_FD *) item->data;

        if (fd->sock) {
          FD_SET (fd->sock->sock, &readfds);
          FD_SET (fd->sock->sock, &excpfds);

          fd->raised = 0;

          if (fd->sock->sock > fdmax)
            fdmax = fd->sock->sock;
          fdcount++;
        }
      }

      last = item;

      if (fdmax) {
        time.tv_sec = 0;
        time.tv_usec = (next) ? 0 : 1;

        if (0 > (fdcount = select (fdmax + 1, &readfds, 0, &excpfds, &time)))
          return -2;

        if (fdcount) {
          item = next;

          while (fdcount && (item = na_array_get_next (&ctx->fds, item))) {
            fd = (NA_SOCK_FD *) item->data;

            if (FD_ISSET (fd->sock->sock, &readfds) || FD_ISSET (fd->sock->sock, &excpfds)) {
              fd->raised = 1;
              rv = 0;
              fdcount--;
            }
          }
        }

        next = last;
      }
    }
    while (next);
  }

  return rv;
}

int na_sock_fds_add (NA_SOCK_FDS *ctx, NA_SOCK *sock, void *hook)
{
  int         rv;

  char        id [10];

  NA_SOCK_FD  *fd = 0;

  if (NA_MAGIC != ctx->magic ||
      !ctx->init)
    return -1;

  MEM_ALLOC (fd, sizeof (NA_SOCK_FD), (NA_SOCK_FD *));
  if (!fd)
    return -2;

  fd->sock = sock;
  fd->hook = hook;
  sprintf (id, "%x", (int)fd->sock);

  if (!(rv = na_array_add (&ctx->fds, fd, id, 1, 0)))
    fd = 0;
  MEM_CLEAN (fd);

  return rv;
}

int na_sock_fds_del (NA_SOCK_FDS *ctx, NA_SOCK *sock)
{
  int rv;
  char  id [10];

  if (NA_MAGIC != ctx->magic ||
      !ctx->init)
    return -1;

  sprintf (id, "%x", (int)sock);

  if ((rv = na_array_del_by_id (&ctx->fds, id, 0)))
    printf ("id=%s, rv=%d\n", id, rv);

  return rv;
}

