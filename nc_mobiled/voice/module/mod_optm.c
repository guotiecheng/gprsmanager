#include "mod_optm.h"
#include "../voice.h"

void mod_optm_init (MOD_OPTM *p_pstOPTM)
{
	if (0 == g_stCfg.stModules.stOPTM.acID [0])
	{
		logger_log (&g_stVoiceLog, LOG_NORMAL, "@%04d %s: Service is disabled", __LINE__, MOD_OPTM_NAME);
		p_pstOPTM->pstCfg = (void *) 0;
	}
	else
	{
		memset (p_pstOPTM, 0x00, sizeof (MOD_OPTM));

		p_pstOPTM->pstCfg = &g_stCfg.stModules.stOPTM;

		comport_init_voice (&p_pstOPTM->stMaster.stPort, MOD_OPTM_PORT_PORT, MOD_OPTM_PORT_BAUDRATE, MOD_OPTM_PORT_DATA_BIT, MOD_OPTM_PORT_STOP_BIT,
			MOD_OPTM_PORT_PARITY, MOD_OPTM_PORT_FLOW_CTRL, 0, 0, 0, 0);

		optm_init (&p_pstOPTM->stMaster.stOPTM, (void *) p_pstOPTM, mod_optm_raw_recv, 0, mod_optm_raw_send, mod_optm_log,
			mod_optm_malloc, mod_optm_free, mod_optm_memset, mod_optm_memcpy, mod_optm_memmove, mod_optm_memcmp, mod_optm_sleep, mod_optm_get_tick,
			mod_optm_handshake,
			mod_optm_admin_login, mod_optm_admin_logout, mod_optm_admin_chg_pwd,
			mod_optm_auth_pwd, mod_optm_alt_auth_pwd,
			mod_optm_speed_dial_add, mod_optm_speed_dial_del,
			mod_optm_tel_event, mod_optm_pos_event);

		thread_init (&p_pstOPTM->stMaster.stThread, (void *) p_pstOPTM);
		thread_init (&p_pstOPTM->stHelper.stThread, (void *) p_pstOPTM);
	}
}

void mod_optm_term (MOD_OPTM *p_pstOPTM)
{
	if (0 != p_pstOPTM->pstCfg)
	{
		logger_log (&g_stVoiceLog, LOG_NORMAL, "@%04d %s: Service stopping", __LINE__, MOD_OPTM_NAME);

		thread_close (&p_pstOPTM->stHelper.stThread, SIGINT, 15000);
		thread_close (&p_pstOPTM->stMaster.stThread, SIGINT, 15000);

		optm_term (&p_pstOPTM->stMaster.stOPTM);

		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Service stopped", __LINE__, MOD_OPTM_NAME);
	}
}

unsigned char mod_optm_start (MOD_OPTM *p_pstOPTM)
{
	unsigned char	ucRet,
					ucStarted	= 0x00;

	if (0 != p_pstOPTM->pstCfg)
	{
		ucStarted = 0x01;

		logger_log (&g_stVoiceLog, LOG_NORMAL, "@%04d %s: Service starting", __LINE__, MOD_OPTM_NAME);

		ucRet = thread_start (&p_pstOPTM->stMaster.stThread, mod_optm_master_body);

		if (0x00 != ucRet)
		{
			logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Service failure on start. RetCode [0x%02X.0x02X]", __LINE__, MOD_OPTM_NAME, ucRet, p_pstOPTM->stMaster.stThread.iErr);
			ucRet = 0x02;
			goto CleanUp;
		}
		else
		{
			ucRet = thread_start (&p_pstOPTM->stHelper.stThread, mod_optm_helper_body);

			if (0x00 != ucRet)
			{
				logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Service failure on start. RetCode [0x%02X.0x02X]", __LINE__, MOD_OPTM_NAME, ucRet, p_pstOPTM->stHelper.stThread.iErr);
				ucRet = 0x03;
				goto CleanUp;
			}
		}

		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Service started", __LINE__, MOD_OPTM_NAME);
	}

	ucRet = (0x01 == ucStarted) ? 0x00 : 0xFF;

CleanUp:

	return ucRet;
}

unsigned char mod_optm_get_pstn_call_id (MOD_OPTM *p_pstOPTM, unsigned char p_ucChannel, char *p_pcBuf, int p_iBufSize, unsigned char *p_pucSlot)
{
	memset (p_pcBuf, 0x00, p_iBufSize);
	*p_pucSlot = 0x00;

	switch (p_ucChannel)
	{
	case 0x01:	// TEL
	case 0x03:	// FAX
		break;

	case 0x02:	// POS
		*p_pucSlot = p_pstOPTM->stMaster.iPOS_Slot;

		strncpy (p_pcBuf, p_pstOPTM->stMaster.acPOS_CallID, p_iBufSize - 1);
		memset (p_pstOPTM->stMaster.acPOS_CallID, 0x00, sizeof (p_pstOPTM->stMaster.acPOS_CallID));
		return 0x00;
	}

	return 0x01;
}

void *mod_optm_malloc (size_t p_stSize)
{
	void	*pvPtr;

	MEM_ALLOC (pvPtr, p_stSize, (void *));

	return pvPtr;
}

void mod_optm_free (void *p_pvMem)
{
	MEM_CLEAN (p_pvMem);
}

void mod_optm_memset (void *p_pvDst, int p_iVal, size_t p_stSize)
{
	memset (p_pvDst, p_iVal, p_stSize);
}

void mod_optm_memcpy (void *p_pvDst, const void *p_pvSrc, size_t p_stSize)
{
	memcpy (p_pvDst, p_pvSrc, p_stSize);
}

void mod_optm_memmove (void *p_pvDst, const void *p_pvSrc, size_t p_stSize)
{
	memmove (p_pvDst, p_pvSrc, p_stSize);
}

int mod_optm_memcmp (void *p_pvDst, const void *p_pvSrc, size_t p_stSize)
{
	return memcmp (p_pvDst, p_pvSrc, p_stSize);
}

void mod_optm_sleep (unsigned long p_ulMillisecond)
{
	ISLEEP (p_ulMillisecond);
}

void mod_optm_get_tick (unsigned long *p_pulTick)
{
	IGET_TICKCOUNT (*p_pulTick);
}

unsigned char mod_optm_raw_recv (void *p_pvOPTM, unsigned char *p_pucRecv, unsigned int p_uiRecvSize, unsigned int *p_puiRecvLen)
{
	unsigned char	ucRet;

	MOD_OPTM		*pstOPTM	= (MOD_OPTM *) ((OPTM *) p_pvOPTM)->pvBasket;

	ucRet = comport_recv_voice (&pstOPTM->stMaster.stPort, p_pucRecv, p_uiRecvSize, (int *) p_puiRecvLen, 1);

	if (0 != *p_puiRecvLen)
	{
		if (LOG_DETAIL >= g_stVoiceLog.ucLogLevel)
		{
			pstOPTM->ucMoreHex = ((*p_puiRecvLen * 2) < sizeof (pstOPTM->acHex) - 1) ? 0x00 : 0x01;
			ucByteToHex (p_pucRecv, (int) (0x00 == pstOPTM->ucMoreHex) ? *p_puiRecvLen : ((sizeof (pstOPTM->acHex) / 2) - 1), pstOPTM->acHex);

			logger_log (&g_stVoiceLog, LOG_DETAIL, "@%04d %s: %s @%02X recv [%04d: %s%s]", __LINE__, MOD_OPTM_PROTO_NAME,
				pstOPTM->pstCfg->acID, pstOPTM->stMaster.stOPTM.ucProcState, *p_puiRecvLen, pstOPTM->acHex, (0x00 == pstOPTM->ucMoreHex) ? "" : "...");
		}
	}

	return ucRet;
}

unsigned char mod_optm_raw_send (void *p_pvOPTM, unsigned char *p_pucSend, unsigned int p_uiSendSize, unsigned int *p_puiSendLen)
{
	MOD_OPTM	*pstOPTM	= (MOD_OPTM *) ((OPTM *) p_pvOPTM)->pvBasket;

	if (LOG_DETAIL >= g_stVoiceLog.ucLogLevel)
	{
		pstOPTM->ucMoreHex = ((*p_puiSendLen * 2) < sizeof (pstOPTM->acHex) - 1) ? 0x00 : 0x01;
		ucByteToHex (p_pucSend, (int) (0x00 == pstOPTM->ucMoreHex) ? *p_puiSendLen : ((sizeof (pstOPTM->acHex) / 2) - 1), pstOPTM->acHex);

		logger_log (&g_stVoiceLog, LOG_DETAIL, "@%04d %s: %s @%02X sent [%04d: %s%s]", __LINE__, MOD_OPTM_PROTO_NAME,
			pstOPTM->pstCfg->acID, pstOPTM->stMaster.stOPTM.ucProcState, *p_puiSendLen, pstOPTM->acHex, (0x00 == pstOPTM->ucMoreHex) ? "" : "...");
	}

	return comport_send_voice (&pstOPTM->stMaster.stPort, p_pucSend, *p_puiSendLen);
}

