#ifndef NA_NETWORK_H
#define NA_NETWORK_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>

#include <linux/if_ether.h>
#include <linux/sockios.h>
#include <linux/types.h>
#include <linux/filter.h>

#include <arpa/inet.h>
#include <arpa/nameser.h>
#include <netpacket/packet.h>
#include <netdb.h>
#include <resolv.h>

#include <net/if.h>
#include <net/if_arp.h>
#include <net/route.h>
#include <net/ethernet.h>

#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <netinet/ip.h>

#include "na_common.h"
#include "na_array.h"
#include "na_proc.h"

#define NA_NETWORK_PROTO_RAW        1
#define NA_NETWORK_PROTO_ICMP       2
#define NA_NETWORK_PROTO_NET        3
#define NA_NETWORK_PROTO_DNS        4

#define NA_NETWORK_OP_ADD_RT        1
#define NA_NETWORK_OP_DEL_RT        2

#define NA_NETWORK_FRAME_MAX_SIZE   1600
#define NA_NETWORK_ICMP_DATA_SIZE   8
#define NA_NETWORK_ICMP_MIN_LEN     8

typedef struct NA_NETWORK_FILTER_T
{
  unsigned short  code;
  unsigned char   jump_true;
  unsigned char   jump_false;
  unsigned int    generic;
}
NA_NETWORK_FILTER;

typedef struct NA_NETWORK_IF_T
{
  int             index,
                  flags;

  unsigned char mac [6];

  char            name [64],
                  addr [16],
                  mask [16];
}
NA_NETWORK_IF;

typedef struct NA_NETWORK_RT_T
{
  int             metric,
                  flags;

  char            name [64],
                  addr [16],
                  gate [16],
                  mask [16];
}
NA_NETWORK_RT;

typedef struct NA_NETWORK_T
{
  int             magic,
                  init,

                  rawif,

                  sock_net,
                  sock_raw,
                  sock_icmp,
                  sock_dns;

  unsigned char   *buf;

  unsigned int    len;
}
NA_NETWORK;


int   na_network_init     (NA_NETWORK *ctx);
void  na_network_term     (NA_NETWORK *ctx);

int   na_network_raw_if   (NA_NETWORK *ctx, char *ifname);
int   na_network_send     (NA_NETWORK *ctx, unsigned char *sbuf, unsigned int slen);
int   na_network_recv     (NA_NETWORK *ctx, unsigned char *rbuf, unsigned int rsize, unsigned int *rlen);

int   na_network_ping     (NA_NETWORK *ctx, char *addr, unsigned long timeout, unsigned long *latency);
int   na_network_icmp_req (NA_NETWORK *ctx, char *addr, unsigned long id, unsigned short seq);
int   na_network_icmp_res (NA_NETWORK *ctx, unsigned long *id, unsigned short *seq, unsigned long *latency);

int   na_network_resolve  (NA_NETWORK *ctx, char *name, char *addr, unsigned int size);

int   na_network_info_if  (NA_NETWORK *ctx, char *ifname, NA_ARRAY *iflist);
int   na_network_info_rt  (NA_NETWORK *ctx, NA_ARRAY *rtlist);

int   na_network_routeif  (NA_NETWORK *ctx, char *addr, NA_NETWORK_RT *rtret, NA_NETWORK_IF *ifret);

int   na_network_add_if   (NA_NETWORK *ctx, NA_NETWORK_IF *ifnew);
int   na_network_del_if   (NA_NETWORK *ctx, char *ifname);
int   na_network_get_if   (NA_NETWORK *ctx, char *ifname, NA_NETWORK_IF *ifret);
int   na_network_set_if   (NA_NETWORK *ctx, char *ifname, NA_NETWORK_IF *ifnew);

int   na_network_add_rt   (NA_NETWORK *ctx, NA_NETWORK_RT *rtnew);
int   na_network_del_rt   (NA_NETWORK *ctx, NA_NETWORK_RT *rtdel);
int   na_network_set_rt   (NA_NETWORK *ctx, NA_NETWORK_RT *rtset, int op);

int   na_network_sock     (NA_NETWORK *ctx, int proto, char *ifname);

int   na_network_filter   (NA_NETWORK *ctx, NA_NETWORK_FILTER *filter_code, unsigned int filter_size);

#ifdef __cplusplus
}
#endif

#endif
