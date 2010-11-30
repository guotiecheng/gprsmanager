#ifndef HAL_BUZZER_H
#define HAL_BUZZER_H

#include <stdio.h>

#if defined (WIN32)

	#include <windows.h>

#elif defined (LINUX) || defined (UCLINUX)

	#include <unistd.h>
	#include <stdlib.h>
	#include <fcntl.h>
	#include <signal.h>
	#include <sys/types.h>
	#include <sys/wait.h>
	#include <errno.h>

#endif

#include "../local.h"

#define HAL_BUZZER_NAME										"BUZZER    "

#define HAL_BUZZER_STATE_IDLE								0x01
#define HAL_BUZZER_STATE_PAUSE								0x02
#define HAL_BUZZER_STATE_GAP								0x03
#define HAL_BUZZER_STATE_REST								0x04
#define HAL_BUZZER_STATE_ON									0x05
#define HAL_BUZZER_STATE_OFF								0x06

#define HAL_BUZZER_STATUS_OFF								0x00
#define HAL_BUZZER_STATUS_ON								0x01


#if defined (ARCH_L100) || defined (ARCH_L200) || defined (ARCH_L300) || defined (ARCH_L350) || defined (ARCH_N300) || defined (ARCH_L300B)

	#define HAL_BUZZER_CTRL_DEV_FILE						"/dev/beep"

#else

	#define HAL_BUZZER_CTRL_DEV_FILE						""

#endif

typedef struct HAL_BUZZER_SLOT_T
{
	unsigned char	ucIsUsed,
					ucInterrupt,
					ucCount,
					ucRepeat,
					ucState,	// Idle / Pause / gap / on / off
					ucStatus;	// On / off

	unsigned long	ulLastTime,
					ulExpiry,
					ulPause,
					ulGap,
					ulRest,
					ulTrgPoint;
}
HAL_BUZZER_SLOT;

typedef struct HAL_BUZZER_T
{
	COM_PORT_VOICE		stCtrlPort;

	HAL_BUZZER_SLOT	stBuzzer [2];
}
HAL_BUZZER;

unsigned char hal_buzzer_init (HAL_BUZZER *p_pstHAL);
void hal_buzzer_term (HAL_BUZZER *p_pstHAL);
unsigned char hal_buzzer_main (HAL_BUZZER *p_pstHAL);

void hal_buzzer (HAL_BUZZER *p_pstHAL, unsigned long p_ulPause, unsigned long p_ulGap, unsigned long p_ulRest, unsigned char p_ucCount,
				 unsigned char p_ucRepeat, unsigned char p_ucState, unsigned long p_ulTrgPoint);

void hal_buzzer_on (HAL_BUZZER *p_pstHAL);
void hal_buzzer_off (HAL_BUZZER *p_pstHAL);
void _hal_buzzer_on (HAL_BUZZER *p_pstHAL, unsigned char p_ucSetFlag);
void _hal_buzzer_off (HAL_BUZZER *p_pstHAL, unsigned char p_ucSetFlag);

#endif