unsigned char mod_optm_log (void *p_pvOPTM, unsigned int p_uiLogEvent, unsigned int p_uiLine, unsigned char p_ucRet, ...)
{
	unsigned char	*pucBuf;

	unsigned int	uiBufLen;

	MOD_OPTM		*pstOPTM	= (MOD_OPTM *) ((OPTM *) p_pvOPTM)->pvBasket;

	va_list			args;

	va_start (args, p_ucRet);

	switch (p_uiLogEvent)
	{
	case OPTM_LOG_HANDSHAKE_SUCCESS_CODE:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Handshake successful", p_uiLine, MOD_OPTM_PROTO_NAME); break;

	case OPTM_LOG_HANDSHAKE_ERROR_CODE:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Handshake failure. RetCode [0x%02X]", p_uiLine, MOD_OPTM_PROTO_NAME, p_ucRet); break;

	case OPTM_LOG_HANDSHAKE_TIMEOUT_CODE:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Handshake is required", p_uiLine, MOD_OPTM_PROTO_NAME); break;

	case OPTM_LOG_ADMIN_LOGIN_SUCCESS_CODE:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Admin login successful", p_uiLine, MOD_OPTM_PROTO_NAME); break;

	case OPTM_LOG_ADMIN_LOGIN_REJECT_CODE:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Admin login rejected. RetCode [0x%02X]", p_uiLine, MOD_OPTM_PROTO_NAME, p_ucRet); break;

	case OPTM_LOG_ADMIN_LOGIN_ERROR_CODE:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Admin login failure. RetCode [0x%02X]", p_uiLine, MOD_OPTM_PROTO_NAME, p_ucRet); break;

	case OPTM_LOG_ADMIN_LOGOUT_SUCCESS_CODE:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Admin logout successful", p_uiLine, MOD_OPTM_PROTO_NAME); break;

	case OPTM_LOG_ADMIN_LOGOUT_REJECT_CODE:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Admin logout rejected. RetCode [0x%02X]", p_uiLine, MOD_OPTM_PROTO_NAME, p_ucRet); break;

	case OPTM_LOG_ADMIN_LOGOUT_ERROR_CODE:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Admin logout failure. RetCode [0x%02X]", p_uiLine, MOD_OPTM_PROTO_NAME, p_ucRet); break;

	case OPTM_LOG_ADMIN_CHG_PWD_SUCCESS_CODE:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Alter admin login password successful", p_uiLine, MOD_OPTM_PROTO_NAME); break;

	case OPTM_LOG_ADMIN_CHG_PWD_REJECT_CODE:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Alter admin login password rejected. RetCode [0x%02X]", p_uiLine, MOD_OPTM_PROTO_NAME, p_ucRet); break;

	case OPTM_LOG_ADMIN_CHG_PWD_ERROR_CODE:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Alter admin login password failure. RetCode [0x%02X]", p_uiLine, MOD_OPTM_PROTO_NAME, p_ucRet); break;

	case OPTM_LOG_AUTH_SUCCESS_CODE:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Authentication successful", p_uiLine, MOD_OPTM_PROTO_NAME); break;

	case OPTM_LOG_AUTH_REJECT_CODE:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Authentication rejected. RetCode [0x%02X]", p_uiLine, MOD_OPTM_PROTO_NAME, p_ucRet); break;

	case OPTM_LOG_AUTH_ERROR_CODE:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Authentication failure. RetCode [0x%02X]", p_uiLine, MOD_OPTM_PROTO_NAME, p_ucRet); break;

	case OPTM_LOG_ALT_AUTH_SUCCESS_CODE:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Alter authentication successful", p_uiLine, MOD_OPTM_PROTO_NAME); break;

	case OPTM_LOG_ALT_AUTH_REJECT_CODE:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Alter authentication rejected. RetCode [0x%02X]", p_uiLine, MOD_OPTM_PROTO_NAME, p_ucRet); break;

	case OPTM_LOG_ALT_AUTH_ERROR_CODE:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Alter authentication failure. RetCode [0x%02X]", p_uiLine, MOD_OPTM_PROTO_NAME, p_ucRet); break;

	case OPTM_LOG_RECV_TIMEOUT_CODE:
		pstOPTM->ucMoreHex = ((pstOPTM->stMaster.stOPTM.uiRecvBufLen * 2) < sizeof (pstOPTM->acHex) - 1) ? 0x00 : 0x01;
		ucByteToHex (pstOPTM->stMaster.stOPTM.aucRecvBuf,
			(int) (0x00 == pstOPTM->ucMoreHex) ? pstOPTM->stMaster.stOPTM.uiRecvBufLen : ((sizeof (pstOPTM->acHex) / 2) - 1), pstOPTM->acHex);

		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: %s receive frame timeout [%04d: %s%s]", p_uiLine, MOD_OPTM_PROTO_NAME,
			pstOPTM->pstCfg->acID, pstOPTM->stMaster.stOPTM.uiRecvBufLen, pstOPTM->acHex, (0x00 == pstOPTM->ucMoreHex) ? "" : "...");
		break;

	case OPTM_LOG_FRAME_CORRUPTED_CODE:
		pucBuf = va_arg (args, unsigned char *);
		uiBufLen = va_arg (args, unsigned int);

		pstOPTM->ucMoreHex = ((uiBufLen * 2) < sizeof (pstOPTM->acHex) - 1) ? 0x00 : 0x01;
		ucByteToHex (pucBuf, (int) (0x00 == pstOPTM->ucMoreHex) ? uiBufLen : ((sizeof (pstOPTM->acHex) / 2) - 1), pstOPTM->acHex);

		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: %s received frame error [Send class: %02X cmd: %02X - Recv %04d: %s%s]", p_uiLine, MOD_OPTM_PROTO_NAME,
			pstOPTM->pstCfg->acID, pstOPTM->stMaster.stOPTM.stSendCache.stFrame.ucClass, pstOPTM->stMaster.stOPTM.stSendCache.stFrame.ucCmd_Status,
			uiBufLen, pstOPTM->acHex, (0x00 == pstOPTM->ucMoreHex) ? "" : "...");
		break;

	case OPTM_LOG_FRAME_UNPROCESS_CODE:
		pstOPTM->ucMoreHex = ((pstOPTM->stMaster.stOPTM.uiRecvBufLen * 2) < sizeof (pstOPTM->acHex) - 1) ? 0x00 : 0x01;
		ucByteToHex (pstOPTM->stMaster.stOPTM.aucRecvBuf, (int) (0x00 == pstOPTM->ucMoreHex) ? pstOPTM->stMaster.stOPTM.uiRecvBufLen : ((sizeof (pstOPTM->acHex) / 2) - 1), pstOPTM->acHex);

		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: %s unprocess frame class 0x%02X command 0x%02X @0x%02X (Send event class 0x%02X command 0x%02X) [%04d: %s%s]", p_uiLine, MOD_OPTM_PROTO_NAME,
			pstOPTM->pstCfg->acID, pstOPTM->stMaster.stOPTM.stRecvFrame.ucClass, pstOPTM->stMaster.stOPTM.stRecvFrame.ucCmd_Status, pstOPTM->stMaster.stOPTM.ucProcState,
			pstOPTM->stMaster.stOPTM.stSendCache.stFrame.ucClass, pstOPTM->stMaster.stOPTM.stSendCache.stFrame.ucCmd_Status,
			pstOPTM->stMaster.stOPTM.uiRecvBufLen, pstOPTM->acHex, (0x00 == pstOPTM->ucMoreHex) ? "" : "...");
		break;

	case OPTM_LOG_PROC_FAILED_CODE:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Proc %d failure. RetCode [0x%02X]", p_uiLine, MOD_OPTM_PROTO_NAME,
			pstOPTM->stMaster.stOPTM.ucProcState, p_ucRet);
		break;

	default:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Event 0x%02X happen. RetCode [0x%02X]", p_uiLine, MOD_OPTM_PROTO_NAME, p_uiLogEvent, p_ucRet);
	}

	return 0x00;
}

unsigned char mod_optm_tel_event (void *p_pvOPTM, unsigned char p_ucEventID, unsigned int p_uiLine, unsigned char p_ucRet, ...)
{
	unsigned char	ucRet		= 0x00;

	char			acName [64]	= { 0 },
					*pcCallID;

	int				iSpeedDial;

	MOD_OPTM		*pstOPTM	= (MOD_OPTM *) ((OPTM *) p_pvOPTM)->pvBasket;

	va_list			args;

	va_start (args, p_ucRet);

	switch (p_ucEventID)
	{
	case OPTM_FRAME_TEL_EVT_MCU_DAILING_CODE:
		pstOPTM->stMaster.ucEvent = MOD_OPTM_MASTER_TEL_EVT_MCU_DIALING;

		pstOPTM->stMaster.ucTEL_State = 0x01;

		if (0 >= g_stCoRel.stCoRels.iCount)
		{
			logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: TEL @%02X is dialing", __LINE__, MOD_OPTM_NAME, pstOPTM->stHelper.ucState);
		}
		else
		{
			logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: TEL is not allow to use during transaction processing", __LINE__, MOD_OPTM_NAME);
			ucRet = 0x01;
		}

		break;

	case OPTM_FRAME_TEL_EVT_MCU_DIAL_NO_CODE:
		// Set the caller ID
		pcCallID = va_arg (args, char *);
		iSpeedDial = va_arg (args, int);

		pstOPTM->stMaster.ucTEL_State = 0x01;
		memset (pstOPTM->stMaster.acTEL_CallID, 0x00, sizeof (pstOPTM->stMaster.acTEL_CallID));

		if (1 == iSpeedDial)
		{
			// retrieve speed dial number from file
			sprintf (acName, "speed_dial.%d=", atoi (pcCallID));

			ucRet = mod_optm_read_file (p_pvOPTM, acName, pstOPTM->stMaster.acTEL_CallID, sizeof (pstOPTM->stMaster.acTEL_CallID) - 1);

			if (0x00 != ucRet || 0 == pstOPTM->stMaster.acTEL_CallID [0])
			{
				ucRet = 0x01;
			}
			else
			{
				ucRet = 0x00;
			}
		}
		else
		{
			// Normal caller id
			strncpy (pstOPTM->stMaster.acTEL_CallID, pcCallID, sizeof (pstOPTM->stMaster.acTEL_CallID) - 1);
			ucRet = 0x00;
		}

		if (0x00 == ucRet)
		{
			pstOPTM->stMaster.ucEvent = MOD_OPTM_MASTER_TEL_EVT_MCU_DIAL_NO;

			logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: TEL @%02X is dialing %s", __LINE__, MOD_OPTM_NAME, pstOPTM->stHelper.ucState, pstOPTM->stMaster.acTEL_CallID);
		}
		else
		{
			logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Speed-dial number %s# is empty", __LINE__, MOD_OPTM_NAME, pcCallID);
		}

		break;

	case OPTM_FRAME_TEL_EVT_MCU_HANG_UP_CODE:
		pstOPTM->stMaster.ucTEL_State = 0x00;
		memset (pstOPTM->stMaster.acTEL_CallID, 0x00, sizeof (pstOPTM->stMaster.acTEL_CallID));
		pstOPTM->stMaster.ucEvent = MOD_OPTM_MASTER_TEL_EVT_MCU_HANG_UP;

		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: TEL @%02X is hang-up", __LINE__, MOD_OPTM_NAME, pstOPTM->stHelper.ucState);
		break;

	case OPTM_FRAME_TEL_EVT_MCU_CONNECT_CODE:
		pstOPTM->stMaster.ucTEL_State = 0x01;
		pstOPTM->stMaster.ucEvent = MOD_OPTM_MASTER_TEL_EVT_MCU_CONNECT;

		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: TEL @%02X is off-hook", __LINE__, MOD_OPTM_NAME, pstOPTM->stHelper.ucState);
		break;

	case OPTM_FRAME_TEL_EVT_MCU_RINGING_CODE:
		pstOPTM->stMaster.ucTEL_State = 0x00;

		if (0x00 == p_ucRet)
		{
			pstOPTM->stMaster.ucEvent = MOD_OPTM_MASTER_TEL_EVT_MCU_RINGING;
			logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: TEL @%02X is ringing", __LINE__, MOD_OPTM_NAME, pstOPTM->stHelper.ucState);
		}
		else
		{
			pstOPTM->stMaster.ucEvent = MOD_OPTM_MASTER_TEL_EVT_MCU_HANG_UP;
			logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: TEL is not ready to answer incoming call", __LINE__, MOD_OPTM_NAME);
		}

		break;

	case OPTM_FRAME_TEL_EVT_TIMEOUT_CODE:
	case OPTM_FRAME_TEL_EVT_ERROR_CODE:
	case OPTM_FRAME_TEL_EVT_FAILED_CODE:
	default:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: TEL event 0x%02X happen. RetCode [0x%02X]", __LINE__, MOD_OPTM_NAME, p_ucEventID, p_ucRet);
	}

	va_end (args);

	return ucRet;
}

unsigned char mod_optm_pos_event (void *p_pvOPTM, unsigned char p_ucEventID, unsigned int p_uiLine, unsigned char p_ucRet, ...)
{
	unsigned char	ucRet		= 0x00;

	char			*pcCallID;

	int				iSlot;

	MOD_OPTM		*pstOPTM	= (MOD_OPTM *) ((OPTM *) p_pvOPTM)->pvBasket;

	va_list			args;

	va_start (args, p_ucRet);

	switch (p_ucEventID)
	{
	case OPTM_FRAME_POS_EVT_MCU_DAILING_CODE:
		iSlot = va_arg (args, int);	// Set phone cable slot

		memset (pstOPTM->stMaster.acPOS_CallID, 0x00, sizeof (pstOPTM->stMaster.acPOS_CallID));
		pstOPTM->stMaster.iPOS_Slot = iSlot;
		pstOPTM->stMaster.ucPOS_State = 0x01;

		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: POS %d is off-hook", __LINE__, MOD_OPTM_NAME, iSlot);
		break;

	case OPTM_FRAME_POS_EVT_MCU_DIAL_NO_CODE:
		pcCallID = va_arg (args, char *);	// Set the caller ID
		iSlot = va_arg (args, int);	// Set phone cable slot

		memset (pstOPTM->stMaster.acPOS_CallID, 0x00, sizeof (pstOPTM->stMaster.acPOS_CallID));
		strncpy (pstOPTM->stMaster.acPOS_CallID, pcCallID, sizeof (pstOPTM->stMaster.acPOS_CallID) - 1);

		pstOPTM->stMaster.iPOS_Slot = iSlot;
		pstOPTM->stMaster.ucPOS_State = 0x01;

		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: POS %d is dialed %s", __LINE__, MOD_OPTM_NAME, iSlot, pstOPTM->stMaster.acPOS_CallID);
		break;

	case OPTM_FRAME_POS_EVT_MCU_HANG_UP_CODE:
		iSlot = va_arg (args, int);	// Set phone cable slot

		memset (pstOPTM->stMaster.acPOS_CallID, 0x00, sizeof (pstOPTM->stMaster.acPOS_CallID));
		pstOPTM->stMaster.iPOS_Slot = iSlot;
		pstOPTM->stMaster.ucPOS_State = 0x00;

		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: POS %d is hang-up", __LINE__, MOD_OPTM_NAME, iSlot);
		break;

	case OPTM_FRAME_POS_EVT_TIMEOUT_CODE:
	case OPTM_FRAME_POS_EVT_ERROR_CODE:
	case OPTM_FRAME_POS_EVT_FAILED_CODE:
	default:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: POS event 0x%02X happen. RetCode [0x%02X]", __LINE__, MOD_OPTM_NAME, p_ucEventID, p_ucRet);
	}

	va_end (args);

	return ucRet;
}

