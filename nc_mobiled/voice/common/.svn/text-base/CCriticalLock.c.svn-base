#include "CCriticalLock.h"

// =========================================================================
// | Initialize critical section
// |------------------------------------------------------------------------
// | Note         : None
// |------------------------------------------------------------------------
// | Return value : None
// |
// | Parameters   : Critical section structure pointer
// |------------------------------------------------------------------------
// | History (dd Mmm yyyy):
// | # 06 Jun 2008: SIK JEAN SOON created
// =========================================================================

void critical_init (CRITICAL_LOCK *p_pstLock)
{
	#if defined (WIN32)

		p_pstLock->hLock = 0;

	#elif defined (LINUX) || defined (UCLINUX)

		pthread_mutexattr_t attr;

		pthread_mutexattr_init (&attr);
		pthread_mutexattr_settype (&attr, PTHREAD_MUTEX_RECURSIVE_NP);
		pthread_mutex_init (&(p_pstLock->hLock), &attr);
		pthread_mutexattr_destroy (&attr);

	#endif

	p_pstLock->iMagic = CRITICAL_LOCK_MAGIC;
	p_pstLock->ucInit = 0x01;
}

// =========================================================================
// | Close a critical section
// |------------------------------------------------------------------------
// | Note         : None
// |------------------------------------------------------------------------
// | Return value : None
// |
// | Parameters   : Critical section structure pointer
// |------------------------------------------------------------------------
// | History (dd Mmm yyyy):
// | # 06 Jun 2008: SIK JEAN SOON created
// =========================================================================

void critical_term (CRITICAL_LOCK *p_pstLock)
{
	#if defined (LINUX) || defined (UCLINUX)

		if (CRITICAL_LOCK_MAGIC == p_pstLock->iMagic && 0x01 == p_pstLock->ucInit)
		{
			critical_lock (p_pstLock, 0x01);

			p_pstLock->ucInit = 0x00;
			p_pstLock->iMagic = 0;
			pthread_mutex_destroy (&(p_pstLock->hLock));
		}

	#endif
}

// =========================================================================
// | Lock a critical section
// |------------------------------------------------------------------------
// | Note         : None
// |------------------------------------------------------------------------
// | Return value : 0x00 - Success
// |                0x01 - Failed to lock critical section
// |
// | Parameters   : # Critical section structure pointer
// |                # 0x00 - Non-Block mode
// |                  0x01 - Block mode
// |------------------------------------------------------------------------
// | History (dd Mmm yyyy):
// | # 06 Jun 2008: SIK JEAN SOON created
// =========================================================================

unsigned char critical_lock (CRITICAL_LOCK *p_pstLock, unsigned char p_ucBlock)
{
	#if defined (WIN32)
			
		long	lThreadID;
	
	#endif
	
	if (CRITICAL_LOCK_MAGIC == p_pstLock->iMagic && 0x01 == p_pstLock->ucInit)
	{
		if (0x01 == p_ucBlock)
		{
			#if defined (WIN32)
				
				lThreadID = (long) GetCurrentThreadId ();
				
				while (p_pstLock->hLock != lThreadID)
				{
					if (0 == p_pstLock->hLock)
					{
						// The return value must be same as previous value (0) else it might locked by other
						if (0 == InterlockedExchange (&(p_pstLock->hLock), lThreadID))
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
				
				if (0 != pthread_mutex_lock (&(p_pstLock->hLock)))
				{
					return 0x01;
				}
				
			#endif
				
			return 0x00;
		}
		else
		{
			#if defined (WIN32)
				
				lThreadID = (long) GetCurrentThreadId ();

				if (0 == p_pstLock->hLock)
				{
					// The return value must be same as previous value (0) else it might locked by other
					if (0 == InterlockedExchange (&(p_pstLock->hLock), lThreadID))
					{
						return 0x00;
					}
				}
				
			#elif defined (LINUX) || defined (UCLINUX)

				if (0 == pthread_mutex_trylock (&(p_pstLock->hLock)))
				{
					return 0x00;
				}

			#endif
		}
	}

	return 0x01;
}

// =========================================================================
// | Unlock a critical section
// |------------------------------------------------------------------------
// | Note         : None
// |------------------------------------------------------------------------
// | Return value : 0x00 - Success
// |                0x01 - Failed to unlock critical section
// |
// | Parameters   : # Critical section structure pointer
// |------------------------------------------------------------------------
// | History (dd Mmm yyyy):
// | # 06 Jun 2008: SIK JEAN SOON created
// =========================================================================

unsigned char critical_unlock (CRITICAL_LOCK *p_pstLock)
{
	#if defined (WIN32)
			
		long	lThreadID;
	
	#endif
	
	if (CRITICAL_LOCK_MAGIC != p_pstLock->iMagic || 0x01 != p_pstLock->ucInit)
	{
		return 0x01;
	}

	#if defined (WIN32)
		
		lThreadID = (long) GetCurrentThreadId ();

		if (lThreadID == p_pstLock->hLock)
		{
			InterlockedExchange (&(p_pstLock->hLock), 0);
		}

	#elif defined (LINUX) || defined (UCLINUX)

		if (0 != pthread_mutex_unlock (&(p_pstLock->hLock)))
		{
			return 0x02;
		}

	#endif

	return 0x00;
}
