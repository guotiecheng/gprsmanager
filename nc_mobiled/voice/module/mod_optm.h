#ifndef MOD_OPTM_H
#define MOD_OPTM_H

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

#include "../config/config.h"
#include "../local.h"

#include "optm.h"
//#include "hal/arch.h"

#define MOD_OPTM_NAME							"OPTIMUS   "
#define MOD_OPTM_PROTO_NAME						"OPTM      "

#define MOD_OPTM_PORT_PORT						"/dev/ttyS3"
#define MOD_OPTM_PORT_BAUDRATE					38400
#define MOD_OPTM_PORT_DATA_BIT					8
#define MOD_OPTM_PORT_STOP_BIT					1
#define MOD_OPTM_PORT_PARITY					0
#define MOD_OPTM_PORT_FLOW_CTRL					0

#if defined (WIN32)

	#define MOD_OPTM_FILE						"C:\\optimus.pwd"

#else

	#define MOD_OPTM_FILE						"/apps/etc/optimus.pwd"

#endif

// ==================
// == MASTER STATE ==
// ==================

#define MOD_OPTM_MASTER_STATE_IDLE				0x01
#define MOD_OPTM_MASTER_STATE_ONLINE			0x02
#define MOD_OPTM_MASTER_STATE_HANG_UP			0x03


// ==================
// == HELPER STATE ==
// ==================

#define MOD_OPTM_HELPER_STATE_IDLE				0x01
#define MOD_OPTM_HELPER_STATE_DIALING			0x02
#define MOD_OPTM_HELPER_STATE_ANSWER			0x03
#define MOD_OPTM_HELPER_STATE_ONLINE			0x04
#define MOD_OPTM_HELPER_STATE_HANG_UP			0x05
#define MOD_OPTM_HELPER_STATE_COOLDOWN			0x06


// ======================
// == MASTER TEL EVENT ==
// ======================

#define MOD_OPTM_MASTER_TEL_EVT_NONE			0x00
#define MOD_OPTM_MASTER_TEL_EVT_MCU_DIALING		0x10
#define MOD_OPTM_MASTER_TEL_EVT_MCU_DIAL_NO		0x11
#define MOD_OPTM_MASTER_TEL_EVT_MCU_HANG_UP		0x12
#define MOD_OPTM_MASTER_TEL_EVT_MCU_CONNECT		0x13
#define MOD_OPTM_MASTER_TEL_EVT_MCU_RINGING		0x14

#define MOD_OPTM_MASTER_TEL_EVT_GSM_CONNECT		0x15
#define MOD_OPTM_MASTER_TEL_EVT_GSM_HANG_UP		0x16


// ======================
// == HELPER TEL EVENT ==
// ======================

#define MOD_OPTM_HELPER_TEL_EVT_NONE			0x00
#define MOD_OPTM_HELPER_TEL_EVT_GSM_RINGING		0x20
#define MOD_OPTM_HELPER_TEL_EVT_GSM_CONNECT		0x21
#define MOD_OPTM_HELPER_TEL_EVT_GSM_HANG_UP		0x22

#define MOD_OPTM_HELPER_TEL_EVT_MCU_DIALING		0x23
#define MOD_OPTM_HELPER_TEL_EVT_MCU_DIAL_NO		0x24
#define MOD_OPTM_HELPER_TEL_EVT_MCU_HANG_UP		0x25
#define MOD_OPTM_HELPER_TEL_EVT_MCU_CONNECT		0x26


// =================
// == EVENT FLAGS ==
// =================

#define MOD_OPTM_EVT_FLAG_RINGING				0x01
#define MOD_OPTM_EVT_FLAG_DIAL_NO				0x02
#define MOD_OPTM_EVT_FLAG_OFF_HOOK				0x04
#define MOD_OPTM_EVT_FLAG_CONNECT				0x08
#define MOD_OPTM_EVT_FLAG_HANG_UP				0x10


typedef struct MOD_OPTM_AT_CMD_T
{
	char				acReq [64],
						acRes [64],
						acErr [64];

	unsigned long		ulGap,
						ulTimeout;

	int					iRetry;
}
MOD_OPTM_AT_CMD;

typedef struct MOD_OPTM_MASTER_T
{
	unsigned char		ucEvent,
						ucState,

						ucTEL_State,
						ucPOS_State;

	char				acTEL_CallID [64],
						acPOS_CallID [64];

	int					iPOS_Slot;

	COM_PORT_VOICE			stPort;

	OPTM				stOPTM;

	WRK_THREAD			stThread;
}
MOD_OPTM_MASTER;

typedef struct MOD_OPTM_HELPER_T
{
	unsigned char		ucEvent,
						ucState,
						ucEvtFlag,
						ucDetach;

	unsigned long		ulLastRING,
						ulLastCall;
//						ulLastSignal;

	char				acCallID [64],
						acRecvBuf [32];

	int					iP1_SID,
              iP2_SID,
						iRecvBufLen,
						iRecvBufSize;

	WRK_THREAD			stThread;
}
MOD_OPTM_HELPER;

typedef struct MOD_OPTM_T
{
	unsigned char		ucMoreHex;

	char				acHex [SYS_LOG_MAX + 1];

	MOD_OPTM_MASTER		stMaster;

	MOD_OPTM_HELPER		stHelper;

	CFG_MOD_OPTM		*pstCfg;
}
MOD_OPTM;


