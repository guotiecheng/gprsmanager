#ifndef HAL_CX930XX_H
#define HAL_CX930XX_H

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

#define HAL_CX930XX_NAME									"CX930XX   "

#define HAL_CX930XX_MAX_SLOT								32
#define HAL_CX930XX_MAX_DATA_PORT							1

// == Notification ID ==
#define HAL_CX930XX_NOTIFY_REQ_DETACH						1
#define HAL_CX930XX_NOTIFY_FORCED_DETACH					2
#define HAL_CX930XX_NOTIFY_RING								3

// == Request ID ==
#define HAL_CX930XX_REQ_RESET								1
#define HAL_CX930XX_REQ_DETACH								2
#define HAL_CX930XX_REQ_OPEN_DATA_PORT						3
#define HAL_CX930XX_REQ_CLOSE_DATA_PORT						4
#define HAL_CX930XX_REQ_DATA_FD								5
#define HAL_CX930XX_REQ_DTR									6
#define HAL_CX930XX_REQ_GET_DATA_PORT_FILE					7

// == Log ID ==
#define HAL_CX930XX_LOG_DATA_PORT_FAILURE_CODE				1
#define HAL_CX930XX_LOG_POWER_ON_FAILURE_CODE				2
#define HAL_CX930XX_LOG_POWERING_ON_CODE					3
#define HAL_CX930XX_LOG_POWERED_ON_CODE						4
#define HAL_CX930XX_LOG_POWERING_OFF_CODE					5
#define HAL_CX930XX_LOG_POWERED_OFF_CODE					6
#define HAL_CX930XX_LOG_RESETTING_CODE						7
#define HAL_CX930XX_LOG_RESET_CODE							8
#define HAL_CX930XX_LOG_TERMINATING_CODE					9
#define HAL_CX930XX_LOG_TERMINATED_CODE						10
#define HAL_CX930XX_LOG_DATA_PORT_RAW_CODE					11
#define HAL_CX930XX_LOG_AT_CMD_CODE							12
#define HAL_CX930XX_LOG_DTR_CODE							13

#define HAL_CX930XX_DATA_PORT_MODE_OPEN						0x01
#define HAL_CX930XX_DATA_PORT_MODE_CLOSE					0x02

// Factory settings
#define HAL_CX930XX_FAC_BAUDRATE							115200
#define HAL_CX930XX_FAC_DATA_BIT							8
#define HAL_CX930XX_FAC_STOP_BIT							1
#define HAL_CX930XX_FAC_PARITY								0
#define HAL_CX930XX_FAC_FLOW_CTRL							0
#define HAL_CX930XX_FAC_FRAG_SIZE							64

// Default settings
#define HAL_CX930XX_DEF_BAUDRATE							115200
#define HAL_CX930XX_DEF_DATA_BIT							8
#define HAL_CX930XX_DEF_STOP_BIT							1
#define HAL_CX930XX_DEF_PARITY								0
#define HAL_CX930XX_DEF_FLOW_CTRL							0
#define HAL_CX930XX_DEF_FRAG_SIZE							64


// == EXTERNAL FUNCTIONS ==
typedef unsigned char (*HAL_CX930XX_NOTIFY) (void *p_pvPtr, int p_iNotifyID, unsigned char p_ucRet, ...);
typedef unsigned char (*HAL_CX930XX_PROC_TOKEN) (void *p_pvPtr);


typedef struct HAL_CX930XX_SESSION_T
{
	unsigned char			ucPriority;

	int						iSID;

	HAL_CX930XX_NOTIFY		pf_notify;

	void					*pvPtr;
}
HAL_CX930XX_SESSION;

typedef struct HAL_CX930XX_DATA_PORT_T
{
	unsigned char			ucIndex;

	COM_PORT_VOICE				stPort;

	HAL_CX930XX_SESSION		stSession;
}
HAL_CX930XX_DATA_PORT;

typedef struct HAL_CX930XX_SLOT_T
{
	unsigned char			ucPorts,
							ucReady,
							ucReset,
							ucStatus;

	char					acCountryReg [3];

	COM_PORT_VOICE				stCtrlPort;

	HAL_CX930XX_DATA_PORT	astDataPort [HAL_CX930XX_MAX_DATA_PORT];

	CRITICAL_LOCK			stLock;
}
HAL_CX930XX_SLOT;

typedef struct HAL_CX930XX_T
{
	unsigned char			ucInit;

	int						iSeqNo;

	ARRAY_LIST				stSlots;	// HAL_CX930XX_SLOT

	WRK_THREAD				stThread;
}
HAL_CX930XX;

