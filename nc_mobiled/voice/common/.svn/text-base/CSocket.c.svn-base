#include "CSocket.h"

void sock_startup ()
{
	#if defined (WIN32)

		WSADATA		stWSAData;
		
		WSAStartup (MAKEWORD (2, 0), &stWSAData);

	#endif
}

void sock_cleanup ()
{
	#if defined (WIN32)

		WSACleanup ();
	
	#endif
}

void sock_init (CSOCKET *p_pstSock, unsigned char p_ucType, int p_iSendBufSize, int p_iRecvBufSize)
{
	p_pstSock->ucIsConnected = 0x00;
	p_pstSock->ucMode = CSOCKET_MODE_NONE;
	p_pstSock->ucType = p_ucType;

	p_pstSock->iSocket = -1;

	if (0 >= p_iSendBufSize)
	{
		p_pstSock->iSendBufSize = 0;
	}
	else
	{
		p_pstSock->iSendBufSize = p_iSendBufSize;
	}

	if (0 < p_iRecvBufSize)
	{
		p_pstSock->iRecvBufSize = 0;
	}
	else
	{
		p_pstSock->iRecvBufSize = p_iRecvBufSize;
	}

	p_pstSock->iErrNo = 0;
	p_pstSock->iLocPort = 0;
	p_pstSock->iRemPort = 0;

	memset (p_pstSock->acLocIP, 0x00, sizeof (p_pstSock->acLocIP));
	memset (p_pstSock->acRemIP, 0x00, sizeof (p_pstSock->acRemIP));
}

unsigned char sock_term (CSOCKET *p_pstSock)
{
	return sock_close (p_pstSock);
}

unsigned char sock_close (CSOCKET *p_pstSock)
{
	unsigned char	ucRet;

	struct linger	stLig;

	int				iRet;

	if (0 < p_pstSock->iSocket)
	{
		if (CSOCKET_TYPE_TCP == p_pstSock->ucType)
		{
			stLig.l_onoff = -1;
			stLig.l_linger = 0;

			// To prevent connection stated as TIME_WAIT
			setsockopt (p_pstSock->iSocket, SOL_SOCKET, SO_LINGER, (char *) &stLig, sizeof (struct linger));

			if (CSOCKET_MODE_CONNECTION_IN == p_pstSock->ucMode || CSOCKET_MODE_CONNECTION_OUT == p_pstSock->ucMode)
			{
				// Stop any data transmission
				#if defined (WIN32)

					shutdown (p_pstSock->iSocket, 0x01 /*SD_SEND*/);

				#elif defined (LINUX) || defined (UCLINUX)

					shutdown (p_pstSock->iSocket, SHUT_RDWR);

				#endif
			}
		}

		SCK_CLOSE_EX (iRet, p_pstSock->iSocket);

		if (0 != iRet)
		{
			#if defined (WIN32)

				p_pstSock->iErrNo = WSAGetLastError ();

			#elif defined (LINUX) || defined (UCLINUX)

				p_pstSock->iErrNo = errno;

			#endif

			ucRet = 0x01;
			goto CleanUp;
		}
	
		p_pstSock->ucIsConnected = 0x00;	// Flag to disconnected

		p_pstSock->iSocket = -1;
		p_pstSock->iLocPort = 0;
		p_pstSock->iRemPort = 0;
		p_pstSock->ucMode = CSOCKET_MODE_NONE;

		memset (p_pstSock->acLocIP, 0x00, sizeof (p_pstSock->acLocIP));
		memset (p_pstSock->acRemIP, 0x00, sizeof (p_pstSock->acRemIP));
	}

	ucRet = 0x00;

CleanUp:

	return ucRet;
}

