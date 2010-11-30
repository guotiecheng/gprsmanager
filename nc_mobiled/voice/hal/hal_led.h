#ifndef HAL_LED_H
#define HAL_LED_H

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

#define HAL_LED_NAME										"LED       "

#define HAL_LED_STATE_IDLE									0x10
#define HAL_LED_STATE_PAUSE									0x20
#define HAL_LED_STATE_GAP									0x30
#define HAL_LED_STATE_REST									0x40
#define HAL_LED_STATE_ON									0x50
#define HAL_LED_STATE_OFF									0x60
#define HAL_LED_STATE_BLINK                                 0x70

#define HAL_LED_STATE_MAX                                   0xF0

#define HAL_LED_YELLOW_IDLE                                 HAL_LED_STATE_IDLE | 0x01
#define HAL_LED_GREEN_IDLE                                  HAL_LED_STATE_IDLE | 0x02
#define HAL_LED_RED_IDLE                                    HAL_LED_STATE_IDLE | 0x03

#define HAL_LED_YELLOW_ON                                   HAL_LED_STATE_ON | 0x01
#define HAL_LED_GREEN_ON                                    HAL_LED_STATE_ON | 0x02
#define HAL_LED_RED_ON                                      HAL_LED_STATE_ON | 0x03

#define HAL_LED_STATUS_OFF									0x00
#define HAL_LED_STATUS_ON									0x01


#if defined (ARCH_L100) || defined (ARCH_L200)

	#define HAL_LED_CTRL_DEV_FILE							"/dev/led"
	#define HAL_LED_MAX										6

#elif defined (ARCH_L300) || defined (ARCH_L350)

	#define HAL_LED_CTRL_DEV_FILE							"/dev/led"
	#define HAL_LED_MAX										8

#elif defined (ARCH_L300B)

	#define HAL_LED_CTRL_DEV_FILE							"/dev/led"
	#define HAL_LED_MAX										4

#elif defined (ARCH_N300)

	#define HAL_LED_CTRL_DEV_FILE							"/dev/led"
	#define HAL_LED_MAX										18

    #define CMD_OFF                 0x00
    #define CMD_ON                  0x01
    #define CMD_RED                 0x02
    #define CMD_YELLOW              0x03
    #define CMD_GREEN               0x04
    #define CMD_GET_LED_STATUS      0x05
    #define CMD_GET_LED_ALL_STATUS  0x06 

    #define CMD_ALL_OFF             0x07
    #define CMD_ALL_ON              0x08

    #define CMD_BLINK               0x09
    #define CMD_RED_BLINK           0x0a
    #define CMD_YELLOW_BLINK        0x0b
    #define CMD_GREEN_BLINK         0x0c
    #define CMD_STOP_BLINK          0x0d

#else

	#define HAL_LED_CTRL_DEV_FILE							""
	#define HAL_LED_MAX										1

#endif

typedef struct HAL_LED_SLOT_T
{
	unsigned char	ucIndex,
					ucIsUsed,
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
HAL_LED_SLOT;

typedef struct HAL_LED_T
{
	COM_PORT_VOICE		stCtrlPort;

	HAL_LED_SLOT	astLED [HAL_LED_MAX][2];
}
HAL_LED;

unsigned char hal_led_init (HAL_LED *p_pstHAL);
void hal_led_term (HAL_LED *p_pstHAL);
unsigned char hal_led_main (HAL_LED *p_pstHAL);

void hal_led (HAL_LED *p_pstHAL, unsigned char p_ucSlot, unsigned long p_ulPause, unsigned long p_ulGap, unsigned long p_ulRest, unsigned char p_ucCount,
			  unsigned char p_ucRepeat, unsigned char p_ucState, unsigned long p_ulTrgPoint);

void hal_led_on (HAL_LED *p_pstHAL, unsigned char p_ucSlot);
void hal_led_off (HAL_LED *p_pstHAL, unsigned char p_ucSlot);
void _hal_led_on (HAL_LED *p_pstHAL, unsigned char p_ucSlot, unsigned char p_ucSetFlag);
void _hal_led_off (HAL_LED *p_pstHAL, unsigned char p_ucSlot, unsigned char p_ucSetFlag);

unsigned char hal_led_slot (HAL_LED *p_pstHAL, char *p_pcIf);

#endif
