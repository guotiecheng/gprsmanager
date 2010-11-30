#ifndef OPTM_H
#define OPTM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "../local.h"

// ===============
// ** CONSTANTS **
// ===============

#define OPTM_MAGIC_CODE								12345

#define OPTM_MAX_PARAM								8

#define OPTM_MAX_RECV_BUF							512
#define OPTM_MAX_SEND_BUF							512

#define OPTM_SUPER_ADMIN_PWD						"89983600"

// TIMING
#define OPTM_TIME_PROTO_RECV_TIMEOUT				10000
#define OPTM_TIME_HANDSHAKE_TIMEOUT					3000
#define OPTM_TIME_TEL_TIMEOUT						15000

// STATE
#define OPTM_PROC_STATE_NONE_CODE					0x00
#define OPTM_PROC_STATE_HANDSHAKE_CODE				0x01
#define OPTM_PROC_STATE_IDLE_CODE					0x02
#define OPTM_PROC_STATE_ADMIN_CODE					0x03
#define OPTM_PROC_STATE_AUTH_CODE					0x04
#define OPTM_PROC_STATE_SPEED_DIAL_CODE				0x05
#define OPTM_PROC_STATE_TEL_CODE					0x06
#define OPTM_PROC_STATE_POS_CODE					0x07
#define OPTM_PROC_STATE_FAX_CODE					0x08
#define OPTM_PROC_STATE_CLOSE_CODE					0x09
#define OPTM_PROC_STATE_TERM_CODE					0xFF

// RETURN STATUS
#define OPTM_RET_STATUS_READY						0x00
#define OPTM_RET_STATUS_FAILED						0x01
#define OPTM_RET_STATUS_CLOSE						0x02
#define OPTM_RET_STATUS_ERROR						0xFF

// FRAME FLAG
#define	OPTM_FRAME_STX_CODE							0x02
#define	OPTM_FRAME_ETX_CODE							0x03


// =================
// == FRAME CLASS ==
// =================

#define	OPTM_FRAME_CLS_NONE_CODE					0x00
#define	OPTM_FRAME_CLS_HANDSHAKE_CODE				0x01

#define	OPTM_FRAME_CLS_ADMIN_CODE					0x02
#define	OPTM_FRAME_CLS_SPEED_DIAL_CODE				0x07
#define	OPTM_FRAME_CLS_MCU_CFG_CODE					0x08
#define	OPTM_FRAME_CLS_AUTH_CODE					0x03

#define	OPTM_FRAME_CLS_EVT_TEL_CODE					0x04
#define	OPTM_FRAME_CLS_EVT_POS_CODE					0x05
#define	OPTM_FRAME_CLS_EVT_FAX_CODE					0x06
#define	OPTM_FRAME_CLS_CORRUPTED_CODE				0xF0


// ===================
// == FRAME COMMAND ==
// ===================

// == HANDSHAKE ==
#define OPTM_FRAME_HANDSHAKE_CMD_EXCHG_PARAM_CODE	0x01

// == ADMIN ==
#define OPTM_FRAME_ADMIN_CMD_LOGIN_CODE				0x01
#define OPTM_FRAME_ADMIN_CMD_LOGOUT_CODE			0x02
#define OPTM_FRAME_ADMIN_CMD_CHG_PWD_CODE			0x03

// == SPEED DIAL ==
#define OPTM_FRAME_SPEED_DIAL_CMD_ADD_CODE			0x01
#define OPTM_FRAME_SPEED_DIAL_CMD_DEL_CODE			0x02

// == MCU CONFIGURATION ==
#define OPTM_FRAME_MCU_CFG_CMD_KEY_GAP_CODE			0x01
#define OPTM_FRAME_MCU_CFG_CMD_ENABLE_PORT_CODE		0x02
#define OPTM_FRAME_MCU_CFG_CMD_DISABLE_PORT_CODE	0x03