unsigned char sock_listen (CSOCKET *p_pstSock, char *p_pcLocIP, int p_iLocPort, int p_iQueueSize)
{
	unsigned char		ucRet;

	int					iSocket		= -1,
						iOpt,
						iLength;

	struct hostent		*pstHost;

	#if defined (WIN32)

		unsigned long	ulIP;

	#endif

	if (0 >= p_iLocPort || 0 >= p_iQueueSize)
	{
		ucRet = 0x02;
		goto CleanUp;
	}

	p_pstSock->iErrNo = 0;

	// Create socket
	SCK_ALLOC (iSocket, AF_INET, SOCK_STREAM, 0);

	if (0 >= iSocket)
	{
		p_pstSock->iErrNo = errno;

		ucRet = 0x03;	// Failed to create socket
		goto CleanUp;
	}

	// Close socket if any opened
	sock_close (p_pstSock);

	// Create socket
	p_pstSock->iSocket = iSocket;
	iSocket = -1;

	if (0 != *p_pcLocIP)
	{
		pstHost = gethostbyname (p_pcLocIP);	// Resolve computer name into IP addr

		if (0 != pstHost)
		{
			p_pstSock->stTCP_SocketAddr.sin_family = pstHost->h_addrtype;

			if (pstHost->h_length > (int) sizeof (p_pstSock->stTCP_SocketAddr.sin_addr))
			{
				pstHost->h_length = sizeof (p_pstSock->stTCP_SocketAddr.sin_addr);
			}

			memcpy (&p_pstSock->stTCP_SocketAddr.sin_addr, pstHost->h_addr, pstHost->h_length);
		}
		else
		{
			p_pstSock->stTCP_SocketAddr.sin_family = AF_INET;

			#if defined (WIN32)

				ulIP = inet_addr (p_pcLocIP);	// Convert IP addr (X.X.X.X) to numerical value

				if (INADDR_NONE == ulIP)
				{
					ucRet = 0x02;
					goto CleanUp;
				}

				p_pstSock->stTCP_SocketAddr.sin_addr.S_un.S_addr = htonl (ulIP);
					
			#elif defined (LINUX) || defined (UCLINUX)

				if (!inet_aton (p_pcLocIP, &p_pstSock->stTCP_SocketAddr.sin_addr))
				{
					ucRet = 0x02;
					goto CleanUp;
				}

			#endif
		}
	}
	else
	{
		p_pstSock->stTCP_SocketAddr.sin_family = AF_INET;

		#if defined (WIN32)

			p_pstSock->stTCP_SocketAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

		#elif defined (LINUX) || defined (UCLINUX)

			p_pstSock->stTCP_SocketAddr.sin_addr.s_addr = htonl(INADDR_ANY);

		#endif
	}

	p_pstSock->stTCP_SocketAddr.sin_port = htons ((u_short) p_iLocPort);
	
	memset (&p_pstSock->stTCP_SocketAddr.sin_zero, 0x00, sizeof (p_pstSock->stTCP_SocketAddr.sin_zero));
	
	// Set the reuse local address flag
	iLength = sizeof (iOpt);
	iOpt = 1;
	
	#if defined (WIN32)

		if (0 != setsockopt (p_pstSock->iSocket, SOL_SOCKET, SO_REUSEADDR, (char *) &iOpt, iLength))
		{
			p_pstSock->iErrNo = errno;

			ucRet = 0x04;	// Failed to set options
			goto CleanUp;
		}

	#elif defined (LINUX) || defined (UCLINUX)

		if (0 != setsockopt (p_pstSock->iSocket, SOL_SOCKET, SO_REUSEADDR, (void *) &iOpt, iLength))
		{
			p_pstSock->iErrNo = errno;

			ucRet = 0x04;	// Failed to set options
			goto CleanUp;
		}

	#endif

	iLength = sizeof (p_pstSock->stTCP_SocketAddr);

	if (0 != bind (p_pstSock->iSocket, (struct sockaddr *) &(p_pstSock->stTCP_SocketAddr), iLength))
	{
		p_pstSock->iErrNo = errno;

		ucRet = 0x05;	// Failed to bind local port
		goto CleanUp;
	}

	if (0 != listen (p_pstSock->iSocket, p_iQueueSize))
	{
		p_pstSock->iErrNo = errno;

		ucRet = 0x06;	// Failed to listen on local port
		goto CleanUp;
	}

	p_pstSock->iLocPort = p_iLocPort;

	strncpy (p_pstSock->acLocIP, inet_ntoa (p_pstSock->stTCP_SocketAddr.sin_addr), sizeof (p_pstSock->acLocIP) - 1);

	p_pstSock->ucIsConnected = 0x01;
	p_pstSock->ucMode = CSOCKET_MODE_LISTENER;

	ucRet = 0x00;

CleanUp:
	SCK_CLOSE (iSocket);

	if (0x00 != ucRet)
	{
		SCK_CLOSE (p_pstSock->iSocket);
	}

	return ucRet;
}

