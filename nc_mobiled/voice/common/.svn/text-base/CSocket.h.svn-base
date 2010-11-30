#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>

#include "GMaster.h"

#if defined (WIN32)

	#pragma comment(lib, "wsock32.lib")
	#include <winsock.h>

#elif defined (LINUX) || defined (UCLINUX)

	#include <pthread.h>

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

#endif

// Socket mode
#define CSOCKET_MODE_NONE				0x00
#define CSOCKET_MODE_CONNECTION_IN		0x01
#define CSOCKET_MODE_CONNECTION_OUT		0x02
#define CSOCKET_MODE_LISTENER			0x03

// Socket type
#define CSOCKET_TYPE_TCP				0x01
#define CSOCKET_TYPE_UDP				0x02
#define CSOCKET_TYPE_UNIX				0x03

typedef struct CSOCKET_T
{
	int						iSendBufSize,
							iRecvBufSize,
							iErrNo,
							iSocket,
							iLocPort,
							iRemPort;

	unsigned char			ucIsConnected,
							ucMode,
							ucType;

	char					acLocIP [128],
							acRemIP [128];
	
	struct sockaddr_in		stTCP_SocketAddr;

	#if defined (LINUX) || defined (UCLINUX)

		struct sockaddr_un	stUnix_SocketAddr;

	#endif
}
CSOCKET;

void			sock_startup ();
void			sock_cleanup ();

void			sock_init (CSOCKET *p_pstSock, unsigned char p_ucType, int p_iSendBufSize, int p_iRecvBufSize);
unsigned char	sock_term (CSOCKET *p_pstSock);

unsigned char	sock_listen (CSOCKET *p_pstSock, char *p_pcLocIP, int p_iLocPort, int p_iQueueSize);
unsigned char	sock_accept (CSOCKET *p_pstSock, CSOCKET *p_pstNewSock, unsigned long p_ulTimeout, int p_iKeepAliveInterval, int p_iKeepAliveRetry);
unsigned char	sock_connect (CSOCKET *p_pstSock, char *p_pcRemIP, int p_iLocPort, int p_iRemPort, unsigned long p_ulTimeout, int p_iKeepAliveInterval, int p_iKeepAliveRetry);
unsigned char	sock_close (CSOCKET *p_pstSock);

unsigned char	sock_send (CSOCKET *p_pstSock, unsigned char *p_pucData, int p_iLen);
unsigned char	sock_recv (CSOCKET *p_pstSock, unsigned char *p_pucData, int p_iSize, int *p_piLen, unsigned long p_ulTimeout);

#endif