unsigned char mod_optm_handshake (void *p_pvOPTM)
{
	unsigned char	ucRet;

	char			acBuf [64]		= { 0 };

	int				iParam			= 0;

	OPTM			*pstCore		= (OPTM *) p_pvOPTM;

	MOD_OPTM		*pstOPTM	    = (MOD_OPTM *) ((OPTM *) p_pvOPTM)->pvBasket;

	ucRet = mod_optm_read_file (p_pvOPTM, "auth.pwd=", acBuf, sizeof (acBuf) - 1);

	if (0x00 == ucRet && (0 == strcmp (acBuf, "0000") || 0 == acBuf [0]))
	{
		ucRet = 0x00;
	}
	else
	{
		ucRet = 0x01;
	}

	pstCore->stSendFrame.ucClass = OPTM_FRAME_CLS_HANDSHAKE_CODE;
	pstCore->stSendFrame.ucCmd_Status = OPTM_FRAME_HANDSHAKE_CMD_EXCHG_PARAM_CODE;

	// == Auth flag ==
	pstCore->stSendFrame.stParam.astItem [iParam].ucCode = iParam + 1;
	pstCore->stSendFrame.stParam.astItem [iParam].iDataLen = 1;
	MEM_ALLOC (pstCore->stSendFrame.stParam.astItem [iParam].pucData, pstCore->stSendFrame.stParam.astItem [iParam].iDataLen, (unsigned char *));
	pstCore->stSendFrame.stParam.astItem [iParam].pucData [0] = (0x00 == ucRet) ? 0x00 : 0x01;
	pstCore->stSendFrame.stParam.astItem [iParam].ucIsUsed = 0x01;
	iParam++;

	// == Key gap ==
	pstCore->stSendFrame.stParam.astItem [iParam].ucCode = iParam + 1;
	pstCore->stSendFrame.stParam.astItem [iParam].iDataLen = 1;
	MEM_ALLOC (pstCore->stSendFrame.stParam.astItem [iParam].pucData, pstCore->stSendFrame.stParam.astItem [iParam].iDataLen, (unsigned char *));
	pstCore->stSendFrame.stParam.astItem [iParam].pucData [0] = 0x07;
	pstCore->stSendFrame.stParam.astItem [iParam].ucIsUsed = 0x01;
	iParam++;

	// == Enable/Disable ports ==
	pstCore->stSendFrame.stParam.astItem [iParam].ucCode = iParam + 1;
	pstCore->stSendFrame.stParam.astItem [iParam].iDataLen = 5;
	MEM_ALLOC (pstCore->stSendFrame.stParam.astItem [iParam].pucData, pstCore->stSendFrame.stParam.astItem [iParam].iDataLen, (unsigned char *));
	pstCore->stSendFrame.stParam.astItem [iParam].pucData [0] = 0x01;	// TEL
	pstCore->stSendFrame.stParam.astItem [iParam].pucData [1] = 0x00;	// FAX
	pstCore->stSendFrame.stParam.astItem [iParam].pucData [2] = 0x01;	// POS1
	pstCore->stSendFrame.stParam.astItem [iParam].pucData [3] = 0x01;	// POS2
	pstCore->stSendFrame.stParam.astItem [iParam].pucData [4] = 0x01;	// POS3
	pstCore->stSendFrame.stParam.astItem [iParam].ucIsUsed = 0x01;
	iParam++;

    // == Enable/Disable concurrency data channel and voice channel ==
    // more than 3G level support voice and data channel concurrency happen
	pstCore->stSendFrame.stParam.astItem [iParam].ucCode = iParam + 1;
	pstCore->stSendFrame.stParam.astItem [iParam].iDataLen = 1;
	MEM_ALLOC (pstCore->stSendFrame.stParam.astItem [iParam].pucData, pstCore->stSendFrame.stParam.astItem [iParam].iDataLen, (unsigned char *));

  if (CFG_MODEM_ID_UC864E_CODE == pstOPTM->pstCfg->ucModemID &&
      HAL_UC864E_NETWORK_MODE_HSPDA == g_stArch.stMobile.stUC864E.ucNetworkMode)
  {
  	pstCore->stSendFrame.stParam.astItem [iParam].pucData [0] = 0x01;
  }
  else
  {
      pstCore->stSendFrame.stParam.astItem [iParam].pucData [0] = 0x00;
  }
	pstCore->stSendFrame.stParam.astItem [iParam].ucIsUsed = 0x01;
	iParam++;

	pstCore->stSendFrame.stParam.iCount = iParam;

	return 0x00;
}

unsigned char mod_optm_admin_login (void *p_pvOPTM, unsigned char *p_pucPwd, int p_iLen)
{
	unsigned char	ucRet;

	char			acPwd [64]		= { 0 },
					acBuf [64]		= { 0 };

	OPTM			*pstOPTM	= (OPTM *) p_pvOPTM;

	ucRet = mod_optm_read_file (p_pvOPTM, "admin.pwd=", acBuf, sizeof (acBuf) - 1);

	if (0x00 != ucRet || (0x00 == ucRet && 0 == acBuf [0]))
	{
		strncpy (acPwd, (char *) p_pucPwd, p_iLen);

		if (0 == strcmp (OPTM_SUPER_ADMIN_PWD, acPwd))
		{
			pstOPTM->ucAdmin = 0x01;
			return 0x00;
		}
		else
		{
			return 0x02;
		}
	}
	else
	{
		strncpy (acPwd, (char *) p_pucPwd, p_iLen);

		if (0 == strcmp (acBuf, acPwd))
		{
			pstOPTM->ucAdmin = 0x01;
			return 0x00;
		}
	}

	return 0x01;
}

unsigned char mod_optm_admin_logout (void *p_pvOPTM)
{
	OPTM		*pstOPTM	= (OPTM *) p_pvOPTM;

	pstOPTM->ucAdmin = 0x00;
	return 0x00;
}

unsigned char mod_optm_admin_chg_pwd (void *p_pvOPTM, unsigned char *p_pucPwd, int p_iLen)
{
	unsigned char	ucRet		= 0x01;

	char			acPwd [64]	= { 0 },
					acBuf [64]	= { 0 };

	OPTM			*pstOPTM	= (OPTM *) p_pvOPTM;

	if (0x00 == pstOPTM->ucAdmin)
	{
		// Cater the admin password is not set before then let it go thru
		ucRet = mod_optm_read_file (p_pvOPTM, "admin.pwd=", acBuf, sizeof (acBuf) - 1);

		if (!((0x00 == ucRet && 0 == acBuf [0]) || 0x01 == ucRet))
		{
			return 0x02;
		}
	}

	strncpy (acPwd, (char *) p_pucPwd, p_iLen);

	ucRet = mod_optm_edit_file (p_pvOPTM, "admin.pwd=", acPwd);

	if (0x00 != ucRet)
	{
		ucRet = 0x02;
	}
	else
	{
		pstOPTM->ucAdmin = 0x01;
	}

	return ucRet;
}

unsigned char mod_optm_auth_pwd (void *p_pvOPTM, unsigned char *p_pucPwd, int p_iLen)
{
	unsigned char	ucRet;

	char			acPwd [64]		= { 0 },
					acBuf [64]		= { 0 };

	ucRet = mod_optm_read_file (p_pvOPTM, "auth.pwd=", acBuf, sizeof (acBuf) - 1);

	if (0x00 == ucRet)
	{
		strncpy (acPwd, (char *) p_pucPwd, p_iLen);

		if (0 == strcmp (acBuf, acPwd) || 0 == strcmp (acBuf, "0000"))
		{
			return 0x00;
		}
		else
		{
			return 0x02;
		}
	}

	return 0x01;
}

unsigned char mod_optm_alt_auth_pwd (void *p_pvOPTM, unsigned char *p_pucNewPwd, int p_iNewLen)
{
	unsigned char	ucRet;

	char			acPwd [64]	= { 0 };

	OPTM			*pstOPTM	= (OPTM *) p_pvOPTM;

	if (0x01 == pstOPTM->ucAdmin)
	{
		strncpy (acPwd, (char *) p_pucNewPwd, p_iNewLen);

		ucRet = mod_optm_edit_file (p_pvOPTM, "auth.pwd=", acPwd);

		if (0x00 == ucRet)
		{
			return 0x00;
		}
		else
		{
			return 0x02;
		}
	}

	return 0x01;
}

unsigned char mod_optm_speed_dial_add (void *p_pvOPTM, unsigned char p_ucIndex, unsigned char *p_pucCallerID, int p_iLen)
{
	unsigned char	ucRet;

	char			acCallerID [64]		= { 0 },
					acBuf [32]			= { 0 };

	strncpy (acCallerID, (char *) p_pucCallerID, p_iLen);
	sprintf (acBuf, "speed_dial.%d=", p_ucIndex);

	ucRet = mod_optm_edit_file (p_pvOPTM, acBuf, acCallerID);

	return ucRet;
}

unsigned char mod_optm_speed_dial_del (void *p_pvOPTM, unsigned char p_ucIndex)
{
	unsigned char	ucRet;

	char			acBuf [32]			= { 0 };

	sprintf (acBuf, "speed_dial.%d=", p_ucIndex);

//	if (0x00 != mod_optm_read_file (p_pvOPTM, acBuf, 0, 0))
//	{
//		ucRet = 0x00;
//	}
//	else
//	{
		ucRet = mod_optm_edit_file (p_pvOPTM, acBuf, "");
//	}

	return ucRet;
}

unsigned char mod_optm_read_file (void *p_pvOPTM, char *p_pcName, char *p_pcValue, int p_iSize)
{
	unsigned char	ucRet;

	char			acBuf [64]		= { 0 },
					*pcPtr;

	FILE			*pfd;

	if (0 != p_pcValue)
	{
		memset (p_pcValue, 0x00, p_iSize);
	}

	FOPEN (pfd, MOD_OPTM_FILE, "rb");

	if (0 == pfd)
	{
		return 0x02;
	}

	ucRet = 0x01;

	while (0 != fgets (acBuf, sizeof (acBuf) - 1, pfd))
	{
		pcPtr = strstr (acBuf, p_pcName);

		if (0 != pcPtr)
		{
			pcPtr += strlen (p_pcName);

			if (0 != p_pcValue)
			{
				strncpy (p_pcValue, pcPtr, p_iSize);

				pcPtr = p_pcValue;

				while (0 != *pcPtr)
				{
					if (10 == *pcPtr || 13 == *pcPtr)
						*pcPtr = 0;
					pcPtr++;
				}
			}

			ucRet = 0x00;
			goto CleanUp;
		}
	}

CleanUp:
	FCLOSE (pfd);

	return ucRet;
}