unsigned char sock_accept (CSOCKET *p_pstSock, CSOCKET *p_pstNewSock, unsigned long p_ulTimeout, int p_iKeepAliveInterval, int p_iKeepAliveRetry)
{
	unsigned char		ucRet;

	int					iRet,
						iOpt,
						iSocket			= -1;

	fd_set				stReadFds,
						stExcpFds;

	struct timeval		stTime;

	struct sockaddr_in	stSockAddr;

	#if defined (WIN32)

		int				iOptLen;

	#elif defined (LINUX) || defined (UCLINUX)

		socklen_t		iOptLen;

	#endif
		
	p_pstSock->iErrNo = 0;

	if (0 >= p_pstSock->iSocket || 0x01 != p_pstSock->ucIsConnected)
	{
		ucRet = 0x02;
		goto CleanUp;
	}

	if (0xFFFFFFFF != p_ulTimeout)
	{
		FD_ZERO (&stReadFds);
		FD_ZERO (&stExcpFds);

		FD_SET (p_pstSock->iSocket, &stReadFds);
		FD_SET (p_pstSock->iSocket, &stExcpFds);

		if (0 != p_ulTimeout)
		{
			stTime.tv_sec = p_ulTimeout / 1000;

			if (0 == stTime.tv_sec)
			{
				stTime.tv_usec = p_ulTimeout;
			}
			else
			{
				stTime.tv_usec = 0;
			}
		}
		else
		{
			stTime.tv_sec = 0;
			stTime.tv_usec = 0;
		}

		// Check whether got new incoming connection or not
		iRet = select (p_pstSock->iSocket + 1, &stReadFds, 0, &stExcpFds, &stTime);

		if (0 == iRet)
		{
			ucRet = 0x05;	// No incoming connection
			goto CleanUp;
		}
		else if (0 < iRet)
		{
			if (0 != FD_ISSET (p_pstSock->iSocket, &stExcpFds))
			{
				ucRet = 0x08;	// Listen socket exception
				goto CleanUp;
			}
			
			if (0 == FD_ISSET (p_pstSock->iSocket, &stReadFds))
			{
				ucRet = 0x05;	// No incoming connection
				goto CleanUp;
			}
		}
		else
		{
			if (EINTR == errno)
			{
				ucRet = 0x09;	// assuming interrupted signal is no connected
			}
			else
			{
				ucRet = 0x07;	// Failed to accept during select
			}

			goto CleanUp;
		}
	}

	if (0 != p_pstNewSock)
	{
		iRet = sizeof (p_pstNewSock->stTCP_SocketAddr);
		SCK_ACCEPT (p_pstNewSock->iSocket, p_pstSock->iSocket, (struct sockaddr *) &(p_pstNewSock->stTCP_SocketAddr), &iRet);
	}
	else
	{
		// Drop the accepted connection immediately
		iRet = sizeof (stSockAddr);
		SCK_ACCEPT (iSocket, p_pstSock->iSocket, (struct sockaddr *) &stSockAddr, &iRet);
		SCK_CLOSE (iSocket);

		ucRet = 0x05;
		goto CleanUp;
	}

	if (-1 == p_pstNewSock->iSocket)
	{
		if (EINTR == errno)
		{
			ucRet = 0x09;	// assuming interrupted signal is no connected
		}
		else
		{
			ucRet = 0x03;	// Accepted socket is not valid
		}

		goto CleanUp;
	}

	// Set the receive buffer space
	if (0 < p_pstNewSock->iRecvBufSize)
	{
		iOpt = p_pstNewSock->iRecvBufSize;

		if (0 != setsockopt (p_pstNewSock->iSocket, SOL_SOCKET, SO_RCVBUF, (char *) &iOpt, sizeof (iOpt)))
		{
			ucRet = 0x06;	// Failed to set options
			goto CleanUp;
		}
	}

	// Set the send buffer space
	if (0 < p_pstNewSock->iSendBufSize)
	{
		iOpt = p_pstNewSock->iSendBufSize;
		
		if (0 != setsockopt (p_pstNewSock->iSocket, SOL_SOCKET, SO_SNDBUF, (char *) &iOpt, sizeof (iOpt)))
		{
			ucRet = 0x06;	// Failed to set options
			goto CleanUp;
		}
	}

	// Set the keepalive flag
	iOpt = 1;
	
	if (0 != setsockopt (p_pstNewSock->iSocket, SOL_SOCKET, SO_KEEPALIVE, (char *) &iOpt, sizeof (iOpt)))
	{
		ucRet = 0x06;	// Failed to set options
		goto CleanUp;
	}

	#if defined (LINUX) || defined (UCLINUX)

		// Set the keepalive timeout
		if (0 < p_iKeepAliveRetry || 0 < p_iKeepAliveInterval)
		{
			iOpt = 1;	// in second

			if (0 != setsockopt (p_pstNewSock->iSocket, SOL_TCP, TCP_KEEPIDLE, (char *) &iOpt, sizeof (iOpt)))
			{
				ucRet = 0x06;	// Failed to set options
				goto CleanUp;
			}
		}

		// Set the keepalive count
		if (0 < p_iKeepAliveRetry)
		{
			iOpt = p_iKeepAliveRetry;

			if (0 != setsockopt (p_pstNewSock->iSocket, SOL_TCP, TCP_KEEPCNT, (char *) &iOpt, sizeof (iOpt)))
			{
				ucRet = 0x06;	// Failed to set options
				goto CleanUp;
			}
		}

		// Set the keepalive interval
		if (0 < p_iKeepAliveInterval)
		{
			iOpt = p_iKeepAliveInterval;	// in second

			if (0 != setsockopt (p_pstNewSock->iSocket, SOL_TCP, TCP_KEEPINTVL, (char *) &iOpt, sizeof (iOpt)))
			{
				ucRet = 0x06;	// Failed to set options
				goto CleanUp;
			}
		}

	#endif

	// Get local address
	iOptLen = sizeof (stSockAddr);

	if (0 == getsockname (p_pstNewSock->iSocket, (struct sockaddr *) &stSockAddr, &iOptLen))
	{
		p_pstNewSock->iLocPort = ntohs (stSockAddr.sin_port);
		strncpy (p_pstNewSock->acLocIP, inet_ntoa (stSockAddr.sin_addr), sizeof (p_pstNewSock->acLocIP) - 1);
	}
	else
	{
		p_pstNewSock->iLocPort = p_pstNewSock->iLocPort;
		strncpy (p_pstNewSock->acLocIP, p_pstSock->acLocIP, sizeof (p_pstNewSock->acLocIP) - 1);
	}

	// Get remote address
	p_pstNewSock->iRemPort = ntohs (p_pstNewSock->stTCP_SocketAddr.sin_port);
	strncpy (p_pstNewSock->acRemIP, inet_ntoa (p_pstNewSock->stTCP_SocketAddr.sin_addr), sizeof (p_pstNewSock->acRemIP) - 1);

	p_pstNewSock->ucIsConnected = 0x01;
	p_pstNewSock->ucMode = CSOCKET_MODE_CONNECTION_IN;

	ucRet = 0x00;

CleanUp:

	return ucRet;
}