// == AUTH ==
#define OPTM_FRAME_AUTH_CMD_AUTH_PWD_CODE			0x01
#define OPTM_FRAME_AUTH_CMD_ALT_AUTH_PWD_CODE		0x02
#define OPTM_FRAME_AUTH_CMD_ENABLE_AUTH_PWD_CODE	0x03
#define OPTM_FRAME_AUTH_CMD_DISABLE_AUTH_PWD_CODE	0x04

// == TEL ==
#define OPTM_FRAME_TEL_CMD_MCU_DIALING_CODE			0x01
#define OPTM_FRAME_TEL_CMD_MCU_DIAL_NO_CODE			0x02
#define OPTM_FRAME_TEL_CMD_MCU_HANG_UP_CODE			0x03
#define OPTM_FRAME_TEL_CMD_MCU_CONNECT_CODE			0x04

#define OPTM_FRAME_TEL_CMD_GSM_RINGING_CODE			0x01
#define OPTM_FRAME_TEL_CMD_GSM_HANG_UP_CODE			0x03
#define OPTM_FRAME_TEL_CMD_GSM_CONNECT_CODE			0x04

// == POS ==
#define OPTM_FRAME_POS_CMD_MCU_DIALING_CODE			0x01
#define OPTM_FRAME_POS_CMD_MCU_DIAL_NO_CODE			0x02
#define OPTM_FRAME_POS_CMD_MCU_HANG_UP_CODE			0x03


// ==================
// == FRAME STATUS ==
// ==================

#define	OPTM_FRAME_STA_ACK_CODE						0xA0
#define	OPTM_FRAME_STA_NAK_CODE						0xA1


// ===============
// == TEL EVENT ==
// ===============

#define OPTM_FRAME_TEL_EVT_TIMEOUT_CODE				0x01
#define OPTM_FRAME_TEL_EVT_ERROR_CODE				0x02
#define OPTM_FRAME_TEL_EVT_FAILED_CODE				0x03

#define OPTM_FRAME_TEL_EVT_MCU_DAILING_CODE			0x10
#define OPTM_FRAME_TEL_EVT_MCU_DIAL_NO_CODE			0x11
#define OPTM_FRAME_TEL_EVT_MCU_HANG_UP_CODE			0x12
#define OPTM_FRAME_TEL_EVT_MCU_CONNECT_CODE			0x13
#define OPTM_FRAME_TEL_EVT_MCU_RINGING_CODE			0x14

#define OPTM_FRAME_TEL_EVT_GSM_HANG_UP_CODE			0x22
#define OPTM_FRAME_TEL_EVT_GSM_CONNECT_CODE			0x23


// ===============
// == POS EVENT ==
// ===============

#define OPTM_FRAME_POS_EVT_TIMEOUT_CODE				0x01
#define OPTM_FRAME_POS_EVT_ERROR_CODE				0x02
#define OPTM_FRAME_POS_EVT_FAILED_CODE				0x03

#define OPTM_FRAME_POS_EVT_MCU_DAILING_CODE			0x10
#define OPTM_FRAME_POS_EVT_MCU_DIAL_NO_CODE			0x11
#define OPTM_FRAME_POS_EVT_MCU_HANG_UP_CODE			0x12


// ===============
// == LOG EVENT ==
// ===============

#define OPTM_LOG_FRAME_CORRUPTED_CODE				0x01
#define OPTM_LOG_FRAME_UNPROCESS_CODE				0x02
#define OPTM_LOG_PROC_FAILED_CODE					0x03
#define OPTM_LOG_PROC_DEAD_LOOP_CODE				0x04
#define OPTM_LOG_PARSE_FAILED_CODE					0x05
#define OPTM_LOG_BUILD_FAILED_CODE					0x06
#define OPTM_LOG_SEND_FAILED_CODE					0x07
#define OPTM_LOG_RECV_FAILED_CODE					0x08
#define OPTM_LOG_RECV_TIMEOUT_CODE					0x09

