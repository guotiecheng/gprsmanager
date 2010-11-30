#ifndef HAL_GTM900B_H
#define HAL_GTM900B_H

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

#define HAL_GTM900B_NAME									"GTM900B   "

#define HAL_GTM900B_MAX_DATA_PORT							1

// == Notification ID ==
#define HAL_GTM900B_NOTIFY_REQ_DETACH						1
#define HAL_GTM900B_NOTIFY_FORCED_DETACH					2
#define HAL_GTM900B_NOTIFY_SIM								3

// == Request ID ==
#define HAL_GTM900B_REQ_CHG_SIM								1
#define HAL_GTM900B_REQ_RESET								2
#define HAL_GTM900B_REQ_DETACH								3
#define HAL_GTM900B_REQ_OPEN_DATA_PORT						4
#define HAL_GTM900B_REQ_CLOSE_DATA_PORT						5
#define HAL_GTM900B_REQ_GET_DATA_PORT_FILE					6
#define HAL_GTM900B_REQ_REG_FLAG							7
#define HAL_GTM900B_REQ_NETWORK_SIGNAL						8
#define HAL_GTM900B_REQ_SET_SIM_AVAILABILITY				9

// == Log ID ==
#define HAL_GTM900B_LOG_DATA_PORT_FAILURE_CODE				1
#define HAL_GTM900B_LOG_CURRENT_SIM_CODE					2
#define HAL_GTM900B_LOG_CHANGING_BAUDRATE_CODE				3
#define HAL_GTM900B_LOG_CHANGED_BAUDRATE_CODE				4
#define HAL_GTM900B_LOG_POWER_ON_FAILURE_CODE				5
#define HAL_GTM900B_LOG_POWERING_ON_CODE					6
#define HAL_GTM900B_LOG_POWERED_ON_CODE						7
#define HAL_GTM900B_LOG_POWERING_OFF_CODE					8
#define HAL_GTM900B_LOG_POWERED_OFF_CODE					9
#define HAL_GTM900B_LOG_RESETTING_CODE						10
#define HAL_GTM900B_LOG_RESET_CODE							11
#define HAL_GTM900B_LOG_CHANGE_SIM_CODE						12
#define HAL_GTM900B_LOG_DATA_PORT_RAW_CODE					13
#define HAL_GTM900B_LOG_DTR_CODE							14

// Factory settings
#define HAL_GTM900B_FAC_BAUDRATE							9600
#define HAL_GTM900B_FAC_DATA_BIT							8
#define HAL_GTM900B_FAC_STOP_BIT							1
#define HAL_GTM900B_FAC_PARITY								0
#define HAL_GTM900B_FAC_FLOW_CTRL							0
#define HAL_GTM900B_FAC_FRAG_SIZE							64

// Default settings
#define HAL_GTM900B_DEF_BAUDRATE							115200
#define HAL_GTM900B_DEF_DATA_BIT							8
#define HAL_GTM900B_DEF_STOP_BIT							1
#define HAL_GTM900B_DEF_PARITY								0
#define HAL_GTM900B_DEF_FLOW_CTRL							0
#define HAL_GTM900B_DEF_FRAG_SIZE							64


// == EXTERNAL FUNCTIONS ==
typedef unsigned char (*HAL_GTM900B_NOTIFY) (void *p_pvPtr, int p_iNotifyID, unsigned char p_ucRet, ...);


typedef struct HAL_GTM900B_SESSION_T
{
	unsigned char			ucPriority;

	int						iSID;

	HAL_GTM900B_NOTIFY		pf_notify;

	void					*pvPtr;
}
HAL_GTM900B_SESSION;

typedef struct HAL_GTM900B_DATA_PORT_T
{
	unsigned char			ucPort;

	COM_PORT_VOICE				stPort;

	HAL_GTM900B_SESSION		stSession;
}
HAL_GTM900B_DATA_PORT;