unsigned char sock_connect (CSOCKET *p_pstSock, char *p_pcRemIP, int p_iLocPort, int p_iRemPort, unsigned long p_ulTimeout, int p_iKeepAliveInterval, int p_iKeepAliveRetry)
{
	unsigned char		ucRet;

	int					iRet,
						iSocket		= -1,
						iOpt,
						iType;

	char				*pcPtr;

	struct hostent		*pstHost;

	fd_set				stSendFds,
						stExcpFds;

	struct timeval		stTime;

	#if defined (WIN32)

		unsigned long	ulIP;

		int				iOptLen;

	#elif defined (LINUX) || defined (UCLINUX)

		socklen_t		iOptLen;

	#endif

	if (0 == p_pcRemIP || 0 > p_iLocPort || 0 >= p_iRemPort)
	{
		ucRet = 0x07;
		goto CleanUp;
	}

	switch (p_pstSock->ucType)
	{
	case CSOCKET_TYPE_TCP:	iType = AF_INET; break;
	case CSOCKET_TYPE_UNIX:	iType = AF_UNIX; break;
	default:
		ucRet = 0x07;
		goto CleanUp;
	}

	// Create socket
	SCK_ALLOC (iSocket, iType, SOCK_STREAM, 0);	
	
	if (0 >= iSocket)
	{
		ucRet = 0x04;	// Failed to create socket
		goto CleanUp;
	}

	sock_close (p_pstSock);

	p_pstSock->iSocket = iSocket;
	iSocket = -1;

	switch (p_pstSock->ucType)
	{
	case CSOCKET_TYPE_TCP:
		p_pstSock->stTCP_SocketAddr.sin_family = AF_INET;

		if (0 < p_iLocPort)
		{
			p_pstSock->stTCP_SocketAddr.sin_port = htons ((u_short) p_iLocPort);
			
			memset (&p_pstSock->stTCP_SocketAddr.sin_zero, 0x00, sizeof (p_pstSock->stTCP_SocketAddr.sin_zero));

			// Set the reuse local port flag
			iOptLen = sizeof (iOpt);
			iOpt = 1;
			
			#if defined (WIN32)

				if (0 != setsockopt (p_pstSock->iSocket, SOL_SOCKET, SO_REUSEADDR, (char *) &iOpt, iOptLen))
				{
					p_pstSock->iErrNo = errno;

					ucRet = 0x05;	// Failed to set options
					goto CleanUp;
				}

			#elif defined (LINUX) || defined (UCLINUX)

				if (0 != setsockopt (p_pstSock->iSocket, SOL_SOCKET, SO_REUSEADDR, (void *) &iOpt, iOptLen))
				{
					p_pstSock->iErrNo = errno;

					ucRet = 0x05;	// Failed to set options
					goto CleanUp;
				}

			#endif
			
			iOptLen = sizeof (p_pstSock->stTCP_SocketAddr);

			if (0 != bind (p_pstSock->iSocket, (struct sockaddr *) &(p_pstSock->stTCP_SocketAddr), iOptLen))
			{
				p_pstSock->iErrNo = errno;

				ucRet = 0x08;	// Failed to bind local port
				goto CleanUp;
			}
		}

		pstHost = gethostbyname (p_pcRemIP);	// Resolve computer name into IP addr

		if (0 != pstHost)
		{
			p_pstSock->stTCP_SocketAddr.sin_family = pstHost->h_addrtype;

			if (pstHost->h_length > (int) sizeof (p_pstSock->stTCP_SocketAddr.sin_addr))
			{
				pstHost->h_length = sizeof (p_pstSock->stTCP_SocketAddr.sin_addr);
			}

			memcpy (&p_pstSock->stTCP_SocketAddr.sin_addr, pstHost->h_addr, pstHost->h_length);
		}
		else
		{
			#if defined (WIN32)

				ulIP = inet_addr (p_pcRemIP);	// Convert IP addr (X.X.X.X) to numerical value

				if (INADDR_NONE == ulIP)
				{
					ucRet = 0x02;
					goto CleanUp;
				}

				p_pstSock->stTCP_SocketAddr.sin_addr.S_un.S_addr = ulIP;
					
			#elif defined (LINUX) || defined (UCLINUX)

				if (!inet_aton (p_pcRemIP, &p_pstSock->stTCP_SocketAddr.sin_addr))
				{
					ucRet = 0x02;
					goto CleanUp;
				}

			#endif
		}

		p_pstSock->stTCP_SocketAddr.sin_port = htons ((u_short) p_iRemPort);

		if (0 < p_pstSock->iRecvBufSize)
		{
			// Set the receive buffer space
			iOpt = p_pstSock->iRecvBufSize;
			iOptLen = sizeof (iOpt);

			if (0 != setsockopt (p_pstSock->iSocket, SOL_SOCKET, SO_RCVBUF, (char *) &iOpt, iOptLen))
			{
				ucRet = 0x05;	// Failed to set options
				goto CleanUp;
			}
		}
		
		break;

	#if defined (LINUX) || defined (UCLINUX)

	case CSOCKET_TYPE_UNIX:

		p_pstSock->stUnix_SocketAddr.sun_family = AF_UNIX;
		strncpy (p_pstSock->stUnix_SocketAddr.sun_path, p_pcRemIP, sizeof (p_pstSock->stUnix_SocketAddr.sun_path) - 1);
		break;

	#endif
	}

	if (0 < p_pstSock->iSendBufSize)
	{
		// Set the send buffer space
		iOpt = p_pstSock->iSendBufSize;
		iOptLen = sizeof (iOpt);
		
		if (0 != setsockopt (p_pstSock->iSocket, SOL_SOCKET, SO_SNDBUF, (const char *) &iOpt, iOptLen))
		{
			ucRet = 0x05;	// Failed to set options
			goto CleanUp;
		}
	}

	// Set the keepalive flag
	iOpt = 1;
	iOptLen = sizeof (iOpt);
	
	if (0 != setsockopt (p_pstSock->iSocket, SOL_SOCKET, SO_KEEPALIVE, (const char *) &iOpt, iOptLen))
	{
		ucRet = 0x05;	// Failed to set options
		goto CleanUp;
	}

	#if defined (LINUX) || defined (UCLINUX)

		// Set the keepalive idle and interval
		if (0 < p_iKeepAliveInterval)
		{
			iOpt = p_iKeepAliveInterval;	// in second
			iOptLen = sizeof (iOpt);

			if (0 != setsockopt (p_pstSock->iSocket, SOL_TCP, TCP_KEEPIDLE, (const char *) &iOpt, iOptLen))
			{
				ucRet = 0x05;	// Failed to set options
				goto CleanUp;
			}

			iOpt = p_iKeepAliveInterval;	// in second
			iOptLen = sizeof (iOpt);

			if (0 != setsockopt (p_pstSock->iSocket, SOL_TCP, TCP_KEEPINTVL, (const char *) &iOpt, iOptLen))
			{
				ucRet = 0x05;	// Failed to set options
				goto CleanUp;
			}
		}

		// Set the keepalive count
		if (0 < p_iKeepAliveRetry)
		{
			iOpt = p_iKeepAliveRetry;	// counter
			iOptLen = sizeof (iOpt);

			if (0 != setsockopt (p_pstSock->iSocket, SOL_TCP, TCP_KEEPCNT, (const char *) &iOpt, iOptLen))
			{
				ucRet = 0x05;	// Failed to set options
				goto CleanUp;
			}
		}

	#endif

	// Allocate memory to store remote host name / IP
	// CAUTION: To prevent self copy (passed in local module IP variable)
	if (p_pstSock->acRemIP != p_pcRemIP)
	{
		memset (p_pstSock->acRemIP, 0x00, sizeof (p_pstSock->acRemIP));
		strncpy (p_pstSock->acRemIP, p_pcRemIP, sizeof (p_pstSock->acRemIP) - 1);
	}

	p_pstSock->iRemPort = p_iRemPort;

	if (0 < p_ulTimeout)
	{
		// Set socket to non-blocking mode
		iOpt = 1;

		#if defined (WIN32)

			iRet = ioctlsocket (p_pstSock->iSocket, FIONBIO, &iOpt);

		#elif defined (LINUX) || defined (UCLINUX)

			iRet = ioctl (p_pstSock->iSocket, FIONBIO, &iOpt);

		#endif

		if (0 != iRet)
		{
			ucRet = 0x05;	// Failed to set options
			goto CleanUp;
		}
	}

	switch (p_pstSock->ucType)
	{
	case CSOCKET_TYPE_TCP:
		iOpt = connect (p_pstSock->iSocket, (struct sockaddr*) &(p_pstSock->stTCP_SocketAddr), sizeof (struct sockaddr_in));
		break;

	#if defined (LINUX) || defined (UCLINUX)

	case CSOCKET_TYPE_UNIX:
		iOpt = connect (p_pstSock->iSocket, (struct sockaddr*) &(p_pstSock->stUnix_SocketAddr), sizeof (struct sockaddr_un));
		break;

	#endif
	}

	p_pstSock->iErrNo = errno;

	if (0 != iOpt)
	{
		#if defined (LINUX) || defined (UCLINUX)

			if (!(EINPROGRESS == p_pstSock->iErrNo && 0 < p_ulTimeout))
			{
				ucRet = 0x06;	// Failed to connect
				goto CleanUp;
			}

		#endif

		FD_ZERO (&stSendFds);
		FD_ZERO (&stExcpFds);

		FD_SET (p_pstSock->iSocket, &stSendFds);
		FD_SET (p_pstSock->iSocket, &stExcpFds);

		stTime.tv_sec = (time_t) (p_ulTimeout / 1000);
		stTime.tv_usec = (long) (1000 * (p_ulTimeout % 1000));

		// Wait connection status until timeout
		iOpt = select (p_pstSock->iSocket + 1, 0, &stSendFds, 0, &stTime);

		p_pstSock->iErrNo = errno;

		if (0 < iOpt)
		{
			if (FD_ISSET (p_pstSock->iSocket, &stSendFds))
			{
				iOpt = 1;
				iOptLen = sizeof (iOpt);

				#if defined (WIN32)

					if (0 == getsockopt (p_pstSock->iSocket, SOL_SOCKET, SO_ERROR, (char *) &iOpt, &iOptLen))
					{
						p_pstSock->iErrNo = iOpt;

						if (0 != iOpt)
						{
							ucRet = 0x06;	// Failed to connect
							goto CleanUp;
						}
					}

				#elif defined (LINUX) || defined (UCLINUX)

					if (0 == getsockopt (p_pstSock->iSocket, SOL_SOCKET, SO_ERROR, &iOpt, &iOptLen))
					{
						p_pstSock->iErrNo = iOpt;

						if (0 != iOpt)
						{
							ucRet = 0x06;	// Failed to connect
							goto CleanUp;
						}
					}

				#endif
			}
			else
			{
				ucRet = 0x0B;	// Failed to get status
				goto CleanUp;
			}
		}
		else if (0 != iOpt)
		{
			ucRet = 0x0B;	// Failed to get status
			goto CleanUp;
		}
		else
		{
			ucRet = 0x09;	// Connect timeout
			goto CleanUp;
		}
	}

	// ===============
	// ** Connected **
	// ===============

	if (0 < p_ulTimeout)
	{
		// Set socket to blocking mode
		iOpt = 0;

		#if defined (WIN32)

			iRet = ioctlsocket (p_pstSock->iSocket, FIONBIO, &iOpt);

		#elif defined (LINUX) || defined (UCLINUX)

			iRet = ioctl (p_pstSock->iSocket, FIONBIO, &iOpt);

		#endif

		if (0 != iRet)
		{
			ucRet = 0x05;	// Failed to set options
			goto CleanUp;
		}
	}

	p_pstSock->iLocPort = p_iLocPort;

	if (CSOCKET_TYPE_TCP == p_pstSock->ucType)
	{
		// Get the local port
		iOptLen = sizeof ((p_pstSock->stTCP_SocketAddr));

		if (0 == getsockname (p_pstSock->iSocket, (struct sockaddr *) &(p_pstSock->stTCP_SocketAddr), &iOptLen))
		{
			p_pstSock->iLocPort = ntohs (p_pstSock->stTCP_SocketAddr.sin_port);
		}

		// Copy local IP address
		pcPtr = inet_ntoa (p_pstSock->stTCP_SocketAddr.sin_addr);

		if (0 != pcPtr)
		{
			strncpy (p_pstSock->acLocIP, pcPtr, sizeof (p_pstSock->acLocIP) - 1);
		}
	}

	p_pstSock->ucIsConnected = 0x01;	// Flag to connected
	p_pstSock->ucMode = CSOCKET_MODE_CONNECTION_OUT;

	ucRet = 0x00;

CleanUp:
	SCK_CLOSE (iSocket);

	return ucRet;
}

