#ifndef HAL_UC864E_H
#define HAL_UC864E_H

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

#define HAL_UC864E_NAME										"UC864E    "

#define HAL_UC864E_MAX_DATA_PORT							2

// == Notification ID ==
#define HAL_UC864E_NOTIFY_REQ_DETACH						1
#define HAL_UC864E_NOTIFY_FORCED_DETACH						2
#define HAL_UC864E_NOTIFY_SIM								3
#define HAL_UC864E_NOTIFY_SET_NETWORK_MODE		4

// == Request ID ==
#define HAL_UC864E_REQ_CHG_SIM								1
#define HAL_UC864E_REQ_RESET								2
#define HAL_UC864E_REQ_DETACH								3
#define HAL_UC864E_REQ_OPEN_DATA_PORT						4
#define HAL_UC864E_REQ_CLOSE_DATA_PORT						5
#define HAL_UC864E_REQ_GET_DATA_PORT_FILE					6
#define HAL_UC864E_REQ_SET_NETWORK_MODE						7
#define HAL_UC864E_REQ_NETWORK_SIGNAL						8
#define HAL_UC864E_REQ_SET_SIM_AVAILABILITY					9

// == Log ID ==
#define HAL_UC864E_LOG_DATA_PORT_FAILURE_CODE				1
#define HAL_UC864E_LOG_SWITCH_SIM_CODE						2
#define HAL_UC864E_LOG_CHANGING_BAUDRATE_CODE				3
#define HAL_UC864E_LOG_CHANGED_BAUDRATE_CODE				4
#define HAL_UC864E_LOG_POWER_ON_FAILURE_CODE				5
#define HAL_UC864E_LOG_POWERING_ON_CODE						6
#define HAL_UC864E_LOG_POWERED_ON_CODE						7
#define HAL_UC864E_LOG_POWERING_OFF_CODE					8
#define HAL_UC864E_LOG_POWERED_OFF_CODE						9
#define HAL_UC864E_LOG_RESETTING_CODE						10
#define HAL_UC864E_LOG_RESET_CODE							11
#define HAL_UC864E_LOG_TERMINATING_CODE						12
#define HAL_UC864E_LOG_TERMINATED_CODE						13
#define HAL_UC864E_LOG_CHANGE_SIM_CODE						14
#define HAL_UC864E_LOG_DATA_PORT_RAW_CODE					15
#define HAL_UC864E_LOG_AT_CMD_CODE							16
#define HAL_UC864E_LOG_DTR_CODE								17

#define HAL_UC864E_DATA_PORT_MODE_OPEN						0x01
#define HAL_UC864E_DATA_PORT_MODE_CLOSE						0x02

#define HAL_UC864E_NETWORK_MODE_DEFAULT						HAL_UC864E_NETWORK_MODE_HSPDA
#define HAL_UC864E_NETWORK_MODE_GPRS						0x01
#define HAL_UC864E_NETWORK_MODE_HSPDA						0x02

// Factory settings
#define HAL_UC864E_FAC_BAUDRATE								115200
#define HAL_UC864E_FAC_DATA_BIT								8
#define HAL_UC864E_FAC_STOP_BIT								1
#define HAL_UC864E_FAC_PARITY								0
#define HAL_UC864E_FAC_FLOW_CTRL							0
#define HAL_UC864E_FAC_FRAG_SIZE							64

// Default settings
#define HAL_UC864E_DEF_BAUDRATE								115200
#define HAL_UC864E_DEF_DATA_BIT								8
#define HAL_UC864E_DEF_STOP_BIT								1
#define HAL_UC864E_DEF_PARITY								0
#define HAL_UC864E_DEF_FLOW_CTRL							0
#define HAL_UC864E_DEF_FRAG_SIZE							64


// == EXTERNAL FUNCTIONS ==
typedef unsigned char (*HAL_UC864E_NOTIFY) (void *p_pvPtr, int p_iNotifyID, unsigned char p_ucRet, ...);


typedef struct HAL_UC864E_SESSION_T
{
	unsigned char			ucPriority;

	int						iSID;

	HAL_UC864E_NOTIFY		pf_notify;

	void					*pvPtr;
}
HAL_UC864E_SESSION;

typedef struct HAL_UC864E_DATA_PORT_T
{
	unsigned char			ucIndex;

	COM_PORT_VOICE				stPort;

	HAL_UC864E_SESSION		stSession;
}
HAL_UC864E_DATA_PORT;