void			mod_optm_catch_event (MOD_OPTM *p_pstOPTM);

unsigned char	mod_optm_ring (MOD_OPTM *p_pstOPTM);
unsigned char	mod_optm_off_hook (MOD_OPTM *p_pstOPTM);
unsigned char	mod_optm_dial (MOD_OPTM *p_pstOPTM);
unsigned char	mod_optm_answer (MOD_OPTM *p_pstOPTM);
unsigned char	mod_optm_online (MOD_OPTM *p_pstOPTM);
unsigned char	mod_optm_hangup (MOD_OPTM *p_pstOPTM);
//unsigned char mod_optm_signal (MOD_OPTM *p_pstOPTM);






void mod_optm_init (MOD_OPTM *p_pstOPTM);
void mod_optm_term (MOD_OPTM *p_pstOPTM);
unsigned char mod_optm_start (MOD_OPTM *p_pstOPTM);

unsigned char mod_optm_get_pstn_call_id (MOD_OPTM *p_pstOPTM, unsigned char p_ucChannel, char *p_pcBuf, int p_iBufSize, unsigned char *p_pucSlot);

// ** INTERNAL FUNCTIONS **
void mod_optm_master_body (void *p_pvWrk);
void mod_optm_helper_body (void *p_pvWrk);

unsigned char mod_optm_attach (MOD_OPTM *p_pstOPTM);
unsigned char mod_optm_detach (MOD_OPTM *p_pstOPTM);

unsigned char mod_optm_at_cmd (MOD_OPTM *p_pstOPTM, int p_iSID, char *p_pcReq, char *p_pcRes, char *p_pcErr, char *p_pcRet, int p_iRetSize,
							   unsigned long p_ulDelay, unsigned long p_ulGap, unsigned long p_ulTimeout, unsigned int p_uiLine);

unsigned char mod_optm_echo_ctrl (MOD_OPTM *p_pstOPTM, int p_iSID);
unsigned char mod_optm_get_mobile_call_id (MOD_OPTM *p_pstOPTM, int p_iSID, char *p_pcBuf, int p_iBufSize);

// ** CALLBACK FUNCTIONS **
unsigned char mod_optm_notify (void *p_pvPtr, int p_iNotifyID, unsigned char p_ucRet, ...);

unsigned char mod_optm_raw_recv (void *p_pvOPTM, unsigned char *p_pucRecv, unsigned int p_uiRecvSize, unsigned int *p_puiRecvLen);
unsigned char mod_optm_raw_send (void *p_pvOPTM, unsigned char *p_pucSend, unsigned int p_uiSendSize, unsigned int *p_puiSendLen);
unsigned char mod_optm_log (void *p_pvOPTM, unsigned int p_uiLogEvent, unsigned int p_uiLine, unsigned char p_ucRet, ...);

unsigned char mod_optm_handshake (void *p_pvOPTM);

unsigned char mod_optm_admin_login (void *p_pvOPTM, unsigned char *p_pucPwd, int p_iLen);
unsigned char mod_optm_admin_logout (void *p_pvOPTM);
unsigned char mod_optm_admin_chg_pwd (void *p_pvOPTM, unsigned char *p_pucPwd, int p_iLen);

unsigned char mod_optm_auth_pwd (void *p_pvOPTM, unsigned char *p_pucPwd, int p_iLen);
unsigned char mod_optm_alt_auth_pwd (void *p_pvOPTM, unsigned char *p_pucNewPwd, int p_iNewLen);

unsigned char mod_optm_speed_dial_add (void *p_pvOPTM, unsigned char p_ucIndex, unsigned char *p_pucCallerID, int p_iLen);
unsigned char mod_optm_speed_dial_del (void *p_pvOPTM, unsigned char p_ucIndex);

unsigned char mod_optm_tel_event (void *p_pvOPTM, unsigned char p_ucEventID, unsigned int p_uiLine, unsigned char p_ucRet, ...);
unsigned char mod_optm_pos_event (void *p_pvOPTM, unsigned char p_ucEventID, unsigned int p_uiLine, unsigned char p_ucRet, ...);

unsigned char mod_optm_read_file (void *p_pvOPTM, char *p_pcName, char *p_pcValue, int p_iSize);
unsigned char mod_optm_edit_file (void *p_pvOPTM, char *p_pcName, char *p_pcValue);
unsigned char mod_optm_script (void *p_pvOPTM, char *p_pcScript, char *p_pacArgv [], unsigned long p_ulTimeout, unsigned char p_ucForced);

void *mod_optm_malloc (size_t p_stSize);
void mod_optm_free (void *p_pvMem);
void mod_optm_memset (void *p_pvDst, int p_iVal, size_t p_stSize);
void mod_optm_memcpy (void *p_pvDst, const void *p_pvSrc, size_t p_stSize);
void mod_optm_memmove (void *p_pvDst, const void *p_pvSrc, size_t p_stSize);
int mod_optm_memcmp (void *p_pvDst, const void *p_pvSrc, size_t p_stSize);
void mod_optm_sleep (unsigned long p_ulMillisecond);
void mod_optm_get_tick (unsigned long *p_pulTick);

#endif
