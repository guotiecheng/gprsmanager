#include "CLogger.h"

static char g_acLogLevel [] = {'A', 'D', 'N', 'W', 'E', ' '};

void logger_init_voice (LOGGER_VOICE *p_pstLog, char *p_pcFileName, unsigned char p_ucLogLevel, unsigned long p_ulFileSize, unsigned char p_ucMute, unsigned char p_ucConsole)
{
	#if defined (WIN32)
		
		p_pstLog->hLock = 0;
		
	#elif defined (LINUX) || defined (UCLINUX)
		
		pthread_mutexattr_t attr;

		pthread_mutexattr_init (&attr);
		pthread_mutexattr_settype (&attr, PTHREAD_MUTEX_RECURSIVE_NP);
		pthread_mutex_init (&(p_pstLog->hLock), &attr);
		pthread_mutexattr_destroy (&attr);

	#endif

	memset (p_pstLog->acLogFileName, 0x00, LOG_FILE_NAME_SIZE);
	strncpy (p_pstLog->acLogFileName, p_pcFileName, LOG_FILE_NAME_SIZE - 1);

	p_pstLog->ucLogType = 0x02;
	p_pstLog->ucLogLevel = p_ucLogLevel;
	p_pstLog->pfdLog = 0;
	p_pstLog->ucHeader = 0x00;
	p_pstLog->ulCurSize = 0;
	p_pstLog->ulFileSize = p_ulFileSize;

	p_pstLog->iMuteLen = 0;
	p_pstLog->ucMute = p_ucMute;
	p_pstLog->ucConsole = p_ucConsole;
}

void logger_term_voice (LOGGER_VOICE *p_pstLog)
{
	int	iLen;

	if (0 != p_pstLog->pfdLog)
	{
		if (0x01 == p_pstLog->ucHeader)
		{
			ftime (&(p_pstLog->stTime));
			p_pstLog->pstTime = localtime (&((p_pstLog->stTime).time));

			iLen = sprintf (p_pstLog->acLogBuf, "\n-------Date Time-------\tThread- -Level-\tMessage------------------------------- CLOSE %04d-%02d-%02d %02d:%02d:%02d.%03d (Size = %ld)\n\n",
				(p_pstLog->pstTime)->tm_year + 1900, (p_pstLog->pstTime)->tm_mon + 1, (p_pstLog->pstTime)->tm_mday, (p_pstLog->pstTime)->tm_hour, (p_pstLog->pstTime)->tm_min, (p_pstLog->pstTime)->tm_sec, (p_pstLog->stTime).millitm, p_pstLog->ulCurSize);
			
			if (0 < iLen)
			{
				fwrite (p_pstLog->acLogBuf, sizeof (char), iLen, p_pstLog->pfdLog);
				fflush (p_pstLog->pfdLog);
			}
		}

		FCLOSE (p_pstLog->pfdLog);
	}

	#if defined (LINUX) || defined (UCLINUX)

		pthread_mutex_destroy (&p_pstLog->hLock);

	#endif
}