unsigned char mod_optm_edit_file (void *p_pvOPTM, char *p_pcName, char *p_pcValue)
{
	unsigned char	ucRet				= 0xFF;

	char			acBuf [3096]		= { 0 },
					*pcPtr,
					*pcStart			= 0,
					*pcEnd				= 0,
					*pcHead,
					*pcTail,
					*pacArgv [3];

	int				iBufSize			= 0,
					iLen;

	FILE			*pfd;

	// Load file into memory
	FOPEN (pfd, MOD_OPTM_FILE, "rb");

	if (0 != pfd)
	{
		pcPtr = acBuf;
		iBufSize = sizeof (acBuf) - 1;

		do
		{
			iLen = fread (pcPtr, sizeof (char), iBufSize, pfd);

			if (0 < iLen)
			{
				iBufSize -= iLen;
				pcPtr += iLen;
			}
			else
			{
				break;
			}
		}
		while (!feof (pfd) && 0 < iBufSize);

		FCLOSE (pfd);
	}

	pcHead = acBuf;
	pcTail = acBuf + sizeof (acBuf) - 1;

	pcStart = strstr (pcHead, p_pcName);

	if (0 != pcStart)
	{
		pcStart += strlen (p_pcName);

		pcEnd = strstr (pcStart, "\r\n");

		if (0 == pcEnd)
		{
			pcEnd = pcHead + strlen (acBuf);
		}
		else
		{
			pcEnd += 2;
		}
	}

	if (0 != pcStart && 0 != pcEnd)
	{
		if (pcEnd < pcStart + strlen (p_pcValue) + 2)
		{
			// extend length
			memmove (pcStart + strlen (p_pcValue) + 2, pcEnd, pcTail - pcEnd - strlen (p_pcValue) + 2);
		}
		else
		{
			// shorten length
			memmove (pcStart + strlen (p_pcValue) + 2, pcEnd, pcTail - pcEnd);
		}

		memcpy (pcStart, p_pcValue, strlen (p_pcValue));
		*(pcStart + strlen (p_pcValue)) = '\r';
		*(pcStart + strlen (p_pcValue) + 1) = '\n';
	}
	else
	{
		pcStart = pcHead + strlen (acBuf);

		if (pcStart >= pcHead + 2)
		{
			if ('\r' != *(pcStart - 2) && '\n' != *(pcStart - 1))
			{
				*pcStart++ = '\r';
				*pcStart++ = '\n';
			}
		}

		sprintf (pcStart, "%s%s\r\n", p_pcName, p_pcValue);
	}

	iLen = strlen (acBuf);

	// Unlock the partion to be writable
	pacArgv [0] = "/usr/sbin/unlock";
	pacArgv [1] = "apps";
	pacArgv [2] = 0;

	ucRet = mod_optm_script (p_pvOPTM, pacArgv [0], pacArgv, 5000, 0x00);

	if (0x00 != ucRet)
	{
		return 0x02;
	}

	FOPEN (pfd, MOD_OPTM_FILE, "wb");

	if (0 == pfd)
	{
		ucRet = 0x02;
	}
	else
	{
		fwrite (acBuf, sizeof (char), iLen, pfd);
		FCLOSE (pfd);

		ucRet = 0x00;
	}

	// Lock the partion to be writable
	pacArgv [0] = "/usr/sbin/lock";
	pacArgv [1] = "apps";
	pacArgv [2] = 0;

	mod_optm_script (p_pvOPTM, pacArgv [0], pacArgv, 5000, 0x00);

	return ucRet;
}

unsigned char mod_optm_script (void *p_pvOPTM, char *p_pcScript, char *p_pacArgv [], unsigned long p_ulTimeout, unsigned char p_ucForced)
{
	#if defined (WIN32)

		return 0x00;

	#else

		unsigned char	ucRet			= 0x01;

		unsigned long	ulCurTime		= 0,
						ulStartTime,
						ulLastLog		= 0;

		int				iStatus,
						iRet,
						iFd;

		pid_t			stChildPid,
						stRetPid;

		stChildPid = vfork ();

		switch (stChildPid)
		{
		case 0:		// Child
			// Dont close the stdin (0), stdout (1), strerr (2)
			for (iRet = getdtablesize (); 3 <= iRet; --iRet)
			{
				iFd = iRet;
				CLOSE (iFd); // close all descriptors
			}

			// Set a new group ID
			setpgid (0, 0);

			execv (p_pcScript, p_pacArgv);

			iRet = (0 != errno) ? errno : 1;

			logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Script execution failure. RetCode [%d: %s]", __LINE__, MOD_OPTM_NAME, errno, strerror (errno));
			_exit (iRet);

		case -1:	// Error
			logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Script execution failure. RetCode [%d: %s]", __LINE__, MOD_OPTM_NAME, errno, strerror (errno));
			ucRet = 0x02;
			goto CleanUp;

		default:	// Parent
			IGET_TICKCOUNT (ulCurTime);
			ulStartTime = ulCurTime;

			while (1)
			{
				stRetPid = waitpid (stChildPid, &iStatus, WNOHANG | WUNTRACED);

				if (0 == stRetPid)
				{
					if (ulCurTime - ulLastLog >= 1000 && ulCurTime >= ulLastLog)
					{
						ulLastLog = ulCurTime;
						logger_log (&g_stVoiceLog, LOG_DETAIL, "@%04d %s: Script %d: %s is in progress", __LINE__, MOD_OPTM_NAME, stChildPid, p_pcScript);
					}
				}
				else if (-1 == stRetPid)
				{
					logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Script %d: %s termination failure. RetCode [%d: %s]", __LINE__, MOD_OPTM_NAME,
						stChildPid, p_pcScript, errno, strerror (errno));
					ucRet = 0x03;
					goto CleanUp;
				}
				else
				{
					if (WIFEXITED (iStatus))
					{
						iRet = WEXITSTATUS (iStatus);

						if (0 == iRet)
						{
							logger_log (&g_stVoiceLog, LOG_NORMAL, "@%04d %s: Script %d: %s is executed", __LINE__, MOD_OPTM_NAME, stChildPid, p_pcScript);
							ucRet = 0x00;
						}
						else
						{
							logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Script %d: %s is executed with return code [%d]", __LINE__, MOD_OPTM_NAME, stChildPid, p_pcScript, iRet);
							ucRet = 0x04;
						}
					}
					else
					{
						logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Script %d: %s is executed. RetStatus [%d]", __LINE__, MOD_OPTM_NAME, stChildPid, p_pcScript, iStatus);
						ucRet = 0x05;
					}

					break;
				}

				ISLEEP (1);
				IGET_TICKCOUNT (ulCurTime);

				if (ulCurTime - ulStartTime >= p_ulTimeout && ulCurTime >= ulStartTime)
				{
					logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Script %d: %s is terminating after %ums timeout", __LINE__, MOD_OPTM_NAME,
						stChildPid, p_pcScript, ulCurTime - ulStartTime);

					// Kill the script after timeout
					iRet = killpg (stChildPid, SIGTERM);

					waitpid (stChildPid, 0, 0);

					if (0 != iRet)
					{
						logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Script %d: %s kill failure. RetCode [%d: %s]", __LINE__, MOD_OPTM_NAME,
							stChildPid, p_pcScript, errno, strerror (errno));
					}
					else
					{
						logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Script %d: %s killed", __LINE__, MOD_OPTM_NAME, stChildPid, p_pcScript);
					}

					ucRet = 0x06;
					break;
				}
				else if (ulCurTime < ulStartTime)
				{
					ulStartTime = ulCurTime;
				}
			}
		}

	CleanUp:

		return ucRet;

	#endif
}

unsigned char mod_optm_at_cmd (MOD_OPTM *p_pstOPTM, int p_iSID, char *p_pcReq, char *p_pcRes, char *p_pcErr, char *p_pcRet, int p_iRetSize,
							   unsigned long p_ulDelay, unsigned long p_ulGap, unsigned long p_ulTimeout, unsigned int p_uiLine)
{
	unsigned char	ucRet;

	switch (p_pstOPTM->pstCfg->ucModemID)
	{
	case CFG_MODEM_ID_GTM900B_CODE:
		ucRet = hal_gtm900b_at_cmd (&g_stArch.stMobile.stGTM900B, p_iSID, p_pcReq, p_pcRes, p_pcErr, p_pcRet, p_iRetSize,
			p_ulDelay, p_ulGap, p_ulTimeout, &p_pstOPTM->stHelper.stThread.ucProcToken, p_uiLine, MOD_OPTM_NAME);
		break;

	case CFG_MODEM_ID_G600_CODE:
    ucRet = hal_g600_at_cmd (&g_stArch.stMobile.stG600, p_iSID, p_pcReq, p_pcRes, p_pcErr, p_pcRet, p_iRetSize,
			p_ulDelay, p_ulGap, p_ulTimeout, &p_pstOPTM->stHelper.stThread.ucProcToken, p_uiLine, MOD_OPTM_NAME);
		break;

	case CFG_MODEM_ID_UC864E_CODE:
		ucRet = hal_uc864e_at_cmd (&g_stArch.stMobile.stUC864E, p_iSID, p_pcReq, p_pcRes, p_pcErr, p_pcRet, p_iRetSize,
			p_ulDelay, p_ulGap, p_ulTimeout, &p_pstOPTM->stHelper.stThread.ucProcToken, p_uiLine, MOD_OPTM_NAME);
		break;

	default:
		ucRet = 0xFF;
	}

	return ucRet;
}

unsigned char mod_optm_get_mobile_call_id (MOD_OPTM *p_pstOPTM, int p_iSID, char *p_pcBuf, int p_iBufSize)
{
	unsigned char	ucRet			= 0x01;

	char			acReq [64]		= { 0 },
					acRes [64]		= { 0 },
					acErr [64]		= { 0 },
					acRet [128]		= { 0 },
					*pcStart,
					*pcEnd;

	memset (p_pcBuf, 0x00, sizeof (p_iBufSize));

	strncpy (acReq, "AT+CLCC\r", sizeof (acReq) - 1);
	strncpy (acRes, "OK", sizeof (acRes) - 1);
	strncpy (acErr, "ERROR", sizeof (acErr) - 1);

	switch (p_pstOPTM->pstCfg->ucModemID)
	{
	case CFG_MODEM_ID_GTM900B_CODE:
		ucRet = hal_gtm900b_at_cmd (&g_stArch.stMobile.stGTM900B, p_iSID, acReq, acRes, acErr, acRet, sizeof (acRet) - 1,
			0, 0, 3000, &p_pstOPTM->stHelper.stThread.ucProcToken, __LINE__, MOD_OPTM_NAME);
		break;

	case CFG_MODEM_ID_G600_CODE:
		ucRet = hal_g600_at_cmd (&g_stArch.stMobile.stG600, p_iSID, acReq, acRes, acErr, acRet, sizeof (acRet) - 1,
			0, 0, 3000, &p_pstOPTM->stHelper.stThread.ucProcToken, __LINE__, MOD_OPTM_NAME);
		break;

	case CFG_MODEM_ID_UC864E_CODE:
		ucRet = hal_uc864e_at_cmd (&g_stArch.stMobile.stUC864E, p_iSID, acReq, acRes, acErr, acRet, sizeof (acRet) - 1,
			0, 0, 3000, &p_pstOPTM->stHelper.stThread.ucProcToken, __LINE__, MOD_OPTM_NAME);
		break;

	default:
		ucRet = 0xFF;
	}

	if (0x00 == ucRet)
	{
		pcStart = strstr (acRet, "+CLCC: ");

		if (0 != pcStart)
		{
			pcStart += 7;
			pcStart = strstr (pcStart, "\"");

			if (0 != pcStart)
			{
				pcStart += 1;
				pcEnd = strchr (pcStart, '"');

				if (0 != pcEnd && 0 < (pcEnd - pcStart) && sizeof (acRet) > (pcEnd - pcStart))
				{
					strncpy (p_pcBuf, pcStart, pcEnd - pcStart);
				}
			}
		}
	}

	return ucRet;
}