// == PRIVATE FUNCTIONS ==
void hal_cx930xx_body (void *p_pvThread);
void hal_cx930xx_log (HAL_CX930XX *p_pstHAL, int p_iLogID, unsigned char p_ucRet, ...);
unsigned char hal_cx930xx_proc_token (void *p_pstHAL);

// == PUBLIC FUNCTIONS ==
unsigned char hal_cx930xx_init (HAL_CX930XX *p_pstHAL, int p_iSize);
void hal_cx930xx_term (HAL_CX930XX *p_pstHAL);

unsigned char hal_cx930xx_add_slot (HAL_CX930XX *p_pstHAL, char *p_pcSlot, char *p_pcCtrlDrv, char *p_pcDataDrv, char *p_pcCountryReg);

unsigned char hal_cx930xx_start (HAL_CX930XX *p_pstHAL);
unsigned char hal_cx930xx_close (HAL_CX930XX *p_pstHAL);

unsigned char hal_cx930xx_attach (HAL_CX930XX *p_pstHAL, char *p_pcSlot, unsigned char p_ucPort, unsigned char p_ucPriority, unsigned char p_ucInterrupt,
								 HAL_CX930XX_NOTIFY p_pf_notify, void *p_pvPtr, int *p_piSessionID);
unsigned char hal_cx930xx_detach (HAL_CX930XX *p_pstHAL, char *p_pcSlot, int p_iSID);
unsigned char hal_cx930xx_request (HAL_CX930XX *p_pstHAL, char *p_pcSlot, int p_iSID, unsigned char p_ucOpID, ...);

unsigned char hal_cx930xx_at_cmd (HAL_CX930XX *p_pstHAL, char *p_pcSlot, int p_iSID, char *p_pcReq, char *p_pcRes, char *p_pcErr, char *p_pcRet, int p_iRetSize,
								  unsigned long p_ulDelay, unsigned long p_ulGap, unsigned long p_ulTimeout, HAL_CX930XX_PROC_TOKEN p_pf_proc_token, void *p_pvPtr, unsigned int p_uiLine, char *p_pcName);

unsigned char hal_cx930xx_send (HAL_CX930XX *p_pstHAL, char *p_pcSlot, int p_iSID, unsigned char *p_pucData, int p_iLen);
unsigned char hal_cx930xx_recv (HAL_CX930XX *p_pstHAL, char *p_pcSlot, int p_iSID, unsigned char *p_pucData, int p_iSize, int *p_piLen, unsigned long p_ulTimeout);

// == INTERNAL FUNCTIONS ==
unsigned char hal_cx930xx_switch_on (HAL_CX930XX *p_pstHAL, HAL_CX930XX_SLOT *p_pstSlot);
unsigned char hal_cx930xx_switch_off (HAL_CX930XX *p_pstHAL, HAL_CX930XX_SLOT *p_pstSlot);

unsigned char hal_cx930xx_get_ready (HAL_CX930XX *p_pstHAL, HAL_CX930XX_SLOT *p_pstSlot);

unsigned char hal_cx930xx_at_cmd_ex (HAL_CX930XX *p_pstHAL, HAL_CX930XX_SLOT *p_pstSlot, int p_iSID, char *p_pcReq, char *p_pcRes, char *p_pcErr, char *p_pcRet, int p_iRetSize,
									 unsigned long p_ulDelay, unsigned long p_ulGap, unsigned long p_ulTimeout, HAL_CX930XX_PROC_TOKEN p_pf_proc_token, void *p_pvPtr, unsigned int p_uiLine, char *p_pcName);

void hal_cx930xx_power_on (HAL_CX930XX *p_pstHAL, HAL_CX930XX_SLOT *p_pstSlot);
void hal_cx930xx_power_off (HAL_CX930XX *p_pstHAL, HAL_CX930XX_SLOT *p_pstSlot);
void hal_cx930xx_reset (HAL_CX930XX *p_pstHAL, HAL_CX930XX_SLOT *p_pstSlot);
unsigned char hal_cx930xx_ring (HAL_CX930XX *p_pstHAL, HAL_CX930XX_SLOT *p_pstSlot);
void hal_cx930xx_dtr (HAL_CX930XX *p_pstHAL, HAL_CX930XX_SLOT *p_pstSlot, unsigned char p_ucFlag);

unsigned char hal_cx930xx_get_slot_by_sid (HAL_CX930XX *p_pstHAL, int p_iSID, HAL_CX930XX_SLOT **p_ppstSlot);

unsigned char hal_cx930xx_port (HAL_CX930XX *p_pstHAL, HAL_CX930XX_SLOT *p_pstSlot, unsigned char p_ucPort, unsigned char p_ucSettings, unsigned char p_ucMode);

void hal_cx930xx_send_delay (void *p_pvPtr, unsigned int len);

#endif