typedef struct HAL_UC864E_T
{
	unsigned char			ucInit,
							ucPorts,
							ucReady,
							ucReset,
							ucStatus,
							ucSignal,
							ucNetworkMode,

							ucSIM_1_Door,
							ucSIM_2_Door,

							ucCurSIM,
							ucGotSIM_1,
							ucGotSIM_2;

	int						iSeqNo;

	COM_PORT_VOICE				stCtrlPort;

	HAL_UC864E_DATA_PORT	astDataPort [HAL_UC864E_MAX_DATA_PORT];

	CRITICAL_LOCK			stLock;

	WRK_THREAD				stThread;
}
HAL_UC864E;


// == PUBLIC FUNCTIONS ==
unsigned char hal_uc864e_init (HAL_UC864E *p_pstHAL, unsigned char p_ucNetworkMode);
void hal_uc864e_term (HAL_UC864E *p_pstHAL);

unsigned char hal_uc864e_start (HAL_UC864E *p_pstHAL);
unsigned char hal_uc864e_close (HAL_UC864E *p_pstHAL);

unsigned char hal_uc864e_attach (HAL_UC864E *p_pstHAL, unsigned char p_ucPort, unsigned char p_ucPriority, unsigned char p_ucInterrupt,
								 HAL_UC864E_NOTIFY p_pf_notify, void *p_pvPtr, int *p_piSessionID);
unsigned char hal_uc864e_detach (HAL_UC864E *p_pstHAL, int p_iSID);
unsigned char hal_uc864e_request (HAL_UC864E *p_pstHAL, int p_iSID, unsigned char p_ucOpID, ...);

unsigned char hal_uc864e_at_cmd (HAL_UC864E *p_pstHAL, int p_iSID, char *p_pcReq, char *p_pcRes, char *p_pcErr, char *p_pcRet, int p_iRetSize,
								 unsigned long p_ulDelay, unsigned long p_ulGap, unsigned long p_ulTimeout, unsigned char *p_pucProcToken, unsigned int p_uiLine, char *p_pcName);

unsigned char hal_uc864e_send (HAL_UC864E *p_pstHAL, int p_iSID, unsigned char *p_pucData, int p_iLen);
unsigned char hal_uc864e_recv (HAL_UC864E *p_pstHAL, int p_iSID, unsigned char *p_pucData, int p_iSize, int *p_piLen, unsigned long p_ulTimeout);

void hal_uc864e_dtr (HAL_UC864E *p_pstHAL, unsigned char p_ucFlag);

// == INTERNAL FUNCTIONS ==
unsigned char hal_uc864e_switch_on (HAL_UC864E *p_pstHAL, unsigned char p_ucSim, unsigned char *p_pucProcToken);
unsigned char hal_uc864e_switch_off (HAL_UC864E *p_pstHAL, unsigned char *p_pucProcToken);

unsigned char hal_uc864e_get_ready (HAL_UC864E *p_pstHAL);
unsigned char hal_uc864e_signal (HAL_UC864E *p_pstHAL, int p_iSID, unsigned char p_ucPort, int *p_piSignal, unsigned char *p_pucProcToken, char *p_pcCaller);


void hal_uc864e_power_on (HAL_UC864E *p_pstHAL, unsigned char p_ucSim);
void hal_uc864e_power_off (HAL_UC864E *p_pstHAL);
void hal_uc864e_reset (HAL_UC864E *p_pstHAL, unsigned char p_ucSim);
unsigned char hal_uc864e_ring (HAL_UC864E *p_pstHAL);

unsigned char hal_uc864e_sim_door (HAL_UC864E *p_pstHAL, unsigned char p_ucSim);
unsigned char hal_uc864e_sim (HAL_UC864E *p_pstHAL, unsigned char p_ucSim);

unsigned char hal_uc864e_port (HAL_UC864E *p_pstHAL, unsigned char p_ucPort, unsigned char p_ucSettings, unsigned char p_ucMode);

void hal_uc864e_get_data_port_file (HAL_UC864E *p_pstHAL, unsigned char p_ucPort, char *p_pcDrvFile, int p_iSize);
void hal_uc864e_get_ctrl_port_file (HAL_UC864E *p_pstHAL, char *p_pcDrvFile, int p_iSize);

void hal_uc864e_body (void *p_pvThread);
void hal_uc864e_log (HAL_UC864E *p_pstHAL, int p_iLogID, unsigned char p_ucRet, ...);

#endif