unsigned char mod_optm_attach (MOD_OPTM *p_pstOPTM)
{
  unsigned char		ucRet             = 0x01;

  int             iIndex,
                  iSize             = 0,
                  iMicGain,
                  iSpeckerGain,
                  iSpeckerVolume;

	MOD_OPTM_AT_CMD *pstCmd           = 0,

                  astGTM900B []     =	{
                                        { "AT%NFI", "OK", "ERROR", 0, 5000, 0 },
                                        { "AT%NFO", "OK", "ERROR", 0, 5000, 0 },
                                        { "AT%NFV", "OK", "ERROR", 0, 5000, 0 },
                                        { "AT%STN=-26\r", "OK", "ERROR", 0, 5000, 0 },
                                        { "AT%VLB=1\r", "OK", "ERROR", 0, 5000, 0 }
                                      },
                  astG600 []     =	{
                                        { "ATS96=7\r", "OK", "ERROR", 0, 5000, 0 }, //Echo cancelation
                                        { "ATS94=0\r", "OK", "ERROR", 0, 5000, 0 },   //Side tone effect
                                        { "AT+CLVL", "OK", "ERROR", 0, 5000, 0 }, //Loudspeaker volume
                                        { "AT+MMICG", "OK", "ERROR", 0, 5000, 0 } //Mic gain
                                      },
                  astUC864E []      = {
                                        { "AT#CAP=2\r", "OK", "ERROR", 0, 10000, 0 },
                                        { "AT#SRP=1\r", "OK", "ERROR", 0, 10000, 0 },
                                        { "AT#STM=0\r", "OK", "ERROR", 0, 10000, 0 },
                                        { "AT#DVI=0\r", "OK", "ERROR", 0, 10000, 0 },
                                        { "AT#SHFSD=1\r", "OK", "ERROR", 0, 10000, 0 },
                                        { "AT#SHFEC=1\r", "OK", "ERROR", 0, 10000, 0 },
                                        { "AT#SHSEC=1\r", "OK", "ERROR", 0, 10000, 0 },
                                        { "AT#HSMICG", "OK", "ERROR", 0, 10000, 0 },
                                        { "AT+CLVL", "OK", "ERROR", 0, 5000, 0 }
                                      };

	if (0 < p_pstOPTM->stHelper.iP1_SID)
	{
		ucRet = 0x00;
		goto CleanUp;
	}

	switch (p_pstOPTM->pstCfg->ucModemID)
	{
	case CFG_MODEM_ID_GTM900B_CODE:
		ucRet = hal_gtm900b_attach (&g_stArch.stMobile.stGTM900B, 0x01, 0x0A, 0x01, mod_optm_notify, (void *) p_pstOPTM, &p_pstOPTM->stHelper.iP1_SID);

		if (0x00 != ucRet || 0 >= p_pstOPTM->stHelper.iP1_SID)
		{
			ucRet = 0x02;
			goto CleanUp;
		}

		logger_log (&g_stVoiceLog, LOG_NORMAL, "@%04d %s: Session %08X is attached port 1", __LINE__, MOD_OPTM_NAME, p_pstOPTM->stHelper.iP1_SID);

		pstCmd = astGTM900B;
		iSize = sizeof (astGTM900B) / sizeof (MOD_OPTM_AT_CMD);

		switch (p_pstOPTM->pstCfg->ucVolume)
		{
		case CFG_OPTM_VOLUME_LOW_CODE:
			iMicGain = -4;
			iSpeckerGain = 0;
			iSpeckerVolume = 3;
			break;

		case CFG_OPTM_VOLUME_HIGH_CODE:
			iMicGain = -8;
			iSpeckerGain = 6;
			iSpeckerVolume = 5;
			break;

		case CFG_OPTM_VOLUME_MIDDLE_CODE:
		default:
			iMicGain = -6;
			iSpeckerGain = 3;
			iSpeckerVolume = 4;
		}

		for (iIndex = 0; iSize > iIndex; iIndex++)
		{
			// Replace with configured setting
			if (0 == strcmp (pstCmd [iIndex].acReq, "AT%NFI"))
			{
				sprintf (pstCmd [iIndex].acReq, "AT%%NFI=0,%d,1\r", iMicGain);
			}
			else if (0 == strcmp (pstCmd [iIndex].acReq, "AT%NFO"))
			{
				sprintf (pstCmd [iIndex].acReq, "AT%%NFO=0,%d,0\r", iSpeckerGain);
			}
			else if (0 == strcmp (pstCmd [iIndex].acReq, "AT%NFV"))
			{
				sprintf (pstCmd [iIndex].acReq, "AT%%NFV=%d\r", iSpeckerVolume);
			}
		}

		break;

	case CFG_MODEM_ID_G600_CODE:
		ucRet = hal_g600_attach (&g_stArch.stMobile.stG600, 0x01, 0x0A, 0x01, mod_optm_notify, (void *) p_pstOPTM, &p_pstOPTM->stHelper.iP1_SID);

		if (0x00 != ucRet || 0 >= p_pstOPTM->stHelper.iP1_SID)
		{
			ucRet = 0x02;
			goto CleanUp;
		}

		logger_log (&g_stVoiceLog, LOG_NORMAL, "@%04d %s: Session %08X is attached port 1", __LINE__, MOD_OPTM_NAME, p_pstOPTM->stHelper.iP1_SID);

		pstCmd = astG600;
		iSize = sizeof (astG600) / sizeof (MOD_OPTM_AT_CMD);

		switch (p_pstOPTM->pstCfg->ucVolume)
		{
		case CFG_OPTM_VOLUME_LOW_CODE:
			iMicGain = 5;//2;//6;
//			iSpeckerGain = 0;
			iSpeckerVolume = 5;
			break;

		case CFG_OPTM_VOLUME_HIGH_CODE:
			iMicGain = 9;//8;//12;
//			iSpeckerGain = 6;
			iSpeckerVolume = 7;
			break;

		case CFG_OPTM_VOLUME_MIDDLE_CODE:
		default:
			iMicGain = 7;//5;//8;
//			iSpeckerGain = 3;
			iSpeckerVolume = 7;
		}

		for (iIndex = 0; iSize > iIndex; iIndex++)
		{
			// Replace with configured setting
			if (0 == strcmp (pstCmd [iIndex].acReq, "AT+MMICG"))
			{
				sprintf (pstCmd [iIndex].acReq, "AT+MMICG=%d\r", iMicGain);
			}
			else if (0 == strcmp (pstCmd [iIndex].acReq, "AT+CLVL"))
			{
				sprintf (pstCmd [iIndex].acReq, "AT+CLVL=%d\r", iSpeckerVolume);
			}
		}

		break;

	case CFG_MODEM_ID_UC864E_CODE:
		// If the module is on GPRS mode then take the 1st port to use, else use the 2nd port
		// This is because in GPRS mode, phone call will interferce with data channel,
		// so phone call have to let data channel priority
		if (HAL_UC864E_NETWORK_MODE_GPRS == g_stArch.stMobile.stUC864E.ucNetworkMode)
		{
			ucRet = hal_uc864e_attach (&g_stArch.stMobile.stUC864E, 0x01, 0x0A, 0x01, mod_optm_notify, (void *) p_pstOPTM, &p_pstOPTM->stHelper.iP2_SID);
  		if (0x00 != ucRet || 0 >= p_pstOPTM->stHelper.iP2_SID)
  		{
  			ucRet = 0x02;
  			goto CleanUp;
  		}

  		logger_log (&g_stVoiceLog, LOG_NORMAL, "@%04d %s: Session %08X is attached port 1", __LINE__, MOD_OPTM_NAME, p_pstOPTM->stHelper.iP2_SID);
		}
		else
		{
			ucRet = hal_uc864e_attach (&g_stArch.stMobile.stUC864E, 0x02, 0x0B, 0x01, mod_optm_notify, (void *) p_pstOPTM, &p_pstOPTM->stHelper.iP1_SID);

  		if (0x00 != ucRet || 0 >= p_pstOPTM->stHelper.iP1_SID)
  		{
  			ucRet = 0x02;
  			goto CleanUp;
  		}

  		logger_log (&g_stVoiceLog, LOG_NORMAL, "@%04d %s: Session %08X is attached port 1", __LINE__, MOD_OPTM_NAME, p_pstOPTM->stHelper.iP1_SID);
		}



		if (HAL_UC864E_NETWORK_MODE_GPRS == g_stArch.stMobile.stUC864E.ucNetworkMode)
		{
			ucRet = hal_uc864e_attach (&g_stArch.stMobile.stUC864E, 0x02, 0x0A, 0x01, mod_optm_notify, (void *) p_pstOPTM, &p_pstOPTM->stHelper.iP1_SID);

  		if (0x00 != ucRet || 0 >= p_pstOPTM->stHelper.iP1_SID)
  		{
  			ucRet = 0x02;
  			goto CleanUp;
  		}

  		logger_log (&g_stVoiceLog, LOG_NORMAL, "@%04d %s: Session %08X is attached port 2", __LINE__, MOD_OPTM_NAME, p_pstOPTM->stHelper.iP1_SID);
		}

		pstCmd = astUC864E;
		iSize = sizeof (astUC864E) / sizeof (MOD_OPTM_AT_CMD);

		switch (p_pstOPTM->pstCfg->ucVolume)
		{
		case CFG_OPTM_VOLUME_LOW_CODE:
			iMicGain = 0;
			iSpeckerVolume = 7;
			break;

		case CFG_OPTM_VOLUME_HIGH_CODE:
			iMicGain = 4;
			iSpeckerVolume = 14;
			break;

		case CFG_OPTM_VOLUME_MIDDLE_CODE:
		default:
			iMicGain = 2;
			iSpeckerVolume = 10;
		}

		for (iIndex = 0; iSize > iIndex; iIndex++)
		{
			// Replace with configured setting
			if (0 == strcmp (pstCmd [iIndex].acReq, "AT#HSMICG"))
			{
				sprintf (pstCmd [iIndex].acReq, "AT#HSMICG=%d\r", iMicGain);
			}
			else if (0 == strcmp (pstCmd [iIndex].acReq, "AT+CLVL"))
			{
				sprintf (pstCmd [iIndex].acReq, "AT+CLVL=%d\r", iSpeckerVolume);
			}
		}

		break;

	default:
		logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Unsupported modem ID %d found", __LINE__, MOD_OPTM_NAME, p_pstOPTM->pstCfg->ucModemID);
		ucRet = 0x02;
		goto CleanUp;
	}

/*
  if (CFG_MODEM_ID_UC864E_CODE == p_pstOPTM->pstCfg->ucModemID)
  {
  	if (HAL_UC864E_NETWORK_MODE_HSPDA != g_stArch.stMobile.stUC864E.ucNetworkMode)
        iHelperSID = p_pstOPTM->stHelper.iP2_SID;
  }
*/
	for (iIndex = 0; iSize > iIndex; iIndex++)
	{
		// Execute command
		ucRet = mod_optm_at_cmd (p_pstOPTM, p_pstOPTM->stHelper.iP1_SID, pstCmd [iIndex].acReq, pstCmd [iIndex].acRes, pstCmd [iIndex].acErr, 0, 0, 0,
			pstCmd [iIndex].ulGap, pstCmd [iIndex].ulTimeout, __LINE__);

		if (0x00 != ucRet)
		{
			ucRet = 0x03;
			goto CleanUp;
		}
	}

CleanUp:
  if (ucRet)
    mod_optm_detach (p_pstOPTM);

	return ucRet;
}

unsigned char mod_optm_detach (MOD_OPTM *p_pstOPTM)
{
	unsigned char	ucRet		= 0x01,
					ucDetach	= 0x00;

	if (0 < p_pstOPTM->stHelper.iP1_SID)
	{
		switch (p_pstOPTM->pstCfg->ucModemID)
		{
		case CFG_MODEM_ID_GTM900B_CODE:
			ucRet = hal_gtm900b_detach (&g_stArch.stMobile.stGTM900B, p_pstOPTM->stHelper.iP1_SID);
			break;

		case CFG_MODEM_ID_G600_CODE:
			ucRet = hal_g600_detach (&g_stArch.stMobile.stG600, p_pstOPTM->stHelper.iP1_SID);
			break;

		case CFG_MODEM_ID_UC864E_CODE:
      if (0 != p_pstOPTM->stHelper.iP1_SID)
  			ucRet = hal_uc864e_detach (&g_stArch.stMobile.stUC864E, p_pstOPTM->stHelper.iP1_SID);
      if (0 != p_pstOPTM->stHelper.iP2_SID)
  			ucRet = hal_uc864e_detach (&g_stArch.stMobile.stUC864E, p_pstOPTM->stHelper.iP2_SID);
			break;

		default:
			logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Unsupported modem ID %d found", __LINE__, MOD_OPTM_NAME, p_pstOPTM->pstCfg->ucModemID);
			ucRet = 0x02;
			goto CleanUp;
		}

		p_pstOPTM->stHelper.iP1_SID = 0;
    p_pstOPTM->stHelper.iP2_SID = 0;
		ucDetach = 0x01;
	}

	p_pstOPTM->stHelper.ucDetach = 0x00;

	if (0x01 == ucDetach)
	{
		logger_log (&g_stVoiceLog, LOG_NORMAL, "@%04d %s: Detached port", __LINE__, MOD_OPTM_NAME);
	}

CleanUp:

	return ucRet;
}

