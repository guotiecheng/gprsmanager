/*******************************************************************************
* This file is licensed under the terms of your license agreement(s) with
* GHL covering this file. Redistribution, except as permitted by the terms of
* your license agreement(s) with GHL, is strictly prohibited.
********************************************************************************/

#include "na_network.h"

/* ===============================================================
 * Description: initialize structure
 *
 * Parameter  : [IN  ] ctx - structure context
 * Return     : 0 - success
 *              else - failure
 * ===============================================================
 */
int na_network_init (NA_NETWORK *ctx)
{
  na_network_term (ctx);
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
void na_network_term (NA_NETWORK *ctx)
{
  struct ifreq  ifr;

  if (!ctx ||
      (NA_MAGIC == ctx->magic && !ctx->init))
    return;

  if (NA_MAGIC == ctx->magic && ctx->init) {
    if (0 < ctx->sock_raw) {
      // reset interface promiscous mode back to normal
      if (!ioctl (ctx->sock_raw, SIOCGIFFLAGS, &ifr)) {
        ifr.ifr_flags |= ~IFF_PROMISC;

        ioctl (ctx->sock_raw, SIOCSIFFLAGS, &ifr);
      }
    }

    SOCK_CLOSE (ctx->sock_icmp);
    SOCK_CLOSE (ctx->sock_net);
    SOCK_CLOSE (ctx->sock_raw);
    SOCK_CLOSE (ctx->sock_dns);

    MEM_CLEAN (ctx->buf);
  }

  memset (ctx, 0, sizeof (NA_NETWORK));
  ctx->magic = NA_MAGIC;
}

/* ===============================================================
 * Description: get all interface info
 *
 * Parameter  : [IN  ] ctx - structure context
 *              [IN  ] ifname - retrieve specified interface (optional = 0)
 *              [OUT ] iflist - all the interface store into list
 * Return     : 0 - success
 *              else - failure
 * ===============================================================
 */
int na_network_info_if (NA_NETWORK *ctx, char *ifname, NA_ARRAY *iflist)
{
  int           rv,
                size;

  char          buf [MAX_MODEST_BUFFER_SIZE],
                *frm,
                *end;

  FILE          *fd     = 0;

  NA_NETWORK_IF *ifptr  = 0;

  struct ifreq  *ifr;
  struct ifconf ifc     = { 0 };

  if (NA_MAGIC != ctx->magic ||
      !ctx->init ||
      !iflist ||
      na_array_init (iflist, 32, NA_ARRAY_INDEX, NA_ARRAY_FLAG_CHK_DUP | NA_ARRAY_FLAG_NO_LOCK, 0) ||
      na_network_sock (ctx, NA_NETWORK_PROTO_NET, 0))
    return -1;

  FOPEN (fd, "/proc/net/dev", "r");
  if (!fd) {
    rv = -3;
    goto cleanup;
  }

  fgets (buf, sizeof (buf) - 1, fd);  // eat line
  fgets (buf, sizeof (buf) - 1, fd);

  while (fgets (buf, sizeof (buf) - 1, fd)) {
    if (!ifptr) {
      MEM_ALLOC (ifptr, sizeof (NA_NETWORK_IF), (NA_NETWORK_IF *));
    }

    // get interface name
    for (frm = buf; ' ' == *frm; frm++);  // skip all space
    for (end = frm; *end && ':' != *end; end++);

    if (':' != *end || end - frm >= sizeof (ifptr->name)) {
      rv = -4;
      goto cleanup;
    }
    *end = 0;

    if (!ifname || !strcmp (ifname, frm)) {
      if ((rv = na_network_get_if (ctx, frm, ifptr))) {
        rv = -5;
        goto cleanup;
      }

      if ((rv = na_array_add (iflist, ifptr, ifptr->name, 1, 0))) {
        rv = -6;
        goto cleanup;
      }
      ifptr = 0;
    }
  }

  // cater for those interface using eth0:1 method
  for (size = 30; ; size += 10) {
    ifc.ifc_len = sizeof (struct ifreq) * size;
    MEM_ALLOC (ifc.ifc_buf, ifc.ifc_len, (void *));

    if (0 > ioctl (ctx->sock_net, SIOCGIFCONF, &ifc)) {
      rv = -7;
      goto cleanup;
    }

    if (ifc.ifc_len != sizeof (struct ifreq) * size)
      break;
    MEM_CLEAN (ifc.ifc_buf);
  }

	ifr = ifc.ifc_req;
	for (size = 0; size < ifc.ifc_len; ifr++, size += sizeof (struct ifreq)) {
    if (!ifptr) {
      MEM_ALLOC (ifptr, sizeof (NA_NETWORK_IF), (NA_NETWORK_IF *));
    }
    else {
      memset (ifptr, 0, sizeof (NA_NETWORK_IF));
    }

    if ((rv = na_network_get_if (ctx, ifr->ifr_name, ifptr))) {
      rv = -8;
      goto cleanup;
    }

    if (!ifname || !strcmp (ifname, ifptr->name)) {
      if (!(rv = na_array_add (iflist, ifptr, ifptr->name, 1, 0)))
        ifptr = 0;
    }
  }

  rv = 0;

cleanup:
  MEM_CLEAN (ifc.ifc_buf);
  MEM_CLEAN (ifptr);
  FCLOSE (fd);

  return rv;
}

/* ===============================================================
 * Description: get interface according to routing info matching
 *
 * Parameter  : [IN  ] ctx - structure context
 *              [IN  ] addr - destination ip address
 *              [OUT ] rtret - matched route
 *              [OUT ] ifret - matched interface
 * Return     : 0 - success
 *              else - failure
 * ===============================================================
 */
int na_network_routeif (NA_NETWORK *ctx, char *addr, NA_NETWORK_RT *rtret, NA_NETWORK_IF *ifret)
{
  int             rv,
                  metric    = -1;

  unsigned long   l_dt_addr,
                  l_rt_addr,
                  l_rt_mask;

  struct in_addr  dt_addr,
                  rt_addr,
                  rt_mask;

  NA_ARRAY        rtlist;

  NA_ARRAY_ITEM   *item = 0;

  NA_NETWORK_RT   *rtptr;

  if (NA_MAGIC != ctx->magic ||
      !ctx->init ||
      !ifret ||
      !inet_aton ((!addr || !*addr) ? "0.0.0.0" : addr, &dt_addr))
    return -1;

  memcpy (&l_dt_addr, &dt_addr, sizeof (unsigned long));

  if (na_network_info_rt (ctx, &rtlist))
    rv = -2;
  else {
    rv = -3;

    while ((item = na_array_get_next (&rtlist, item))) {
      rtptr = (NA_NETWORK_RT *) item->data;

      if (inet_aton (rtptr->addr, &rt_addr) &&
          inet_aton (rtptr->mask, &rt_mask)) {
        memcpy (&l_rt_addr, &rt_addr, sizeof (unsigned long));
        memcpy (&l_rt_mask, &rt_mask, sizeof (unsigned long));

        if (l_rt_addr == (l_dt_addr & l_rt_mask) &&
            (-1 == metric || metric > rtptr->metric)) {
          if (!na_network_get_if (ctx, rtptr->name, ifret)) {
            metric = rtptr->metric;
            if (rtret)
              memcpy (rtret, rtptr, sizeof (NA_NETWORK_RT));
            rv = 0;
          }
        }
      }
    }
  }

  na_array_term (&rtlist);

  return rv;
}

/* ===============================================================
 * Description: get all routing info
 *
 * Parameter  : [IN  ] ctx - structure context
 *              [OUT ] rtlist - all the routing info store into list
 * Return     : 0 - success
 *              else - failure
 * ===============================================================
 */
int na_network_info_rt (NA_NETWORK *ctx, NA_ARRAY *rtlist)
{
  int                 rv, ref, use, mtu, win, ir;

  unsigned long       d, g, m;

  FILE                *fd     = 0;

  NA_NETWORK_RT       *rtptr  = 0;

  struct sockaddr_in  addr;
  struct in_addr      mask;

  if (NA_MAGIC != ctx->magic ||
      !ctx->init ||
      !rtlist ||
      na_array_init (rtlist, 64, NA_ARRAY_INDEX, NA_ARRAY_FLAG_CHK_DUP | NA_ARRAY_FLAG_NO_LOCK, 0))
    return -1;

  FOPEN (fd, "/proc/net/route", "r");
  if (!fd)
    return -2;

  if (0 > fscanf (fd, "%*[^\n]\n")) { // eat line
    rv = -3;
    goto cleanup;
  }

  while (1) {
    if (!rtptr) {
      MEM_ALLOC (rtptr, sizeof (NA_NETWORK_RT), (NA_NETWORK_RT *));
    }
    else
      memset (rtptr, 0, sizeof (NA_NETWORK_RT));

    if (11 != (rv = fscanf (fd, "%63s%lx%lx%X%d%d%d%lx%d%d%d\n",
                           rtptr->name, &d, &g, &rtptr->flags, &ref, &use, &rtptr->metric, &m, &mtu, &win, &ir)))
      break;

    if (!(rtptr->flags & RTF_UP)) // skip those interface that are down
      continue;

    memset (&addr, 0, sizeof (struct sockaddr_in));

    // get destination address
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = d;
    strncpy (rtptr->addr, inet_ntoa(addr.sin_addr), sizeof (rtptr->addr) - 1);

    // get gateway
    addr.sin_addr.s_addr = g;
    strncpy (rtptr->gate, inet_ntoa(addr.sin_addr), sizeof (rtptr->gate) - 1);

    // get net mask
    mask.s_addr = m;
    strncpy (rtptr->mask, inet_ntoa(mask), sizeof (rtptr->mask) - 1);

    if ((rv = na_array_add (rtlist, rtptr, 0, 1, 0))) {
      rv = -4;
      goto cleanup;
    }
    rtptr = 0;
  }

  rv = 0;

cleanup:
  MEM_CLEAN (rtptr);
  FCLOSE (fd);

  return rv;
}

/* ===============================================================
 * Description: add new interface
 *
 * Parameter  : [IN  ] ctx - structure context
 *              [OUT ] ifnew - new interface info
 * Return     : 0 - success
 *              else - failure
 * ===============================================================
 */
int na_network_add_if (NA_NETWORK *ctx, NA_NETWORK_IF *ifnew)
{
  if (NA_MAGIC != ctx->magic ||
      !ctx->init ||
      !ifnew ||
      !ifnew->name [0])
    return -1;

  ifnew->flags |= (IFF_UP | IFF_RUNNING);

  return na_network_set_if (ctx, ifnew->name, ifnew);
}

/* ===============================================================
 * Description: delete interface
 *
 * Parameter  : [IN  ] ctx - structure context
 *              [IN  ] ifname - interface name
 * Return     : 0 - success
 *              else - failure
 * ===============================================================
 */
int na_network_del_if (NA_NETWORK *ctx, char *ifname)
{
  struct ifreq        ifr;

  struct sockaddr_in  *addr;

  if (NA_MAGIC != ctx->magic ||
      !ctx->init ||
      !ifname ||
      na_network_sock (ctx, NA_NETWORK_PROTO_NET, 0))
    return -1;

  strncpy (ifr.ifr_name, ifname, IFNAMSIZ);

  // set ip address to 0.0.0.0 act as remove
  addr = (struct sockaddr_in *) &ifr.ifr_addr;
  addr->sin_addr.s_addr = INADDR_ANY;
  addr->sin_family = AF_INET;

  if (0 > ioctl (ctx->sock_net, SIOCSIFADDR, &ifr))
    return -2;

  return 0;
}

/* ===============================================================
 * Description: get single interface info
 *
 * Parameter  : [IN  ] ctx - structure context
 *              [IN  ] ifname - interface name
 *              [OUT ] ifret - retrieved interface info
 * Return     : 0 - success
 *              else - failure
 * ===============================================================
 */
int na_network_get_if (NA_NETWORK *ctx, char *ifname, NA_NETWORK_IF *ifret)
{
  struct ifreq    ifr;

  if (NA_MAGIC != ctx->magic ||
      !ctx->init ||
      !ifname ||
      !ifret ||
      na_network_sock (ctx, NA_NETWORK_PROTO_NET, 0))
    return -1;

  memset (ifret, 0, sizeof (NA_NETWORK_IF));
  memset (&ifr, 0, sizeof (struct ifreq));

  strncpy (ifr.ifr_name, ifname, IFNAMSIZ);

  // get interface index
  if (ioctl (ctx->sock_net, SIOCGIFINDEX, &ifr))
    return -2;
  ifret->index = ifr.ifr_ifindex;

  // get interface flags
  if (ioctl (ctx->sock_net, SIOCGIFFLAGS, &ifr))
    return -2;
  ifret->flags = ifr.ifr_flags;

  // get interface name
  strncpy (ifret->name, ifname, sizeof (ifret->name) - 1);

  // get mac address
  if (!ioctl (ctx->sock_net, SIOCGIFHWADDR, &ifr))
    memcpy (ifret->mac, ifr.ifr_ifru.ifru_hwaddr.sa_data, sizeof (ifret->mac));

  // get ip address
  if (!ioctl (ctx->sock_net, SIOCGIFADDR, &ifr))
    strncpy (ifret->addr, inet_ntoa(((struct sockaddr_in *) &ifr.ifr_addr)->sin_addr), sizeof (ifret->addr) - 1);
  else
    strcpy (ifret->addr, "0.0.0.0");

  // get net mask
  if (!ioctl (ctx->sock_net, SIOCGIFNETMASK, &ifr))
    strncpy (ifret->mask, inet_ntoa(((struct sockaddr_in *) &ifr.ifr_netmask)->sin_addr), sizeof (ifret->mask) - 1);
  else
    strcpy (ifret->mask, "0.0.0.0");

  return 0;
}

/* ===============================================================
 * Description: set single interface info
 *
 * Parameter  : [IN  ] ctx - structure context
 *              [IN  ] ifname - interface name
 *              [IN  ] ifnew - new interface info
 * Return     : 0 - success
 *              else - failure
 * ===============================================================
 */
int na_network_set_if (NA_NETWORK *ctx, char *ifname, NA_NETWORK_IF *ifnew)
{
  struct ifreq        ifr;

  NA_NETWORK_IF       ifcur;

  struct sockaddr_in  *addr;

  if (NA_MAGIC != ctx->magic ||
      !ctx->init ||
      !ifname ||
      !ifnew ||
      na_network_sock (ctx, NA_NETWORK_PROTO_NET, 0) ||
      na_network_get_if (ctx, ifname, &ifcur))
    return -1;

  strcpy (ifr.ifr_name, ifname);

  // set mac address
  ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;
  memcpy (&ifr.ifr_hwaddr.sa_data, ifnew->mac, sizeof (ifnew->mac));

  ioctl (ctx->sock_net, SIOCSIFHWADDR, &ifr);

  // set ip address
  addr = (struct sockaddr_in *) &ifr.ifr_addr;
  addr->sin_addr.s_addr = inet_addr (ifnew->addr);
  addr->sin_family = AF_INET;

  if (0 > ioctl (ctx->sock_net, SIOCSIFADDR, &ifr))
    return -3;

  // set net mask
  addr = (struct sockaddr_in *) &ifr.ifr_netmask;
  addr->sin_addr.s_addr = inet_addr (ifnew->mask);
  addr->sin_family = AF_INET;

  if (0 > ioctl (ctx->sock_net, SIOCSIFNETMASK, &ifr))
    return -4;

  // set flags
  ifr.ifr_flags = ifnew->flags;

  if (0 > ioctl (ctx->sock_net, SIOCSIFFLAGS, &ifr))
    return -5;

  // set interface name
  if (strcmp (ifname, ifnew->name)) {
    strcpy (ifr.ifr_name, ifname);
    strcpy (ifr.ifr_newname, ifnew->name);

    if (0 > ioctl (ctx->sock_net, SIOCSIFNAME, &ifr))
      return -6;
  }

  return 0;
}

/* ===============================================================
 * Description: add new routing entry
 *
 * Parameter  : [IN  ] ctx - structure context
 *              [IN  ] rtnew - new routing info
 * Return     : 0 - success
 *              else - failure
 * ===============================================================
 */
int na_network_add_rt (NA_NETWORK *ctx, NA_NETWORK_RT *rtnew)
{
  return na_network_set_rt (ctx, rtnew, NA_NETWORK_OP_ADD_RT);
}

/* ===============================================================
 * Description: delete routing entry
 *
 * Parameter  : [IN  ] ctx - structure context
 *              [IN  ] rtdel - routing info that need to delete
 * Return     : 0 - success
 *              else - failure
 * ===============================================================
 */
int na_network_del_rt (NA_NETWORK *ctx, NA_NETWORK_RT *rtdel)
{
  return na_network_set_rt (ctx, rtdel, NA_NETWORK_OP_DEL_RT);
}

/* ===============================================================
 * Description: helper funtion that add or delete single routing entry
 *
 * Parameter  : [IN  ] ctx - structure context
 *              [IN  ] rtset - routing info that need to set
 *              [IN  ] op - operation type, it can be as below:
 *                         NA_NETWORK_OP_ADD_RT - add new routing entry
 *                         NA_NETWORK_OP_DEL_RT - delete routing entry
 * Return     : 0 - success
 *              else - failure
 * ===============================================================
 */
int na_network_set_rt (NA_NETWORK *ctx, NA_NETWORK_RT *rtset, int op)
{
  int                 rv      = -1,
                      ref, use, mtu, win, ir;

  unsigned long       d, g, m;

  FILE                *fd     = 0;

  struct rtentry      rt      = { 0 };

  struct sockaddr_in  *addr;

  NA_NETWORK_RT       rtnet;

  if (NA_MAGIC != ctx->magic ||
      !ctx->init ||
      !rtset ||
      0 > rtset->metric ||
      na_network_sock (ctx, NA_NETWORK_PROTO_NET, 0))
    return -1;

  rtset->flags |= RTF_UP;

  if (rtset->gate [0] && strcmp (rtset->gate, "0.0.0.0"))
    rtset->flags |= RTF_GATEWAY;

  // check current route to decide wheather to proceed or not
  if (!(FOPEN (fd, "/proc/net/route", "r"))) {
    rv = -2;
    goto cleanup;
  }

  if (0 <= fscanf (fd, "%*[^\n]\n")) { // eat line
    while (1) {
      if (11 != fscanf (fd, "%63s%lx%lx%X%d%d%d%lx%d%d%d\n",
                        rtnet.name, &d, &g, &rtnet.flags, &ref, &use, &rtnet.metric, &m, &mtu, &win, &ir)) {
        break;
      }

      if (!(rtnet.flags & RTF_UP)) // skip those interface that are down
        continue;

      na_addr_ntoa (d, rtnet.addr);
      na_addr_ntoa (g, rtnet.gate);
      na_addr_ntoa (m, rtnet.mask);

      if (!strcmp (rtnet.addr, rtset->addr) &&
          !strcmp (rtnet.gate, rtset->gate) &&
          !strcmp (rtnet.mask, rtset->mask) &&
          !strcmp (rtnet.name, rtset->name)) {
        if (rtnet.flags == rtset->flags &&
            rtnet.metric == rtset->metric)
          rv = 0;
        else
          rv = 1;

        break;
      }
    }
  }

  FCLOSE (fd);

  if ((!rv && NA_NETWORK_OP_ADD_RT == op) ||
      (rv && NA_NETWORK_OP_DEL_RT == op)) {
    rv = 0;
    goto cleanup;
  }

  addr = (struct sockaddr_in *) &rt.rt_dst;
  addr->sin_addr.s_addr = inet_addr (rtset->addr);
  addr->sin_family = AF_INET;

  addr = (struct sockaddr_in *) &rt.rt_gateway;
  addr->sin_addr.s_addr = inet_addr (rtset->gate);
  addr->sin_family = AF_INET;

  addr = (struct sockaddr_in *) &rt.rt_genmask;
  addr->sin_addr.s_addr = inet_addr (rtset->mask);
  addr->sin_family = AF_INET;

  rt.rt_dev = rtset->name;

  // we need to change the route, so we delete it first then add back
  if (1 == rv) {
    rt.rt_flags = rtnet.flags;
    rt.rt_metric = rtnet.metric + 1;

    ioctl (ctx->sock_net, SIOCDELRT, &rt);
  }

  rt.rt_flags = rtset->flags;
  rt.rt_metric = rtset->metric + 1;

  if (ioctl (ctx->sock_net, (NA_NETWORK_OP_ADD_RT == op) ? SIOCADDRT : SIOCDELRT, &rt)) {
    rv = -4;
    goto cleanup;
  }

  rv = 0;

cleanup:

  return rv;
}

int na_network_sock (NA_NETWORK *ctx, int proto, char *ifname)
{
  int                 opt,
                      sock  = -1,
                      ifindex;

  struct sockaddr_ll  src;

  struct ifreq        ifr;

  struct protoent     *p;

  if (NA_MAGIC != ctx->magic ||
      !ctx->init)
    return -1;

  switch (proto) {
  case NA_NETWORK_PROTO_RAW:
    if (0 >= ctx->sock_raw) {
      SOCK_ALLOC (ctx->sock_raw, AF_PACKET, SOCK_RAW, htons (ETH_P_ALL));

      if (ctx->sock_raw &&
          ifname && *ifname) {
        memset (&ifr, 0, sizeof (struct ifreq));
        strncpy (ifr.ifr_name, ifname, IFNAMSIZ);

        // get interface index
        if (ioctl (ctx->sock_raw, SIOCGIFINDEX, &ifr)) {
          SOCK_CLOSE (ctx->sock_raw);
          break;
        }
        ifindex = ifr.ifr_ifindex;

        // get interface to promiscous mode
        if (ioctl (ctx->sock_raw, SIOCGIFFLAGS, &ifr)) {
          SOCK_CLOSE (ctx->sock_raw);
          break;
        }
        else {
          ifr.ifr_flags |= IFF_PROMISC;

          if (ioctl (ctx->sock_raw, SIOCSIFFLAGS, &ifr)) {
            SOCK_CLOSE (ctx->sock_raw);
            break;
          }
        }

        // bind to specified network interface
        memset (&src, 0, sizeof (struct sockaddr_ll));
        src.sll_family = AF_PACKET;
        src.sll_protocol = htons (ETH_P_ALL);
        src.sll_ifindex = ifindex;

        if (0 > bind (ctx->sock_raw, (struct sockaddr *) &src, sizeof (struct sockaddr_ll))) {
          SOCK_CLOSE (ctx->sock_raw);
          break;
        }
        ctx->rawif = ifindex;
      }
    }
    sock = ctx->sock_raw;
    break;

  case NA_NETWORK_PROTO_ICMP:
    if (0 >= ctx->sock_icmp) {
      if (!(p = getprotobyname("icmp")))
        return -2;

      SOCK_ALLOC (ctx->sock_icmp, AF_INET, SOCK_RAW, p->p_proto);

      if (0 < ctx->sock_icmp) {
        opt = 1;
        if (setsockopt (ctx->sock_icmp, SOL_SOCKET, SO_BROADCAST, &opt, sizeof (opt)))
          return -3;
      }
    }
    sock = ctx->sock_icmp;
    break;

  case NA_NETWORK_PROTO_NET:
    if (0 >= ctx->sock_net) {
      SOCK_ALLOC (ctx->sock_net, AF_INET, SOCK_STREAM, 0);
    }
    sock = ctx->sock_net;
    break;

  case NA_NETWORK_PROTO_DNS:
    if (0 >= ctx->sock_dns) {
      SOCK_ALLOC (ctx->sock_dns, AF_INET, SOCK_DGRAM, 0);
    }
    sock = ctx->sock_dns;
  }

  return (0 >= sock) ? -4 : 0;
}

int na_network_raw_if (NA_NETWORK *ctx, char *ifname)
{
  if (na_network_sock (ctx, NA_NETWORK_PROTO_RAW, ifname))
    return -1;

  return 0;
}

int na_network_filter (NA_NETWORK *ctx, NA_NETWORK_FILTER *filter_code, unsigned int filter_size)
{
  struct sock_fprog filter;

  if (NA_MAGIC != ctx->magic ||
      !ctx->init ||
      0 >= ctx->sock_raw)
    return -1;

  // the filter code can be generate by tcpdump command (e.g: tcpdump ether proto 0x3828 -s 0 -dd)
  // attach the bpf filter code
  filter.len = filter_size;
  filter.filter = (struct sock_filter *) filter_code;

  if (setsockopt (ctx->sock_raw, SOL_SOCKET, SO_ATTACH_FILTER, &filter, sizeof (filter)))
    return -2;

  return 0;
}

int na_network_send (NA_NETWORK *ctx, unsigned char *sbuf, unsigned int slen)
{
  struct sockaddr_ll  dst;

  struct timeval      time;

  if (NA_MAGIC != ctx->magic ||
      !ctx->init ||
      0 >= ctx->sock_raw)
    return -1;

  if (!sbuf || !slen)
    return 0;

  memset (&dst, 0, sizeof (struct sockaddr_ll));

  dst.sll_family = AF_PACKET;
  dst.sll_protocol = htons (ETH_P_ALL);
  dst.sll_ifindex = ctx->rawif;

  time.tv_sec = 1;
  time.tv_usec = 0;
  setsockopt (ctx->sock_raw, SOL_SOCKET, SO_SNDTIMEO, &time, sizeof (time));

  // send into network interface
  if (0 >= sendto (ctx->sock_raw, sbuf, slen, 0, (struct sockaddr *) &dst, sizeof (struct sockaddr_ll)))
    return -2;

  return 0;
}

int na_network_recv (NA_NETWORK *ctx, unsigned char *rbuf, unsigned int rsize, unsigned int *rlen)
{
  int             len;

  struct timeval  time;

  if (NA_MAGIC != ctx->magic ||
      !ctx->init ||
      0 >= ctx->sock_raw ||
      !rbuf || !rsize || !rlen)
    return -1;

  time.tv_sec = 0;
  time.tv_usec = 1;
  setsockopt (ctx->sock_raw, SOL_SOCKET, SO_RCVTIMEO, &time, sizeof (time));

  if (0 > (len = recvfrom (ctx->sock_raw, rbuf, rsize, 0, 0, 0)))
    len = 0;
  *rlen = len;

  return 0;
}

int na_network_icmp_req (NA_NETWORK *ctx, char *addr, unsigned long id, unsigned short seq)
{
  int                 rv;

  unsigned int        i,
                      pid,
                      blen;

  unsigned long       now;

  unsigned char       *bptr,
                      *time,
                      *orig,
                      *data;

  struct sockaddr_in  dst;

  struct icmp         *icmp;

  struct hostent      *host;

  if (NA_MAGIC != ctx->magic ||
      !ctx->init)
    return -1;

  memset (&dst, 0, sizeof (struct sockaddr_in));

  // resolve domain name into ip address
  if ((host = gethostbyname (addr))) {
    dst.sin_family = host->h_addrtype;

    if (host->h_length > (int) sizeof (dst.sin_addr))
      host->h_length = sizeof (dst.sin_addr);
    memcpy (&dst.sin_addr, host->h_addr, host->h_length);
  }
  else {
    // convert dot value into ip address
    if (!inet_aton (addr, &dst.sin_addr))
      return -2;
  }

  if (!ctx->buf) {
    MEM_ALLOC (ctx->buf, NA_NETWORK_FRAME_MAX_SIZE, (unsigned char *));
    if (!ctx->buf)
      return -3;
    ctx->len = NA_NETWORK_FRAME_MAX_SIZE;
  }

  icmp = (struct icmp *) ctx->buf;
  time = icmp->icmp_data;
  orig = time + sizeof (unsigned long);
  data = orig + sizeof (unsigned long);

  pid = (na_proc_id () & 0xFFFF);

  icmp->icmp_type = ICMP_ECHO;
  icmp->icmp_code = 0;
  icmp->icmp_cksum = 0;
  icmp->icmp_seq = seq;
  icmp->icmp_id = pid;

  // user-defined data
  now = na_time_now ();
  memcpy (time, &now, sizeof (unsigned long));
  memcpy (orig, &id, sizeof (unsigned long));

  icmp->icmp_cksum = na_crc (ctx->buf, data - ctx->buf);

  if (na_network_sock (ctx, NA_NETWORK_PROTO_ICMP, 0))
    return -4;

  i = 1;
  if (setsockopt (ctx->sock_icmp, SOL_SOCKET, SO_BROADCAST, &i, sizeof (i)))
    return -5;

  bptr = ctx->buf;
  blen = NA_NETWORK_ICMP_MIN_LEN + (data - time);

  while (blen) {
    if (0 >= (rv = sendto (ctx->sock_icmp, bptr, blen, 0, (struct sockaddr *) &dst, sizeof (struct sockaddr))))
      return -6;
    bptr += rv;
    blen -= rv;
  }

  return 0;
}

int na_network_icmp_res (NA_NETWORK *ctx, unsigned long *id, unsigned short *seq, unsigned long *latency)
{
  int                 rv;

  unsigned short      crc;

  unsigned int        i,
                      pid,
                      blen,
                      hlen;

  unsigned long       now;

  unsigned char       *bptr,
                      *time,
                      *orig,
                      *data;

  struct timeval      tv;

  struct sockaddr_in  src;

  struct icmp         *icmp;

  struct ip           *ip;

  if (NA_MAGIC != ctx->magic ||
      !ctx->init)
    return -1;

  if (!ctx->buf) {
    MEM_ALLOC (ctx->buf, NA_NETWORK_FRAME_MAX_SIZE, (unsigned char *));
    if (!ctx->buf)
      return -2;
    ctx->len = NA_NETWORK_FRAME_MAX_SIZE;
  }

  tv.tv_sec = 0;
  tv.tv_usec = 1;
  if (setsockopt (ctx->sock_icmp, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof (tv)))
    return -3;

  bptr = ctx->buf;
  blen = ctx->len;

  i = sizeof (src);
  if (0 < (rv = recvfrom (ctx->sock_icmp, bptr, blen, 0, (struct sockaddr*) &src, (socklen_t *) &i)) &&
      rv >= NA_NETWORK_ICMP_MIN_LEN + NA_NETWORK_ICMP_DATA_SIZE) {
    ip = (struct ip *) bptr;
  	hlen = ip->ip_hl << 2;

    if (hlen + NA_NETWORK_ICMP_DATA_SIZE <= rv) {
      pid = (na_proc_id () & 0xFFFF);

      icmp = (struct icmp *) (bptr + hlen);
      time = icmp->icmp_data;
      orig = time + sizeof (unsigned long);
      data = orig + sizeof (unsigned long);

      if (icmp->icmp_id == pid &&
          ICMP_ECHOREPLY == icmp->icmp_type) {
        crc = icmp->icmp_cksum;
        icmp->icmp_cksum = 0; // clear this variable before calculate crc

        if (crc == na_crc (bptr + hlen, data - (bptr + hlen))) {
          memcpy (&now, time, sizeof (unsigned long));

          if (id)
            memcpy (id, orig, sizeof (unsigned long));

          if (seq)
            *seq = icmp->icmp_seq;

          if (latency)
            *latency = na_time_elapsed (now);

          return 0;
        }
        else
          return 1;
      }
    }
  }

  return 2;
}

int na_network_ping (NA_NETWORK *ctx, char *addr, unsigned long timeout, unsigned long *latency)
{
  int rv;
  unsigned long start;

  if (na_network_icmp_req (ctx, addr, 0, 0))
    return -1;

  start = na_time_now ();

  while (na_proc_token (0, 1)) {
    if (!(rv = na_network_icmp_res (ctx, 0, 0, latency)))
      return 0;
    else if (0 > rv)
      return -2;

    if (timeout &&
        timeout < na_time_elapsed (start))
      return -3;
  }

  return -4;
}

int na_network_resolve (NA_NETWORK *ctx, char *name, char *addr, unsigned int size)
{
  struct sockaddr_in  s_addr;

  struct hostent      *host;

  if (NA_MAGIC != ctx->magic ||
      !ctx->init ||
      !name)
    return -1;

  // convert dot value into ip address
  if (!inet_aton (name, &s_addr.sin_addr)) {
    // resolve domain name into ip address
    if ((host = gethostbyname (name))) {
      if (host->h_length > (int) sizeof (s_addr.sin_addr))
        host->h_length = sizeof (s_addr.sin_addr);
      memcpy (&s_addr.sin_addr, host->h_addr, host->h_length);
    }
    else
      return -2;
  }

  if (addr && size) {
    memset (addr, 0, size);
    strncpy (addr, inet_ntoa (s_addr.sin_addr), size - 1);
  }

  return 0;
}