unsigned char logger_log (LOGGER_VOICE *p_pstLog, unsigned char p_ucLogLevel, char *p_pcMsg, ...)
{
	unsigned char	ucRet	= 0x01;
	
	int				iTemp,
					iLen;
	
	#if defined (WIN32)
		
		long		lThreadID;
		
	#endif
	
	va_list			args;
	
	if (p_ucLogLevel < p_pstLog->ucLogLevel || 0 == p_pcMsg)
	{
		return 0x00;
	}

	// =======================
	// ** Lock share buffer **
	// =======================
	
	#if defined (WIN32)
		
		lThreadID = GetCurrentThreadId ();

		while (p_pstLog->hLock != lThreadID)
		{
			if (0 == p_pstLog->hLock)
			{
				// The return value must be same as previous value (0) else it might locked by other
				if (0 == InterlockedExchange (&(p_pstLog->hLock), lThreadID))
				{
					break;
				}
			}
			else
			{
				SLEEP (1);
			}
		}
		
	#elif defined (LINUX) || defined (UCLINUX)
		
		if (0 != pthread_mutex_lock (&(p_pstLog->hLock)))
		{
			return 0x01;
		}
		
	#endif

	ftime (&(p_pstLog->stTime));
	p_pstLog->pstTime = localtime (&(p_pstLog->stTime.time));
	
	// Manually form the DateTime String (200% faster than sprintf)
	iTemp = (p_pstLog->pstTime)->tm_year + 1900;	// Year
	iLen = (iTemp / 1000);
	p_pstLog->acLogDateTime [0] = '0' + iLen;
	iTemp = iTemp - (1000 * iLen);
	iLen = (iTemp / 100);
	p_pstLog->acLogDateTime [1] = '0' + iLen;
	iTemp = iTemp - (100 * iLen);
	iLen = (iTemp / 10);
	p_pstLog->acLogDateTime [2] = '0' + iLen;
	iTemp = iTemp - (10 * iLen);
	p_pstLog->acLogDateTime [3] = '0' + iTemp;
	p_pstLog->acLogDateTime [4] = '-';
	
	iTemp = (p_pstLog->pstTime)->tm_mon + 1;	// Month
	iLen = (iTemp / 10);
	p_pstLog->acLogDateTime [5] = '0' + iLen;
	iTemp = iTemp - (10 * iLen);
	p_pstLog->acLogDateTime [6] = '0' + iTemp;
	p_pstLog->acLogDateTime [7] = '-';

	iTemp = (p_pstLog->pstTime)->tm_mday;	// Day
	iLen = (iTemp / 10);
	p_pstLog->acLogDateTime [8] = '0' + iLen;
	iTemp = iTemp - (10 * iLen);
	p_pstLog->acLogDateTime [9] = '0' + iTemp;
	p_pstLog->acLogDateTime [10] = ' ';
	
	iTemp = (p_pstLog->pstTime)->tm_hour;	// Hour
	iLen = (iTemp / 10);
	p_pstLog->acLogDateTime [11] = '0' + iLen;
	iTemp = iTemp - (10 * iLen);
	p_pstLog->acLogDateTime [12] = '0' + iTemp;
	p_pstLog->acLogDateTime [13] = ':';
	
	iTemp = (p_pstLog->pstTime)->tm_min;	// Minute
	iLen = (iTemp / 10);
	p_pstLog->acLogDateTime [14] = '0' + iLen;
	iTemp = iTemp - (10 * iLen);
	p_pstLog->acLogDateTime [15] = '0' + iTemp;
	p_pstLog->acLogDateTime [16] = ':';
	
	iTemp = (p_pstLog->pstTime)->tm_sec;	// Second
	iLen = (iTemp / 10);
	p_pstLog->acLogDateTime [17] = '0' + iLen;
	iTemp = iTemp - (10 * iLen);
	p_pstLog->acLogDateTime [18] = '0' + iTemp;
	p_pstLog->acLogDateTime [19] = '.';
	
	iTemp = p_pstLog->stTime.millitm;		// Millisecond
	iLen = (iTemp / 100) % 10;
	p_pstLog->acLogDateTime [20] = '0' + iLen;
	iTemp = iTemp - (100 * iLen);
	iLen = (iTemp / 10);
	p_pstLog->acLogDateTime [21] = '0' + iLen;
	iTemp = iTemp - (10 * iLen);
	p_pstLog->acLogDateTime [22] = '0' + iTemp;
	p_pstLog->acLogDateTime [23] = 0;
	
	if (0x00 == p_pstLog->ucHeader)
	{
		p_pstLog->ucHeader = 0x01;
		
		#if defined (WIN32)
			
			iTemp = sprintf (p_pstLog->acLogBuf, "-------Date Time------- -Thread- L Message - START %s\n\n%s 00000000:%08X %c ", 
				p_pstLog->acLogDateTime, p_pstLog->acLogDateTime, GetCurrentThreadId (), g_acLogLevel [p_ucLogLevel]);
			
		#elif defined (LINUX) || defined (UCLINUX)

			iTemp = sprintf (p_pstLog->acLogBuf, "-------Date Time------- -Thread- L Message - START %s\n\n%s %08d:%08X %c ", 
				p_pstLog->acLogDateTime, p_pstLog->acLogDateTime, getpid (), (unsigned int) pthread_self (), g_acLogLevel [p_ucLogLevel]);
			
		#endif
	}
	else
	{
		#if defined (WIN32)
			
			iTemp = sprintf (p_pstLog->acLogBuf, "%s 00000000:%08X %c ", p_pstLog->acLogDateTime, GetCurrentThreadId (), g_acLogLevel [p_ucLogLevel]);
			
		#elif defined (LINUX) || defined (UCLINUX)

			iTemp = sprintf (p_pstLog->acLogBuf, "%s %08d:%08X %c ", p_pstLog->acLogDateTime, getpid (), (unsigned int) pthread_self (), g_acLogLevel [p_ucLogLevel]);
			
		#endif
	}

	if (0 > iTemp)
	{
		ucRet = 0x02;
		goto CleanUp;
	}
	
	iLen = iTemp;

	va_start (args, p_pcMsg);
	
	#if defined (WIN32)
		
		iTemp = _vsnprintf (p_pstLog->acLogBuf + iLen, LOG_MESSAGE_BUFFER_SIZE - iLen, p_pcMsg, args);
		
	#elif defined (LINUX) || defined (UCLINUX)
		
		iTemp = vsnprintf (p_pstLog->acLogBuf + iLen, LOG_MESSAGE_BUFFER_SIZE - iLen, p_pcMsg, args);
		
	#endif

	va_end (args);

	if (0 > iTemp)
	{
		ucRet = 0x03;
		goto CleanUp;
	}

	// Mute if duplicated log message
	if (0x00 != p_pstLog->ucMute)
	{
		if (0 < p_pstLog->iMuteLen && iTemp == p_pstLog->iMuteLen)
		{
			if (0 == memcmp (p_pstLog->acLogBuf + iLen, p_pstLog->acLogMute, p_pstLog->iMuteLen))
			{
				ucRet = 0x00;
				goto CleanUp;
			}
		}

		p_pstLog->iMuteLen = iTemp;
		memcpy (p_pstLog->acLogMute, p_pstLog->acLogBuf + iLen, p_pstLog->iMuteLen);
	}

	iLen += iTemp;
	p_pstLog->acLogBuf [iLen++] = '\n';


	// ===================
	// ** Write to file **
	// ===================
	
	if (0 != p_pstLog->ulFileSize && p_pstLog->ulFileSize < p_pstLog->ulCurSize)
	{
		logger_truncate_voice (p_pstLog);
	}
	else
	{
		if (0 == p_pstLog->pfdLog)
		{
			if (0 != p_pstLog->acLogFileName [0])
			{
				// Open file
				FOPEN (p_pstLog->pfdLog, p_pstLog->acLogFileName, "a+");

				if (0 == p_pstLog->pfdLog)
				{
					ucRet = 0x04;
					goto CleanUp;
				}

				fseek (p_pstLog->pfdLog, 0, SEEK_END);
				p_pstLog->ulCurSize = ftell (p_pstLog->pfdLog);
			}
		}
	}
		
	if (0 == p_pstLog->pfdLog)
	{
		ucRet = 0x04;
		goto CleanUp;
	}

	if (0x01 == p_pstLog->ucConsole)
	{
		fwrite (p_pstLog->acLogBuf, sizeof (char), iLen, stderr);	// Print to console
	}

	iTemp = fwrite (p_pstLog->acLogBuf, sizeof (char), iLen, p_pstLog->pfdLog);

	if (iLen == iTemp)
	{
		p_pstLog->ulCurSize += iTemp;
		fflush (p_pstLog->pfdLog);
	}
	else
	{
		FCLOSE (p_pstLog->pfdLog);
	}
	
	ucRet = 0x00;

CleanUp:
	#if defined (WIN32)
		
		lThreadID = (long) GetCurrentThreadId ();

		if (lThreadID == p_pstLog->hLock)
		{
			InterlockedExchange (&(p_pstLog->hLock), 0);
		}

	#elif defined (LINUX) || defined (UCLINUX)

		if (0 != pthread_mutex_unlock (&(p_pstLog->hLock)))
		{
			ucRet = 0x04;
		}

	#endif

	return ucRet;
}

unsigned char logger_truncate_voice (LOGGER_VOICE *p_pstLog)
{
	unsigned char	ucRet			= 0x01;

	FCLOSE (p_pstLog->pfdLog);

	if (0 != p_pstLog->acLogFileName [0])
	{
		// Open file
		FOPEN (p_pstLog->pfdLog, p_pstLog->acLogFileName, "w");

		if (0 == p_pstLog->pfdLog)
		{
			ucRet = 0x02;
		}
		else
		{
			p_pstLog->ulCurSize = 0;
			ucRet = 0x00;
		}
	}

	return ucRet;
}