unsigned char mod_optm_notify (void *p_pvPtr, int p_iNotifyID, unsigned char p_ucRet, ...)
{
	unsigned char	ucRet		= 0x00;

	int				iSIM;

	va_list			args;

	MOD_OPTM		*pstOPTM	= (MOD_OPTM *) p_pvPtr;

	va_start (args, p_ucRet);

	if (HAL_UC864E_NOTIFY_REQ_DETACH == p_iNotifyID || HAL_GTM900B_NOTIFY_REQ_DETACH == p_iNotifyID || HAL_G600_NOTIFY_REQ_DETACH == p_iNotifyID)
	{
		if (0x00 == pstOPTM->stHelper.ucDetach)
		{
			pstOPTM->stHelper.ucDetach = 0x01;
			logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Session %08X request to detach port", __LINE__, MOD_OPTM_NAME, pstOPTM->stHelper.iP1_SID);
		}
	}
	else if (HAL_UC864E_NOTIFY_FORCED_DETACH == p_iNotifyID || HAL_GTM900B_NOTIFY_FORCED_DETACH == p_iNotifyID || HAL_G600_NOTIFY_FORCED_DETACH == p_iNotifyID)
	{
		if (0x02 != pstOPTM->stHelper.ucDetach)
		{
			pstOPTM->stHelper.ucDetach = 0x02;
			logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Session %08X forced to detach port", __LINE__, MOD_OPTM_NAME, pstOPTM->stHelper.iP1_SID);
		}
	}
	else if (HAL_UC864E_NOTIFY_SIM == p_iNotifyID || HAL_GTM900B_NOTIFY_SIM == p_iNotifyID || HAL_G600_NOTIFY_SIM == p_iNotifyID)
	{
		iSIM = va_arg (args, int);

		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Current active SIM %d", __LINE__, MOD_OPTM_NAME, iSIM);
	}
	else if (HAL_UC864E_NOTIFY_SET_NETWORK_MODE == p_iNotifyID)
	{
	  pstOPTM->stMaster.stOPTM.ucHandshake = 0x00;
	  pstOPTM->stMaster.stOPTM.ucProcState = OPTM_PROC_STATE_HANDSHAKE_CODE;

		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Trigger re-handshake", __LINE__, MOD_OPTM_NAME);
	}
	else
	{
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Session %08X received unknown notify ID %d", __LINE__, MOD_OPTM_NAME, pstOPTM->stHelper.iP1_SID, p_iNotifyID);
	}

	va_end (args);

	return ucRet;
}

void mod_optm_helper_body (void *p_pvWrk)
{
	unsigned char		ucRet;

	unsigned long		ulCurTime;

	MOD_OPTM			*pstOPTM		= ((WRK_THREAD *) p_pvWrk)->pvPtr;

	pstOPTM->stHelper.ucState = MOD_OPTM_HELPER_STATE_IDLE;

	while (0x01 == pstOPTM->stHelper.stThread.ucProcToken)
	{
		// ========================
		// == Catch master event ==
		// ========================

		mod_optm_catch_event (pstOPTM);

		// ====================================
		// == Other module request to detach ==
		// ====================================

		if (MOD_OPTM_HELPER_STATE_IDLE != pstOPTM->stHelper.ucState && 0x00 != pstOPTM->stHelper.ucDetach)
		{
//			if (0x02 == pstOPTM->stHelper.ucDetach)
//			{
				pstOPTM->stHelper.ucState = MOD_OPTM_HELPER_STATE_HANG_UP;
				pstOPTM->stHelper.ucEvent = MOD_OPTM_HELPER_TEL_EVT_GSM_HANG_UP;

				logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: GSM requested forced to detach", __LINE__, MOD_OPTM_NAME);
//			}
		}

		// ===================
		// == PROCESS STATE ==
		// ===================

		switch (pstOPTM->stHelper.ucState)
		{
		case MOD_OPTM_HELPER_STATE_IDLE:
			// == Detect RING ==
			if (0x00 == mod_optm_ring (pstOPTM))
			{
				logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: GSM detected RING", __LINE__, MOD_OPTM_NAME);
				pstOPTM->stHelper.ucState = MOD_OPTM_HELPER_STATE_ANSWER;
			}
			// == Detect off-hook ==
			else if (0x00 == mod_optm_off_hook (pstOPTM))
			{
				logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: TEL detected off-hook", __LINE__, MOD_OPTM_NAME);
				pstOPTM->stHelper.ucState = MOD_OPTM_HELPER_STATE_DIALING;
			}
			else if (0 < pstOPTM->stHelper.iP1_SID)
			{
				IGET_TICKCOUNT (ulCurTime);

				if ((ulCurTime - pstOPTM->stHelper.ulLastCall >= 30000 && ulCurTime >= pstOPTM->stHelper.ulLastCall && 0 != pstOPTM->stHelper.ulLastCall) ||
					0x00 != pstOPTM->stHelper.ucDetach)
				{
					logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: GSM detach port after %ld ms", __LINE__, MOD_OPTM_NAME, ulCurTime - pstOPTM->stHelper.ulLastCall);

					ucRet = mod_optm_detach (pstOPTM);

					if (0x00 != ucRet)
					{
						logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: GSM detach port failure. RetCode [0x%02X]", __LINE__, MOD_OPTM_NAME, ucRet);
					}
					else
					{
						pstOPTM->stHelper.ulLastCall = 0;
						pstOPTM->stHelper.ulLastRING = 0;
					}
				}
				else if (ulCurTime < pstOPTM->stHelper.ulLastCall || 0 == pstOPTM->stHelper.ulLastCall)
				{
					pstOPTM->stHelper.ulLastCall = ulCurTime;
				}
				else
				{
					pstOPTM->stHelper.ucDetach = 0x00;	// Cancel the request detach from other module
				}
			}

			break;

		case MOD_OPTM_HELPER_STATE_DIALING:
			ucRet = mod_optm_dial (pstOPTM);

			if (0x00 == ucRet)
			{
				pstOPTM->stHelper.ucState = MOD_OPTM_HELPER_STATE_ONLINE;

				pstOPTM->stHelper.iRecvBufLen = 0;
				pstOPTM->stHelper.iRecvBufSize = sizeof (pstOPTM->stHelper.acRecvBuf);
				memset (pstOPTM->stHelper.acRecvBuf, 0x00, pstOPTM->stHelper.iRecvBufSize);
			}
			else if (0xFF != ucRet)
			{
				pstOPTM->stHelper.ucState = MOD_OPTM_HELPER_STATE_HANG_UP;
			}

			break;

		case MOD_OPTM_HELPER_STATE_ANSWER:
			ucRet = mod_optm_answer (pstOPTM);

			if (0x00 == ucRet)
			{
				pstOPTM->stHelper.ucState = MOD_OPTM_HELPER_STATE_ONLINE;

				pstOPTM->stHelper.iRecvBufLen = 0;
				pstOPTM->stHelper.iRecvBufSize = sizeof (pstOPTM->stHelper.acRecvBuf);
				memset (pstOPTM->stHelper.acRecvBuf, 0x00, pstOPTM->stHelper.iRecvBufSize);
			}
			else if (0xFF != ucRet)
			{
				pstOPTM->stHelper.ucState = MOD_OPTM_HELPER_STATE_HANG_UP;
			}

			break;

		case MOD_OPTM_HELPER_STATE_ONLINE:
			ucRet = mod_optm_online (pstOPTM);

			if (0x00 != ucRet)
			{
				pstOPTM->stHelper.ucState = MOD_OPTM_HELPER_STATE_HANG_UP;
			}

			break;

		case MOD_OPTM_HELPER_STATE_HANG_UP:
			logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: GSM hang-up", __LINE__, MOD_OPTM_NAME);
			pstOPTM->stHelper.ucEvent = MOD_OPTM_HELPER_TEL_EVT_GSM_HANG_UP;

			if (0x00 == mod_optm_hangup (pstOPTM))
			{
				if (0x01 == pstOPTM->stHelper.ucDetach)
				{
					pstOPTM->stHelper.ucDetach = 0x00;
				}

				memset (pstOPTM->stHelper.acCallID, 0x00, sizeof (pstOPTM->stHelper.acCallID));

				pstOPTM->stHelper.ulLastCall = 0;
				pstOPTM->stHelper.ulLastRING = 0;
				pstOPTM->stHelper.ucEvtFlag = 0x00;

				pstOPTM->stHelper.ucState = MOD_OPTM_HELPER_STATE_IDLE;
			}

			break;
		}

		ISLEEP (1);
	}
}