#define OPTM_LOG_HANDSHAKE_SUCCESS_CODE				0x10
#define OPTM_LOG_HANDSHAKE_ERROR_CODE				0x11
#define OPTM_LOG_HANDSHAKE_TIMEOUT_CODE				0x12

#define OPTM_LOG_ADMIN_LOGIN_SUCCESS_CODE			0x20
#define OPTM_LOG_ADMIN_LOGIN_REJECT_CODE			0x21
#define OPTM_LOG_ADMIN_LOGIN_ERROR_CODE				0x22
#define OPTM_LOG_ADMIN_LOGOUT_SUCCESS_CODE			0x23
#define OPTM_LOG_ADMIN_LOGOUT_REJECT_CODE			0x24
#define OPTM_LOG_ADMIN_LOGOUT_ERROR_CODE			0x25
#define OPTM_LOG_ADMIN_CHG_PWD_SUCCESS_CODE			0x26
#define OPTM_LOG_ADMIN_CHG_PWD_REJECT_CODE			0x27
#define OPTM_LOG_ADMIN_CHG_PWD_ERROR_CODE			0x28

#define OPTM_LOG_AUTH_SUCCESS_CODE					0x30
#define OPTM_LOG_AUTH_REJECT_CODE					0x31
#define OPTM_LOG_AUTH_ERROR_CODE					0x32
#define OPTM_LOG_ALT_AUTH_SUCCESS_CODE				0x33
#define OPTM_LOG_ALT_AUTH_REJECT_CODE				0x34
#define OPTM_LOG_ALT_AUTH_ERROR_CODE				0x35

#define OPTM_LOG_SPEED_DIAL_ADD_SUCCESS_CODE		0x41
#define OPTM_LOG_SPEED_DIAL_ADD_REJECT_CODE			0x42
#define OPTM_LOG_SPEED_DIAL_ADD_ERROR_CODE			0x43
#define OPTM_LOG_SPEED_DIAL_DEL_SUCCESS_CODE		0x44
#define OPTM_LOG_SPEED_DIAL_DEL_REJECT_CODE			0x45
#define OPTM_LOG_SPEED_DIAL_DEL_ERROR_CODE			0x46

#define OPTM_LOG_TEL_EVENT_TIMEOUT_CODE				0x40


// ========================
// ** EXTERNAL FUNCTIONS **
// ========================

typedef unsigned char (*OPTM_RAW_RECV)      (void *p_pvOPTM, unsigned char *p_pucRecv, unsigned int p_uiRecvSize, unsigned int *p_puiRecvLen);

typedef unsigned char (*OPTM_CHK_SEND)      (void *p_pvOPTM);
typedef unsigned char (*OPTM_RAW_SEND)      (void *p_pvOPTM, unsigned char *p_pucSend, unsigned int p_uiSendSize, unsigned int *p_puiSendLen);

typedef unsigned char (*OPTM_LOGEVENT)      (void *p_pvOPTM, unsigned int p_uiLogEvent, unsigned int p_uiLine, unsigned char p_ucRet, ...);

typedef unsigned char (*OPTM_HANDSHAKE)     (void *p_pvOPTM);
typedef unsigned char (*OPTM_ADMIN_LOGIN)   (void *p_pvOPTM, unsigned char *p_pucPwd, int p_iLen);
typedef unsigned char (*OPTM_ADMIN_LOGOUT)  (void *p_pvOPTM);
typedef unsigned char (*OPTM_ADMIN_CHG_PWD) (void *p_pvOPTM, unsigned char *p_pucPwd, int p_iLen);

typedef unsigned char (*OPTM_AUTH_PWD)      (void *p_pvOPTM, unsigned char *p_pucPwd, int p_iLen);
typedef unsigned char (*OPTM_ALT_AUTH_PWD)  (void *p_pvOPTM, unsigned char *p_pucNewPwd, int p_iNewLen);