unsigned char sock_send (CSOCKET *p_pstSock, unsigned char *p_pucData, int p_iLen)
{
	unsigned char	ucRet;

	int				iRet,
					iSent;
	
	if (0 == p_pucData || 0 > p_iLen)
	{
		ucRet = 0x04;	// No data is pass in
		goto CleanUp;
	}
	else if (0x00 == p_pstSock->ucIsConnected || 0 > p_pstSock->iSocket)
	{
		ucRet = 0x02;	// Socket is not connected
		goto CleanUp;
	}

	iSent = 0;

	do
	{
		iRet = send (p_pstSock->iSocket, (char *) p_pucData, p_iLen - iSent, 0);

		if (0 >= iRet)
		{
			p_pstSock->iErrNo = errno;
			
			ucRet = 0x03;	// Failed to send out data
			goto CleanUp;
		}

		iSent += iRet;
		p_pucData += iRet;
	}
	while (iSent < p_iLen);

	ucRet = 0x00;

CleanUp:

	return ucRet;
}

unsigned char sock_recv (CSOCKET *p_pstSock, unsigned char *p_pucData, int p_iSize, int *p_piLen, unsigned long p_ulTimeout)
{
	unsigned char	ucRet;

	int				iRet;

	fd_set			stReadFds,
					stExcpFds;

	struct timeval	stTime;

	if (0 == p_pucData)
	{
		ucRet = 0x01;	// Pass in parameter(s) is not correct
		goto CleanUp;
	}
	else
	{
		*p_piLen = 0;
		p_pstSock->iErrNo = 0;
	}

	if (0x00 == p_pstSock->ucIsConnected || 0 > p_pstSock->iSocket)
	{
		ucRet = 0x02;	// Socket is not connected
		goto CleanUp;
	}
	
	if (0xFFFFFFFF != p_ulTimeout)
	{
		FD_ZERO (&stReadFds);
		FD_ZERO (&stExcpFds);

		FD_SET (p_pstSock->iSocket, &stReadFds);
		FD_SET (p_pstSock->iSocket, &stExcpFds);

		stTime.tv_sec = (time_t) (p_ulTimeout / 1000);
		stTime.tv_usec = (long) (1000 * (p_ulTimeout % 1000));

		// Check whether got new data or not
		iRet = select (p_pstSock->iSocket + 1, &stReadFds, 0, &stExcpFds, &stTime);

		if (0 == iRet)
		{
			ucRet = 0x00;	// No incoming data
			goto CleanUp;
		}
		else if (0 < iRet && 0 < p_pstSock->iSocket)
		{
			if (0 != FD_ISSET (p_pstSock->iSocket, &stExcpFds))
			{
				ucRet = 0x06;	// Socket exception
				goto CleanUp;
			}

			if (0 == FD_ISSET (p_pstSock->iSocket, &stReadFds))
			{
				ucRet = 0x00;	// No incoming data
				goto CleanUp;
			}

			if (0 >= p_iSize)
			{
				ucRet = 0x07;	// Have incoming data, but dont want to recv
				goto CleanUp;
			}
		}
		else
		{
			p_pstSock->iErrNo = errno;

			if (EINTR == p_pstSock->iErrNo)
			{
				ucRet = 0x08;	// assuming interrupted signal is no incoming connection
			}
			else
			{
				ucRet = 0x03;	// Socket error during checking receive status
			}

			goto CleanUp;
		}
	}

	iRet = recv (p_pstSock->iSocket, (char *) p_pucData, p_iSize, 0);

	if (0 > iRet)
	{
		p_pstSock->iErrNo = errno;

		#if defined (LINUX) || defined (UCLINUX)

			if (EWOULDBLOCK == p_pstSock->iErrNo)
			{
				ucRet = 0x00;	// No incoming data
				goto CleanUp;
			}

		#endif

		if (EINTR == p_pstSock->iErrNo)
		{
			ucRet = 0x08;	// Socket interrupted
			goto CleanUp;
		}
		else
		{
			#if defined (WIN32)

				p_pstSock->iErrNo = WSAGetLastError ();

				switch (p_pstSock->iErrNo)
				{
				case WSAETIMEDOUT:
					ucRet = 0x00;	// No incoming data
					goto CleanUp;

				case WSAECONNRESET:
				case WSAENOTSOCK:
					ucRet = 0x05;	// Socket disconnected
					goto CleanUp;
				}

			#endif

			ucRet = 0x04;	// Socket error during receive
			goto CleanUp;
		}
	}
	else if (0 == iRet)
	{
		p_pstSock->iErrNo = 0;
		ucRet = 0x05;	// Socket disconnected
		goto CleanUp;
	}

	*p_piLen = iRet;
	ucRet = 0x00;

CleanUp:

	return ucRet;
}