void mod_optm_master_body (void *p_pvWrk)
{
	unsigned char		ucRet,
						ucProceed,
						ucFailure		= 0x01;

	unsigned long		ulCurTime,
						ulLastRING		= 0;

	int					iParam;

	char				acCallerID [64]	= { 0 };

	MOD_OPTM			*pstOPTM		= ((WRK_THREAD *) p_pvWrk)->pvPtr;

	OPTM_FRAME			stFrame			= { 0 };

	while (0x01 == pstOPTM->stMaster.stThread.ucProcToken)
	{
		ISLEEP (1);

		if (0x01 == ucFailure)
		{
			memset (pstOPTM->stMaster.acTEL_CallID, 0x00, sizeof (pstOPTM->stMaster.acTEL_CallID));

			ucRet = comport_open_voice (&pstOPTM->stMaster.stPort);

			if (0x00 != ucRet)
			{
				logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Open %s OPTM port failure. RetCode [0x%02X]", __LINE__, MOD_OPTM_NAME,
					pstOPTM->stMaster.stPort.acDevFile, ucRet);
			}
			else
			{
				ucFailure = 0x00;
				pstOPTM->stMaster.ucState = MOD_OPTM_MASTER_STATE_IDLE;
			}
		}

		if (0x00 == ucFailure)
		{
			ucProceed = 0x01;

			switch (pstOPTM->stMaster.ucState)
			{
			case MOD_OPTM_MASTER_STATE_IDLE:
				switch (pstOPTM->stHelper.ucEvent)
				{
				case MOD_OPTM_HELPER_TEL_EVT_GSM_RINGING:
					IGET_TICKCOUNT (ulCurTime);

					if ((ulCurTime - ulLastRING >= 3500 && ulCurTime >= ulLastRING) || 0 == ulLastRING)
					{
						ulLastRING = ulCurTime;

						// Dont RING when TEL is off-hook
						if (0x00 == pstOPTM->stMaster.ucTEL_State)
						{
							// Prepare TEL RINGING event
							stFrame.ucClass = OPTM_FRAME_CLS_EVT_TEL_CODE;
							stFrame.ucCmd_Status = OPTM_FRAME_TEL_CMD_GSM_RINGING_CODE;

							memset (acCallerID, 0x00, sizeof (acCallerID));
							strncpy (acCallerID, pstOPTM->stHelper.acCallID, sizeof (acCallerID) - 1);

							iParam = 0;

							if (0 != acCallerID [0])
							{
								stFrame.stParam.astItem [iParam].ucCode = 0x01;
								stFrame.stParam.astItem [iParam].iDataLen = strlen (acCallerID);

								MEM_ALLOC (stFrame.stParam.astItem [iParam].pucData, stFrame.stParam.astItem [iParam].iDataLen, (unsigned char *));
								memcpy (stFrame.stParam.astItem [iParam].pucData, acCallerID, stFrame.stParam.astItem [iParam].iDataLen);

								stFrame.stParam.astItem [iParam].ucIsUsed = 0x01;
								iParam++;
							}

							stFrame.stParam.iCount = iParam;

							ucRet = optm_event (&pstOPTM->stMaster.stOPTM, &stFrame);

							if (0x00 != ucRet)
							{
								logger_log (&g_stVoiceLog, LOG_NORMAL, "@%04d %s: Trigger TEL ring failure [0x%02X:0x%02X]. RetCode [0x%02X]", __LINE__, MOD_OPTM_NAME,
									pstOPTM->stMaster.stOPTM.stSendEvent.stFrame.ucClass, pstOPTM->stMaster.stOPTM.stSendEvent.stFrame.ucCmd_Status, ucRet);
							}
							else
							{
								logger_log (&g_stVoiceLog, LOG_NORMAL, "@%04d %s: Trigger TEL ring %s", __LINE__, MOD_OPTM_NAME, acCallerID);
							}

							optm_del_frame (&pstOPTM->stMaster.stOPTM, &stFrame);
						}
					}
					else if (ulLastRING > ulCurTime)
					{
						ulLastRING = ulCurTime;
					}

					break;

				case MOD_OPTM_HELPER_TEL_EVT_MCU_DIALING:
				case MOD_OPTM_HELPER_TEL_EVT_MCU_DIAL_NO:
				case MOD_OPTM_HELPER_TEL_EVT_MCU_HANG_UP:
				case MOD_OPTM_HELPER_TEL_EVT_MCU_CONNECT:
				case MOD_OPTM_HELPER_TEL_EVT_GSM_CONNECT:
				case MOD_OPTM_HELPER_TEL_EVT_GSM_HANG_UP:
					pstOPTM->stMaster.ucState = MOD_OPTM_MASTER_STATE_ONLINE;
					ucProceed = 0x00;
				}
				break;

			case MOD_OPTM_MASTER_STATE_ONLINE:
				switch (pstOPTM->stHelper.ucEvent)
				{
				case MOD_OPTM_HELPER_TEL_EVT_GSM_RINGING:
					pstOPTM->stMaster.ucState = MOD_OPTM_MASTER_STATE_IDLE;
					ucProceed = 0x00;
					break;

				case MOD_OPTM_HELPER_TEL_EVT_GSM_CONNECT:
					// Prepare GSM CONNECT event
					stFrame.ucClass = OPTM_FRAME_CLS_EVT_TEL_CODE;
					stFrame.ucCmd_Status = OPTM_FRAME_TEL_CMD_GSM_CONNECT_CODE;

					ucRet = optm_event (&pstOPTM->stMaster.stOPTM, &stFrame);

					if (0x00 == ucRet)
					{
						pstOPTM->stHelper.ucEvent = MOD_OPTM_HELPER_TEL_EVT_NONE;
					}

					break;

				case MOD_OPTM_HELPER_TEL_EVT_GSM_HANG_UP:
					// Prepare GSM HANG_UP event
					stFrame.ucClass = OPTM_FRAME_CLS_EVT_TEL_CODE;
					stFrame.ucCmd_Status = OPTM_FRAME_TEL_CMD_GSM_HANG_UP_CODE;

					ucRet = optm_event (&pstOPTM->stMaster.stOPTM, &stFrame);

					if (0x00 == ucRet)
					{
						pstOPTM->stHelper.ucEvent = MOD_OPTM_HELPER_TEL_EVT_NONE;
						pstOPTM->stMaster.ucState = MOD_OPTM_MASTER_STATE_IDLE;
					}

					pstOPTM->stMaster.ucTEL_State = 0x00;

					break;

				case MOD_OPTM_HELPER_TEL_EVT_MCU_HANG_UP:
					pstOPTM->stMaster.ucState = MOD_OPTM_MASTER_STATE_IDLE;
					pstOPTM->stMaster.ucTEL_State = 0x00;
					ucProceed = 0x00;
				}

				break;

			case MOD_OPTM_MASTER_STATE_HANG_UP:
				// Prepare TEL HANGUP event
				stFrame.ucClass = OPTM_FRAME_CLS_EVT_TEL_CODE;
				stFrame.ucCmd_Status = OPTM_FRAME_TEL_CMD_GSM_HANG_UP_CODE;

				ucRet = optm_event (&pstOPTM->stMaster.stOPTM, &stFrame);

				if (0x00 == ucRet)
				{
					pstOPTM->stMaster.ucState = MOD_OPTM_MASTER_STATE_IDLE;
				}

				pstOPTM->stMaster.ucTEL_State = 0x00;
			}

			if (0x01 == ucProceed)
			{
				ucRet = optm_main (&pstOPTM->stMaster.stOPTM);

				if (OPTM_RET_STATUS_READY != ucRet)
				{
					logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: OPTM @%02X process failure. RetCode [0x%02X]", __LINE__, MOD_OPTM_NAME,
						pstOPTM->stMaster.stOPTM.ucProcState, ucRet);

					optm_term (&pstOPTM->stMaster.stOPTM);
					optm_init (&pstOPTM->stMaster.stOPTM, (void *) pstOPTM, mod_optm_raw_recv, 0, mod_optm_raw_send, mod_optm_log,
						mod_optm_malloc, mod_optm_free, mod_optm_memset, mod_optm_memcpy, mod_optm_memmove, mod_optm_memcmp, mod_optm_sleep, mod_optm_get_tick,
						mod_optm_handshake,
						mod_optm_admin_login, mod_optm_admin_logout, mod_optm_admin_chg_pwd,
						mod_optm_auth_pwd, mod_optm_alt_auth_pwd,
						mod_optm_speed_dial_add, mod_optm_speed_dial_del,
						mod_optm_tel_event, mod_optm_pos_event);

					ucFailure = 0x01;
				}
			}
		}
	}
}

void mod_optm_catch_event (MOD_OPTM *p_pstOPTM)
{
	switch (p_pstOPTM->stMaster.ucEvent)
	{
	case MOD_OPTM_MASTER_TEL_EVT_MCU_DIALING:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: OPTM helper catched TEL dialing event", __LINE__, MOD_OPTM_NAME);
		p_pstOPTM->stHelper.ucEvtFlag |= MOD_OPTM_EVT_FLAG_OFF_HOOK;
		p_pstOPTM->stHelper.ucEvtFlag &= (MOD_OPTM_EVT_FLAG_HANG_UP ^ 0xFF);

		break;

	case MOD_OPTM_MASTER_TEL_EVT_MCU_DIAL_NO:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: OPTM helper catched TEL dial number event", __LINE__, MOD_OPTM_NAME);

		if (0 != p_pstOPTM->stMaster.acTEL_CallID [0])
		{
			strncpy (p_pstOPTM->stHelper.acCallID, p_pstOPTM->stMaster.acTEL_CallID, sizeof (p_pstOPTM->stHelper.acCallID) - 1);
			p_pstOPTM->stHelper.ucEvtFlag |= MOD_OPTM_EVT_FLAG_DIAL_NO;
			p_pstOPTM->stHelper.ucEvtFlag &= (MOD_OPTM_EVT_FLAG_HANG_UP ^ 0xFF);

			logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: OPTM helper catched TEL dialing number %s event", __LINE__, MOD_OPTM_NAME,
				p_pstOPTM->stHelper.acCallID);
		}

		break;

	case MOD_OPTM_MASTER_TEL_EVT_MCU_HANG_UP:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: OPTM helper catched TEL hang up event", __LINE__, MOD_OPTM_NAME);
		p_pstOPTM->stHelper.ucEvtFlag |= MOD_OPTM_EVT_FLAG_HANG_UP;
		break;

	case MOD_OPTM_MASTER_TEL_EVT_MCU_CONNECT:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: OPTM helper catched TEL off-hook event", __LINE__, MOD_OPTM_NAME);
		p_pstOPTM->stHelper.ucEvtFlag |= MOD_OPTM_EVT_FLAG_OFF_HOOK;
		break;

	case MOD_OPTM_MASTER_TEL_EVT_MCU_RINGING:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: OPTM helper catched TEL ring event", __LINE__, MOD_OPTM_NAME);
		p_pstOPTM->stHelper.ucEvtFlag |= MOD_OPTM_EVT_FLAG_RINGING;
		break;

	case MOD_OPTM_MASTER_TEL_EVT_GSM_CONNECT:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: OPTM helper catched GSM connect event", __LINE__, MOD_OPTM_NAME);
		p_pstOPTM->stHelper.ucEvtFlag |= MOD_OPTM_EVT_FLAG_CONNECT;
		break;

	case MOD_OPTM_MASTER_TEL_EVT_GSM_HANG_UP:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: OPTM helper catched GSM hang up event", __LINE__, MOD_OPTM_NAME);
		p_pstOPTM->stHelper.ucEvtFlag |= MOD_OPTM_EVT_FLAG_HANG_UP;
		break;
	}

	if (MOD_OPTM_MASTER_TEL_EVT_NONE != p_pstOPTM->stMaster.ucEvent)
	{
		p_pstOPTM->stMaster.ucEvent = MOD_OPTM_MASTER_TEL_EVT_NONE;
	}
}

unsigned char mod_optm_ring (MOD_OPTM *p_pstOPTM)
{
	unsigned char	ucRet;

	unsigned long	ulCurTime;

	// GSM modem RING
	switch (p_pstOPTM->pstCfg->ucModemID)
	{
	case CFG_MODEM_ID_GTM900B_CODE:
		ucRet = hal_gtm900b_ring (&g_stArch.stMobile.stGTM900B);
		break;

	case CFG_MODEM_ID_G600_CODE:
		ucRet = hal_g600_ring (&g_stArch.stMobile.stG600);
		break;

	case CFG_MODEM_ID_UC864E_CODE:
		ucRet = hal_uc864e_ring (&g_stArch.stMobile.stUC864E);
		break;

	default:
		ucRet = 0xFF;
	}

	IGET_TICKCOUNT (ulCurTime);

	// The RING will be in pulse sequence, ON...OFF...ON...OFF
	if (0x01 == ucRet)
	{
		if (ulCurTime - p_pstOPTM->stHelper.ulLastRING < 5000 && ulCurTime >= p_pstOPTM->stHelper.ulLastRING && 0 != p_pstOPTM->stHelper.ulLastRING)
		{
			ucRet = 0x00;
		}
		else if (ulCurTime < p_pstOPTM->stHelper.ulLastRING)
		{
			p_pstOPTM->stHelper.ulLastRING = ulCurTime;
		}
	}
	else
	{
		p_pstOPTM->stHelper.ulLastRING = ulCurTime;
	}

	if (0x00 == ucRet)
	{
		if (0 < g_stCoRel.stCoRels.iCount)
		{
			logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: GSM is ringing during transaction in progress", __LINE__, MOD_OPTM_NAME);

			// Activate buzzer during trasaction process
			hal_buzzer (&g_stArch.stHI.stBuzzer, 1000, 1000, 5000, 0x01, 0x00, HAL_BUZZER_STATE_IDLE, __LINE__);

			ucRet = 0x01;	// Disable the RING
		}
	}

	return ucRet;
}

unsigned char mod_optm_off_hook (MOD_OPTM *p_pstOPTM)
{
	return ((MOD_OPTM_EVT_FLAG_OFF_HOOK | MOD_OPTM_EVT_FLAG_DIAL_NO) & p_pstOPTM->stHelper.ucEvtFlag) ? 0x00 : 0x01;
}

