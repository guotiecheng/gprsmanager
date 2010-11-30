#ifndef LOCK_H__8096FEFA_E655_46ED_9D17_03C9AB4DFD82__INCLUDED_
#define LOCK_H__8096FEFA_E655_46ED_9D17_03C9AB4DFD82__INCLUDED_

#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

#if defined (WIN32)

	#include <windows.h>

#elif defined (LINUX) || defined (UCLINUX) 

	#include <time.h>
	#include <unistd.h>
	#include <pthread.h>

	extern int pthread_mutexattr_settype (pthread_mutexattr_t *attr, int kind);

#endif

#include "GMaster.h"

#define CRITICAL_LOCK_MAGIC				12345

typedef struct LOCK_T
{
	#if defined (WIN32)

		long				hLock;

	#elif defined (LINUX) || defined (UCLINUX)
		
		pthread_mutex_t		hLock;
		
	#endif

	unsigned char			ucInit;

	int						iMagic;
}
CRITICAL_LOCK;

void critical_init (CRITICAL_LOCK *p_pstLock);
void critical_term (CRITICAL_LOCK *p_pstLock);
unsigned char critical_lock (CRITICAL_LOCK *p_pstLock, unsigned char p_ucBlock);
unsigned char critical_unlock (CRITICAL_LOCK *p_pstLock);

#endif