typedef unsigned char (*OPTM_SPEED_DIAL_ADD)(void *p_pvOPTM, unsigned char p_ucIndex, unsigned char *p_pucCallerID, int p_iLen);
typedef unsigned char (*OPTM_SPEED_DIAL_DEL)(void *p_pvOPTM, unsigned char p_ucIndex);

typedef unsigned char (*OPTM_TEL_EVENT)     (void *p_pvOPTM, unsigned char p_ucEventID, unsigned int p_uiLine, unsigned char p_ucRet, ...);
typedef unsigned char (*OPTM_POS_EVENT)     (void *p_pvOPTM, unsigned char p_ucEventID, unsigned int p_uiLine, unsigned char p_ucRet, ...);

typedef void *        (*OPTM_MALLOC)        (size_t p_stSize);
typedef void          (*OPTM_FREE)          (void *p_pvMem);
typedef void          (*OPTM_MEMSET)        (void *p_pvDst, int p_iVal, size_t p_stSize);
typedef void          (*OPTM_MEMCPY)        (void *p_pvDst, const void *p_pvSrc, size_t p_stSize);
typedef void          (*OPTM_MEMMOVE)       (void *p_pvDst, const void *p_pvSrc, size_t p_stSize);
typedef int           (*OPTM_MEMCMP)        (void *p_pvDst, const void *p_pvSrc, size_t p_stSize);
typedef void          (*OPTM_SLEEP)         (unsigned long p_ulMillisecond);
typedef void          (*OPTM_GETTICK)       (unsigned long *p_pulTick);


typedef struct OPTM_PARAM_ITEM_T
{
	unsigned char		ucIsUsed,
						ucCode,
						*pucData;

	int					iDataLen;
}
OPTM_PARAM_ITEM;

typedef struct OPTM_PARAM_T
{
	int					iCount;

	OPTM_PARAM_ITEM		astItem [OPTM_MAX_PARAM];
}
OPTM_PARAM;

typedef struct OPTM_FRAME_T
{
	unsigned char		ucClass,
						ucCmd_Status;

	OPTM_PARAM			stParam;
}
OPTM_FRAME;

typedef struct OPTM_CACHE_T
{
	OPTM_FRAME			stFrame;

	unsigned long		ulLastOpTime;

	unsigned char		ucProcState,
						ucPending;
}
OPTM_CACHE;

typedef struct OPTM_T
{
	unsigned char			ucProcState,
							ucLoopState,
							ucHandshake,
							ucAuth,
							ucAdmin,
							aucRecvBuf [OPTM_MAX_RECV_BUF],
							aucSendBuf [OPTM_MAX_SEND_BUF];

	unsigned long			ulLastRecv,
							ulLastOpTime;

	unsigned int			uiRecvBufLen,
							uiSendBufLen;

	int						iMagic,
							iRetry;

	OPTM_FRAME				stSendFrame,
							stRecvFrame;
						
	OPTM_CACHE				stSendCache,
							stSendEvent;

	// =======================
	// ** External function **
	// =======================

	OPTM_RAW_RECV			pf_raw_recv;

	OPTM_CHK_SEND			pf_chk_send;
	OPTM_RAW_SEND			pf_raw_send;

	OPTM_HANDSHAKE			pf_handshake;

	OPTM_ADMIN_LOGIN		pf_admin_login;
	OPTM_ADMIN_LOGOUT		pf_admin_logout;
	OPTM_ADMIN_CHG_PWD		pf_admin_chg_pwd;

	OPTM_AUTH_PWD			pf_auth_pwd;
	OPTM_ALT_AUTH_PWD		pf_alt_auth_pwd;

	OPTM_SPEED_DIAL_ADD		pf_speed_dial_add;
	OPTM_SPEED_DIAL_DEL		pf_speed_dial_del;

	OPTM_TEL_EVENT			pf_tel_event;
	OPTM_POS_EVENT			pf_pos_event;

	OPTM_MALLOC				pf_malloc;
	OPTM_FREE				pf_free;
	OPTM_MEMSET				pf_memset;
	OPTM_MEMCPY				pf_memcpy;
	OPTM_MEMMOVE			pf_memmove;
	OPTM_MEMCMP				pf_memcmp;

	OPTM_LOGEVENT			pf_log_event;

	OPTM_SLEEP				pf_sleep;
	OPTM_GETTICK			pf_get_tick;

	void					*pvBasket;
}
OPTM;


