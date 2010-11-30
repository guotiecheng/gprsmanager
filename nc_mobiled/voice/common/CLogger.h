#ifndef _LOGGER_VOICE_H_
#define _LOGGER_VOICE_H_

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include <sys/timeb.h>

#if defined (LINUX) || defined (UCLINUX)

	#include <pthread.h>

	#include <unistd.h>
	#include <stdlib.h>
	#include <fcntl.h>
	#include <signal.h>
	#include <sys/types.h>
	#include <sys/wait.h>
	#include <sys/stat.h>
	#include <errno.h>

	extern int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int kind);

#endif

#include "GMaster.h"

// Log Level Definition
#define	LOG_DISABLE		0x05
#define	LOG_ERROR		0x04
#define	LOG_WARNING		0x03
#define	LOG_NORMAL		0x02
#define	LOG_DETAIL		0x01
#define	LOG_ALL			0x00

#define LOG_FILE_NAME_SIZE		256
#define	LOG_MESSAGE_BUFFER_SIZE 3048

typedef struct LOGGER_VOICE_T
{
	unsigned char		ucLogLevel,
						ucLogType,	// 1 - Daily File, 2 - One File
						ucHeader,
						ucMute,
						ucConsole;

	char				acLogFileName [LOG_FILE_NAME_SIZE + 1],
						acLogDateTime [32],
						acLogBuf [LOG_MESSAGE_BUFFER_SIZE],
						acLogMute [LOG_MESSAGE_BUFFER_SIZE];

	int					iMuteLen;

	unsigned long		ulCurSize,
						ulFileSize;
	
	FILE				*pfdLog;

	struct timeb		stTime;
	struct tm			*pstTime;

	#if defined (WIN32)
		
		long			hLock;
	
	#elif defined (LINUX) || defined (UCLINUX)
		
		pthread_mutex_t	hLock;
		
	#endif
}
LOGGER_VOICE;

void logger_init_voice (LOGGER_VOICE *p_pstLog, char *p_pcFileName, unsigned char p_ucLogLevel, unsigned long p_ulFileSize, unsigned char p_ucMute, unsigned char p_ucConsole);
void logger_term_voice (LOGGER_VOICE *p_pstLog);
unsigned char logger_log (LOGGER_VOICE *p_pstLog, unsigned char p_ucLogLevel, char *p_pcMsg, ...);

unsigned char logger_truncate_voice (LOGGER_VOICE *p_pstLog);

#endif
