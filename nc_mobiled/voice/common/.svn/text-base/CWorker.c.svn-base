#include "CWorker.h"

void thread_init (WRK_THREAD *p_pstWrkThread, void *p_pvPtr)
{
	p_pstWrkThread->ucProcToken = 0x00;
	p_pstWrkThread->ucCreated = 0x00;
	p_pstWrkThread->iErr = 0;
	p_pstWrkThread->pvPtr = p_pvPtr;

	p_pstWrkThread->ulThreadID = 0;

	#if defined (WIN32)

		p_pstWrkThread->hThread = 0;
	
	#endif
}

unsigned char thread_start (WRK_THREAD *p_pstWrkThread, WRK_THREAD_BODY *p_pfBody)
{
	if (0x00 != p_pstWrkThread->ucCreated)
	{
		return 0x01;	// Worker thread was started
	}

	p_pstWrkThread->pfBody = p_pfBody;
	p_pstWrkThread->ucProcToken = 0x01;

	#if defined (WIN32)

		// Create worker thread that point to function body
		p_pstWrkThread->hThread = (HANDLE) _beginthreadex (0, 0, &WrkThread_Body, (void *) p_pstWrkThread, 0, &(p_pstWrkThread->ulThreadID));

		if (0 != p_pstWrkThread->hThread && INVALID_HANDLE_VALUE != p_pstWrkThread->hThread && 0 != p_pstWrkThread->ulThreadID)
		{
			p_pstWrkThread->ucCreated = 0x01;
			return 0x00;
		}
		else
		{
			p_pstWrkThread->iErr = (int) GetLastError ();
		}

	#elif defined (LINUX) || defined (UCLINUX)

		// Create worker thread that point to current class static function
		p_pstWrkThread->iErr = pthread_create (&(p_pstWrkThread->hThread), 0, WrkThread_Body, (void *) p_pstWrkThread);

		if (0 == p_pstWrkThread->iErr)
		{
			while (0 == p_pstWrkThread->ulThreadID)
			{
				sleep (1);
			}

			p_pstWrkThread->ucCreated = 0x01;
			return 0x00;
		}

	#endif

	return 0x02;	// Failed to create thread
}

unsigned char thread_close (WRK_THREAD *p_pstWrkThread, int p_iSignal, unsigned long p_ulTimeout)
{
	if (0x00 == p_pstWrkThread->ucCreated)
	{
		return 0x00;
	}

	// Notify worker thread to stop process and go home
	p_pstWrkThread->ucProcToken = 0x00;

	#if defined (WIN32)

		if (0 != p_pstWrkThread->hThread && INVALID_HANDLE_VALUE != p_pstWrkThread->hThread)
		{
			// Wait the thread to stop
			if (WAIT_OBJECT_0 == WaitForSingleObject (p_pstWrkThread->hThread, p_ulTimeout))
			{
				CloseHandle (p_pstWrkThread->hThread);

				p_pstWrkThread->hThread = 0;
				p_pstWrkThread->ulThreadID = 0;
				p_pstWrkThread->ucCreated = 0;
			
				return 0x00;
			}
			else
			{
				return 0x01;	// Failed to stop worker thread
			}
		}
		else
		{
			return 0x00;
		}

	#elif defined (LINUX) || defined (UCLINUX)

		if (0 != p_pstWrkThread->ulThreadID)
		{
			if (0 != p_iSignal)
			{
				if (0 != pthread_kill (p_pstWrkThread->hThread, p_iSignal))
				{
					return 0x02;	// Failed to signal thread
				}
			}

			// Wait the thread to stop
			if (0 != pthread_join (p_pstWrkThread->hThread, 0))
			{
				return 0x01;	// Failed to stop worker thread
			}
		}

		p_pstWrkThread->ulThreadID = 0;
		p_pstWrkThread->ucCreated = 0x00;

		return 0x00;

	#endif
}

#if defined (WIN32)
	
	unsigned __stdcall WrkThread_Body (void *p_pvWrkThread)
	{
		(((WRK_THREAD *) p_pvWrkThread)->pfBody) (p_pvWrkThread);
		return 0;
	}

#elif defined (LINUX) || defined (UCLINUX)

	void *WrkThread_Body (void *p_pvWrkThread)
	{
		WRK_THREAD	*pstWrk		= (WRK_THREAD *) p_pvWrkThread;

		pstWrk->ulThreadID = (unsigned long) pthread_self ();
		pstWrk->pfBody (p_pvWrkThread);
		pthread_exit (0);

		return 0;
	}

#endif