// =====================
// ** PUBLIC FUNCTION **
// =====================

unsigned char	optm_init  (OPTM *p_pstOPTM, void *p_pvBasket,
						    OPTM_RAW_RECV p_pf_raw_recv, OPTM_CHK_SEND p_pf_chk_send, OPTM_RAW_SEND p_pf_raw_send, OPTM_LOGEVENT p_pf_log_event,
						    OPTM_MALLOC p_pf_malloc, OPTM_FREE p_pf_free, OPTM_MEMSET p_pf_memset, OPTM_MEMCPY p_pf_memcpy,
						    OPTM_MEMMOVE p_pf_memmove, OPTM_MEMCMP p_pf_memcmp, OPTM_SLEEP p_pf_sleep, OPTM_GETTICK p_pf_get_tick,
							OPTM_HANDSHAKE p_pf_handshake,
							OPTM_ADMIN_LOGIN p_pf_admin_login, OPTM_ADMIN_LOGOUT p_pf_admin_logout, OPTM_ADMIN_CHG_PWD p_pf_admin_chg_pwd,
						    OPTM_AUTH_PWD p_pf_auth_pwd, OPTM_ALT_AUTH_PWD p_pf_alt_auth_pwd,
						    OPTM_SPEED_DIAL_ADD p_pf_speed_dial_add, OPTM_SPEED_DIAL_DEL p_pf_speed_dial_del,
						    OPTM_TEL_EVENT p_pf_tel_event, OPTM_POS_EVENT p_pf_pos_event);
void			optm_term  (OPTM *p_pstOPTM);
unsigned char	optm_main  (OPTM *p_pstOPTM);

unsigned char	optm_event (OPTM *p_pstOPTM, OPTM_FRAME *p_pstFrame);


// ======================
// ** PRIVATE FUNCTION **
// ======================

unsigned char	optm_proc_handshake (OPTM *p_pstOPTM);
unsigned char	optm_proc_idle (OPTM *p_pstOPTM);

unsigned char	optm_proc_admin (OPTM *p_pstOPTM);
unsigned char	optm_proc_auth (OPTM *p_pstOPTM);
unsigned char	optm_proc_speed_dial (OPTM *p_pstOPTM);

unsigned char	optm_proc_tel (OPTM *p_pstOPTM);
unsigned char	optm_proc_pos (OPTM *p_pstOPTM);
unsigned char	optm_proc_close (OPTM *p_pstOPTM);

unsigned char	optm_parse (OPTM *p_pstOPTM, int *p_puiUsedLen);
unsigned char	optm_build (OPTM *p_pstOPTM, unsigned char *p_pucBuf, unsigned int p_uiBufSize, unsigned int *p_puiBufLen);

unsigned char	optm_gen_lrc (OPTM *p_pstOPTM, unsigned char *p_pucData, unsigned int p_uiDataLen);

void			optm_get_param (OPTM *p_pstOPTM, unsigned char p_ucCode, OPTM_PARAM *p_pstParam, OPTM_PARAM_ITEM **p_ppstItem);
void			optm_del_param (OPTM *p_pstOPTM, unsigned char p_ucCode, OPTM_PARAM *p_pstParam);

void			optm_del_frame (OPTM *p_pstOPTM, OPTM_FRAME *p_pstSrcFrame);
unsigned char	optm_dup_frame (OPTM *p_pstOPTM, OPTM_FRAME *p_pstSrcFrame, OPTM_FRAME *p_pstDestFrame);

#ifdef __cplusplus 
}
#endif 

#endif
