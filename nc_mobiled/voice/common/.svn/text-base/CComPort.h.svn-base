#ifndef COM_PORT_VOICE_H
#define COM_PORT_VOICE_H

#include <stdarg.h>

#if defined (LINUX) || defined (UCLINUX)

	#include <termios.h>
	#include <stdio.h>
	#include <unistd.h>
	#include <fcntl.h>
	#include <errno.h>
	#include <sys/ioctl.h>
	#include <sys/signal.h>
	#include <sys/types.h>

#endif

#include "GMaster.h"

#define COM_PORT_VOICE_MAGIC					123456

typedef void (*COM_FRAG_DELAY) (void *p_pvPtr, unsigned int len);
typedef void (*COM_FRAG_END) (void *p_pvPtr);

typedef struct COM_PORT_VOICE_T
{
	unsigned char		ucDataBit,
						ucStopBit,
						ucParity,
						ucFlowCtrl,
						ucIsConnected;

	int					iFragSize,
						iMagic;

	long				lBaudrate;

	char				acDevFile [MAX_TINY_BUFFER_SZ];

	#if defined (WIN32)

		HANDLE			hComPort;

	#elif defined (LINUX) || defined (UCLINUX)

		int				hComPort;

	#endif

	COM_FRAG_DELAY		pfFragDelay;
	COM_FRAG_END		pfFragEnd;

	void				*pvBasket;
}
COM_PORT_VOICE;

void comport_init_voice (COM_PORT_VOICE *p_pstComPort, char *p_pcDevFile, long p_lBaudrate, unsigned char p_ucDataBit, unsigned char p_ucStopBit, unsigned char p_ucParity, unsigned char p_ucFlowCtrl, int p_iFragSize, COM_FRAG_DELAY p_pfFragDelay, COM_FRAG_END p_pfFragEnd, void *p_pvBasket);
void comport_term_voice (COM_PORT_VOICE *p_pstComPort);

unsigned char comport_open_voice (COM_PORT_VOICE *p_pstComPort);
void comport_close_voice (COM_PORT_VOICE *p_pstComPort);

unsigned char comport_send_voice (COM_PORT_VOICE *p_pstComPort, unsigned char *p_pucData, int p_iLen);
unsigned char comport_recv_voice (COM_PORT_VOICE *p_pstComPort, unsigned char *p_pucData, int p_iSize, int *p_piLen, unsigned long p_ulTimeout);

#endif