unsigned char mod_optm_dial (MOD_OPTM *p_pstOPTM)
{
	unsigned char		ucRet;

	int					iIndex,
						iSize			= 0;

	MOD_OPTM_AT_CMD		*pstCmd			= 0,

						astUC864E []	=	{
												{ "ATD", "OK\tCONNECT", "ERROR\tNO CARRIER\tBUSY\tNO DIAL TONE", 0, 30000, 0 }
											},

						astGTM900B []	=	{
												{ "ATD", "OK\tCONNECT", "ERROR\tNO CARRIER\tBUSY\tNO DIAL TONE", 0, 30000, 0 }
											},

						astG600 []	=	{
												{ "ATD", "OK\tCONNECT", "ERROR\tNO CARRIER\tBUSY\tNO DIAL TONE", 0, 30000, 0 }
											};

	ucRet = mod_optm_attach (p_pstOPTM);

	if (0x00 != ucRet || 0 >= p_pstOPTM->stHelper.iP1_SID)
	{
		ucRet = 0xFF;
		goto CleanUp;
	}
	else if (0 == p_pstOPTM->stHelper.acCallID [0])
	{
		ucRet = 0xFF;
		goto CleanUp;
	}
	else if (MOD_OPTM_EVT_FLAG_HANG_UP & p_pstOPTM->stHelper.ucEvtFlag)
	{
		ucRet = 0x01;	// Hang up event happen
		goto CleanUp;
	}

	switch (p_pstOPTM->pstCfg->ucModemID)
	{
	case CFG_MODEM_ID_GTM900B_CODE:
		pstCmd = astGTM900B;
		iSize = sizeof (astGTM900B) / sizeof (MOD_OPTM_AT_CMD);
		break;

	case CFG_MODEM_ID_G600_CODE:
		pstCmd = astG600;
		iSize = sizeof (astG600) / sizeof (MOD_OPTM_AT_CMD);
		break;

	case CFG_MODEM_ID_UC864E_CODE:
		pstCmd = astUC864E;
		iSize = sizeof (astUC864E) / sizeof (MOD_OPTM_AT_CMD);
		break;
	}

	for (iIndex = 0; iSize > iIndex; iIndex++)
	{
		// Replace with configured setting
		if (0 == strcmp (pstCmd [iIndex].acReq, "ATD"))
		{
			sprintf (pstCmd [iIndex].acReq, "ATD%s;\r", p_pstOPTM->stHelper.acCallID);
		}

		// Execute command
		ucRet = mod_optm_at_cmd (p_pstOPTM, p_pstOPTM->stHelper.iP1_SID, pstCmd [iIndex].acReq, pstCmd [iIndex].acRes, pstCmd [iIndex].acErr, 0, 0, 0,
			pstCmd [iIndex].ulGap, pstCmd [iIndex].ulTimeout, __LINE__);

		if (0x00 != ucRet)
		{
			ucRet = 0x02;
			goto CleanUp;
		}
	}

	ucRet = 0x00;

CleanUp:

	return ucRet;
}

unsigned char mod_optm_answer (MOD_OPTM *p_pstOPTM)
{
	unsigned char		ucRet;

	int					iIndex,
						iSize;

	char				acRet [128]	= { 0 },
						*pcStart,
						*pcEnd,
						*pcPtr;

	MOD_OPTM_AT_CMD		astCallID []	=	{
												{ "AT+CLCC\r", "OK", "ERROR", 0, 20000, 2 }
											},

						astAnswer []	=	{
												{ "ATA\r", "OK\tCONNECT", "ERROR\tNO CARRIER\tBUSY\tNO DIAL TONE", 0, 20000, 0 }
											},

						*pstCmd;

	if (0x00 != mod_optm_ring (p_pstOPTM))
	{
		ucRet = 0x01;
		goto CleanUp;
	}

	ucRet = mod_optm_attach (p_pstOPTM);

	if (0x00 != ucRet || 0 >= p_pstOPTM->stHelper.iP1_SID)
	{
		ucRet = 0xFF;
		goto CleanUp;
	}

	if (0 == p_pstOPTM->stHelper.acCallID [0])
	{
		pstCmd = astCallID;
		iSize = sizeof (astCallID) / sizeof (MOD_OPTM_AT_CMD);

		for (iIndex = 0; iSize > iIndex; iIndex++)
		{
			// Execute command
			ucRet = mod_optm_at_cmd (p_pstOPTM, p_pstOPTM->stHelper.iP1_SID, pstCmd [iIndex].acReq, pstCmd [iIndex].acRes, pstCmd [iIndex].acErr, acRet, sizeof (acRet) - 1, 0,
				pstCmd [iIndex].ulGap, pstCmd [iIndex].ulTimeout, __LINE__);

			if (0x00 != ucRet)
			{
				if (0 < pstCmd [iIndex].iRetry--)
				{
					iIndex--;
					continue;
				}

				ucRet = 0x02;
				goto CleanUp;
			}

			// Get the caller number
			if (0 == strcmp (pstCmd [iIndex].acReq, "AT+CLCC\r"))
			{
				pcStart = strstr (acRet, "+CLCC: ");

				if (0 != pcStart)
				{
					pcStart += 7;
					pcStart = strstr (pcStart, "\"");

					if (0 != pcStart)
					{
						pcStart += 1;
						pcEnd = strchr (pcStart, '"');

						if (0 != pcEnd && 0 < (pcEnd - pcStart) && sizeof (acRet) > (pcEnd - pcStart))
						{
							memset (p_pstOPTM->stHelper.acCallID, 0x00, sizeof (p_pstOPTM->stHelper.acCallID));
							pcPtr = p_pstOPTM->stHelper.acCallID;

							while (pcStart < pcEnd)
							{
								if ('0' <= *pcStart && '9' >= *pcStart)
								{
									*(pcPtr++) = *pcStart;
								}

								pcStart++;
							}
						}
					}
				}

				if (0 == p_pstOPTM->stHelper.acCallID [0])
				{
					if (0 < pstCmd [iIndex].iRetry--)
					{
						iIndex--;
						continue;
					}
				}
			}
		}
	}

	// Trigger the MCU to RING
	p_pstOPTM->stHelper.ucEvent = MOD_OPTM_HELPER_TEL_EVT_GSM_RINGING;

	// Detected TEL off-hook
	if (MOD_OPTM_EVT_FLAG_OFF_HOOK & p_pstOPTM->stHelper.ucEvtFlag)
	{
		pstCmd = astAnswer;
		iSize = sizeof (astAnswer) / sizeof (MOD_OPTM_AT_CMD);

		for (iIndex = 0; iSize > iIndex; iIndex++)
		{
			// Execute command
			ucRet = mod_optm_at_cmd (p_pstOPTM, p_pstOPTM->stHelper.iP1_SID, pstCmd [iIndex].acReq, pstCmd [iIndex].acRes, pstCmd [iIndex].acErr, acRet, sizeof (acRet) - 1, 0,
				pstCmd [iIndex].ulGap, pstCmd [iIndex].ulTimeout, __LINE__);

			if (0x00 != ucRet)
			{
				if (0 < pstCmd [iIndex].iRetry--)
				{
					iIndex--;
					continue;
				}

				ucRet = 0x02;
				goto CleanUp;
			}
		}

		goto CleanUp;
	}

	ucRet = 0xFF;

CleanUp:

	return ucRet;
}

unsigned char mod_optm_hangup (MOD_OPTM *p_pstOPTM)
{
	unsigned char		ucRet;

	int					iIndex;

	MOD_OPTM_AT_CMD		astCmd []	= {
										{ "ATH\r", "OK\tNO CARRIER", "NO CARRIER\tBUSY\tNO DIAL TONE", 0, 10000, 0 }
	                  };

	MOD_OPTM_AT_CMD		astCmd_g600 []	= {
										{ "AT+MHUP=17,1\r", "OK\tERROR\tNO CARRIER", "BUSY\tNO DIAL TONE", 0, 10000, 0 }
	                  };



	if (0 < p_pstOPTM->stHelper.iP1_SID)
	{
  	switch (p_pstOPTM->pstCfg->ucModemID)
  	{
    	case CFG_MODEM_ID_GTM900B_CODE:
    	case CFG_MODEM_ID_UC864E_CODE:
    		for (iIndex = 0; sizeof (astCmd) / sizeof (MOD_OPTM_AT_CMD) > iIndex; iIndex++)
    		{
    			ucRet = mod_optm_at_cmd (p_pstOPTM, p_pstOPTM->stHelper.iP1_SID, astCmd [iIndex].acReq, astCmd [iIndex].acRes, astCmd [iIndex].acErr, 0, 0, 0,
    				astCmd [iIndex].ulGap, astCmd [iIndex].ulTimeout, __LINE__);
    			// Execute command

    			if (0x00 != ucRet)
    			{
    				ucRet = 0x01;
    				goto CleanUp;
    			}
    		}
        break;

    	case CFG_MODEM_ID_G600_CODE:
    		for (iIndex = 0; sizeof (astCmd_g600) / sizeof (MOD_OPTM_AT_CMD) > iIndex; iIndex++)
    		{
    			ucRet = mod_optm_at_cmd (p_pstOPTM, p_pstOPTM->stHelper.iP1_SID, astCmd_g600 [iIndex].acReq, astCmd_g600 [iIndex].acRes, astCmd_g600 [iIndex].acErr, 0, 0, 0,
    				astCmd_g600 [iIndex].ulGap, astCmd_g600 [iIndex].ulTimeout, __LINE__);
    			// Execute command

    			if (0x00 != ucRet)
    			{
    				ucRet = 0x01;
    				goto CleanUp;
    			}
    		}
        break;
      default:
    		ucRet = 0xFF;
        goto CleanUp;
  	}
	}

	ucRet = 0x00;

CleanUp:

	return ucRet;
}

unsigned char mod_optm_online (MOD_OPTM *p_pstOPTM)
{
	unsigned char		ucRet		= 0x01,
						ucRecv;

	int					iRecv		= 0;

	if (MOD_OPTM_EVT_FLAG_HANG_UP & p_pstOPTM->stHelper.ucEvtFlag)
	{
		goto CleanUp;
	}

	// Detect connected result
	switch (p_pstOPTM->pstCfg->ucModemID)
	{
	case CFG_MODEM_ID_GTM900B_CODE:
		ucRet = hal_gtm900b_recv (&g_stArch.stMobile.stGTM900B, p_pstOPTM->stHelper.iP1_SID, &ucRecv, 1, &iRecv, 10);
		break;

	case CFG_MODEM_ID_G600_CODE:
		ucRet = hal_g600_recv (&g_stArch.stMobile.stG600, p_pstOPTM->stHelper.iP1_SID, &ucRecv, 1, &iRecv, 10);
		break;

	case CFG_MODEM_ID_UC864E_CODE:
		ucRet = hal_uc864e_recv (&g_stArch.stMobile.stUC864E, p_pstOPTM->stHelper.iP1_SID, &ucRecv, 1, &iRecv, 10);
		break;
	}

	if (0x00 != ucRet)
	{
		ucRet = 0x02;
		goto CleanUp;
	}

	if (1 == iRecv)
	{
		logger_log (&g_stVoiceLog, LOG_DETAIL, "@%04d %s: OPTM helper received data [0x%02X]", __LINE__, MOD_OPTM_NAME, ucRecv);

		if (p_pstOPTM->stHelper.iRecvBufLen == p_pstOPTM->stHelper.iRecvBufSize)
		{
			memmove (p_pstOPTM->stHelper.acRecvBuf, p_pstOPTM->stHelper.acRecvBuf + 1, p_pstOPTM->stHelper.iRecvBufSize - 1);
			p_pstOPTM->stHelper.iRecvBufLen--;
		}

		p_pstOPTM->stHelper.acRecvBuf [p_pstOPTM->stHelper.iRecvBufLen++] = (char) ucRecv;

		if (0x00 == at_match_voice (p_pstOPTM->stHelper.acRecvBuf, "NO CARRIER	NO ANSWER	BUSY	NO DIAL TONE	ERROR"))
		{
			memset (p_pstOPTM->stHelper.acRecvBuf, 0x00, p_pstOPTM->stHelper.iRecvBufSize);
			p_pstOPTM->stHelper.iRecvBufLen = 0;

			logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: OPTM helper catched GSM HANG UP event", __LINE__, MOD_OPTM_NAME);
			ucRet = 0x03;
			goto CleanUp;
		}
		else if (0x00 == at_match_voice (p_pstOPTM->stHelper.acRecvBuf, "OK	CONNECT"))
		{
			memset (p_pstOPTM->stHelper.acRecvBuf, 0x00, p_pstOPTM->stHelper.iRecvBufSize);
			p_pstOPTM->stHelper.iRecvBufLen = 0;
			p_pstOPTM->stHelper.ucEvent = MOD_OPTM_HELPER_TEL_EVT_GSM_CONNECT;

			logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: OPTM helper catched GSM CONNECT event", __LINE__, MOD_OPTM_NAME);
		}
	}

	ucRet = 0x00;

CleanUp:

	return ucRet;
}
