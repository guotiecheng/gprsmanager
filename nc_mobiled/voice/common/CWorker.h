#ifndef WORKER_H__CE6F419D_704D_4681_AE54_974C7C238302__INCLUDED_
#define WORKER_H__CE6F419D_704D_4681_AE54_974C7C238302__INCLUDED_

#include <signal.h>

#if defined (WIN32)

	// Remember to set the Project->Settings->C/C++->Category:Code Generation->Multithreded
	#include <windows.h>
	#include <process.h>

#elif defined (LINUX) || defined (UCLINUX)

	#include <pthread.h>

#endif

#include "GMaster.h"

typedef void (WRK_THREAD_BODY) (void *p_pstWrkThread);

#if defined (WIN32)
	
	unsigned __stdcall WrkThread_Body (void *p_pvWrkThread);

#elif defined (LINUX) || defined (UCLINUX)

	void *WrkThread_Body (void *p_pvWrkThread);

#endif
	
typedef struct WRK_THREAD_T
{
	unsigned char	ucProcToken,
					ucCreated;

	unsigned long	ulThreadID;

	WRK_THREAD_BODY *pfBody;
	
	void			*pvPtr;

	int				iErr;

	#if defined (WIN32)
		
		HANDLE		hThread;

	#elif defined (LINUX) || defined (UCLINUX)

		pthread_t	hThread;

	#endif
}
WRK_THREAD;

void thread_init (WRK_THREAD *p_pstWrkThread, void *p_pvPtr);
unsigned char thread_start (WRK_THREAD *p_pstWrkThread, WRK_THREAD_BODY *p_pfBody);
unsigned char thread_close (WRK_THREAD *p_pstWrkThread, int p_iSignal, unsigned long p_ulTimeout);

#endif