typedef struct HAL_GTM900B_T
{
	unsigned char			ucInit,
							ucReady,
							ucReset,
							ucStatus,
							
							ucSIM_1_Door,
							ucSIM_2_Door,

							ucCurSIM,
							ucGotSIM_1,
							ucGotSIM_2,

							ucIsReg;

	unsigned long			ulLastReg;

	int						iSeqNo;

	COM_PORT_VOICE				stCtrlPort;

	HAL_GTM900B_DATA_PORT	astDataPort [HAL_GTM900B_MAX_DATA_PORT];
	
	CRITICAL_LOCK			stLock;

	WRK_THREAD				stThread;
}
HAL_GTM900B;


// == PUBLIC FUNCTIONS ==
unsigned char hal_gtm900b_init (HAL_GTM900B *p_pstHAL);
void hal_gtm900b_term (HAL_GTM900B *p_pstHAL);

unsigned char hal_gtm900b_start (HAL_GTM900B *p_pstHAL);
unsigned char hal_gtm900b_close (HAL_GTM900B *p_pstHAL);

unsigned char hal_gtm900b_attach (HAL_GTM900B *p_pstHAL, unsigned char p_ucPort, unsigned char p_ucPriority, unsigned char p_ucInterrupt,
								  HAL_GTM900B_NOTIFY p_pf_notify, void *p_pvPtr, int *p_piSessionID);
unsigned char hal_gtm900b_detach (HAL_GTM900B *p_pstHAL, int p_iSessionID);
unsigned char hal_gtm900b_request (HAL_GTM900B *p_pstHAL, int p_iSessionID, unsigned char p_ucOpID, ...);

unsigned char hal_gtm900b_at_cmd (HAL_GTM900B *p_pstHAL, int p_iSessionID, char *p_pcReq, char *p_pcRes, char *p_pcErr, char *p_pcRet, int p_iRetSize,
								  unsigned long p_ulDelay, unsigned long p_ulGap, unsigned long p_ulTimeout, unsigned char *p_pucProcToken, unsigned int p_uiLine, char *p_pcName);

unsigned char hal_gtm900b_send (HAL_GTM900B *p_pstHAL, int p_iSessionID, unsigned char *p_pucData, int p_iLen);
unsigned char hal_gtm900b_recv (HAL_GTM900B *p_pstHAL, int p_iSessionID, unsigned char *p_pucData, int p_iSize, int *p_piLen, unsigned long p_ulTimeout);

void hal_gtm900b_dtr (HAL_GTM900B *p_pstHAL, unsigned char p_ucFlag);

// == INTERNAL FUNCTIONS ==
unsigned char hal_gtm900b_switch_on (HAL_GTM900B *p_pstHAL, unsigned char p_ucSim, unsigned char *p_pucProcToken);
unsigned char hal_gtm900b_switch_off (HAL_GTM900B *p_pstHAL, unsigned char *p_pucProcToken);

unsigned char hal_gtm900b_get_ready (HAL_GTM900B *p_pstHAL);

void hal_gtm900b_power_on (HAL_GTM900B *p_pstHAL);
void hal_gtm900b_power_off (HAL_GTM900B *p_pstHAL);
unsigned char hal_gtm900b_select_sim (HAL_GTM900B *p_pstHAL, unsigned char p_ucSim);
unsigned char hal_gtm900b_ring (HAL_GTM900B *p_pstHAL);

unsigned char hal_gtm900b_sim_door (HAL_GTM900B *p_pstHAL, unsigned char p_ucSim);
unsigned char hal_gtm900b_sim (HAL_GTM900B *p_pstHAL, unsigned char p_ucSim);

unsigned char hal_gtm900b_port (HAL_GTM900B *p_pstHAL, unsigned char p_ucPort, unsigned char p_ucSettings, unsigned char p_ucMode);

unsigned char hal_gtm900b_get_data_port_file (HAL_GTM900B *p_pstHAL, unsigned char p_ucPort, char *p_pcDrvFile, int p_iSize);
void hal_gtm900b_get_ctrl_port_file (HAL_GTM900B *p_pstHAL, char *p_pcDrvFile, int p_iSize);

void hal_gtm900b_body (void *p_pvThread);
void hal_gtm900b_log (HAL_GTM900B *p_pstHAL, int p_iLogID, unsigned char p_ucRet, ...);

#endif
