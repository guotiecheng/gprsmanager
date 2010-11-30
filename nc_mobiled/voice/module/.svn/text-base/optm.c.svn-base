#include "optm.h"

unsigned char optm_init (OPTM *p_pstOPTM, void *p_pvBasket,
						 OPTM_RAW_RECV p_pf_raw_recv, OPTM_CHK_SEND p_pf_chk_send, OPTM_RAW_SEND p_pf_raw_send, OPTM_LOGEVENT p_pf_log_event,
						 OPTM_MALLOC p_pf_malloc, OPTM_FREE p_pf_free, OPTM_MEMSET p_pf_memset, OPTM_MEMCPY p_pf_memcpy,
						 OPTM_MEMMOVE p_pf_memmove, OPTM_MEMCMP p_pf_memcmp, OPTM_SLEEP p_pf_sleep, OPTM_GETTICK p_pf_get_tick,
						 OPTM_HANDSHAKE p_pf_handshake,
						 OPTM_ADMIN_LOGIN p_pf_admin_login, OPTM_ADMIN_LOGOUT p_pf_admin_logout, OPTM_ADMIN_CHG_PWD p_pf_admin_chg_pwd,
						 OPTM_AUTH_PWD p_pf_auth_pwd, OPTM_ALT_AUTH_PWD p_pf_alt_auth_pwd,
						 OPTM_SPEED_DIAL_ADD p_pf_speed_dial_add, OPTM_SPEED_DIAL_DEL p_pf_speed_dial_del,
						 OPTM_TEL_EVENT p_pf_tel_event, OPTM_POS_EVENT p_pf_pos_event)
{
	if (0 == p_pstOPTM || 0 == p_pf_raw_recv || 0 == p_pf_raw_send || 0 == p_pf_log_event ||
		0 == p_pf_malloc || 0 == p_pf_free || 0 == p_pf_memset || 0 == p_pf_memcpy || 0 == p_pf_memmove || 0 == p_pf_memcmp || 0 == p_pf_sleep || 0 == p_pf_get_tick ||
		0 == p_pf_handshake || 0 == p_pf_admin_login || 0 == p_pf_admin_logout || 0 == p_pf_admin_chg_pwd ||
		0 == p_pf_auth_pwd || 0 == p_pf_alt_auth_pwd ||
		0 == p_pf_speed_dial_add || 0 == p_pf_speed_dial_del ||
		0 == p_pf_tel_event || 0 == p_pf_pos_event)
	{
		return 0x01;
	}

	p_pf_memset (p_pstOPTM, 0x00, sizeof (OPTM));

	p_pstOPTM->pf_raw_recv = p_pf_raw_recv;
	p_pstOPTM->pf_chk_send = p_pf_chk_send;
	p_pstOPTM->pf_raw_send = p_pf_raw_send;

	p_pstOPTM->pf_log_event = p_pf_log_event;

	p_pstOPTM->pf_handshake = p_pf_handshake;

	p_pstOPTM->pf_admin_login = p_pf_admin_login;
	p_pstOPTM->pf_admin_logout = p_pf_admin_logout;
	p_pstOPTM->pf_admin_chg_pwd = p_pf_admin_chg_pwd;

	p_pstOPTM->pf_auth_pwd = p_pf_auth_pwd;
	p_pstOPTM->pf_alt_auth_pwd = p_pf_alt_auth_pwd;

	p_pstOPTM->pf_speed_dial_add = p_pf_speed_dial_add;
	p_pstOPTM->pf_speed_dial_del = p_pf_speed_dial_del;

	p_pstOPTM->pf_tel_event = p_pf_tel_event;
	p_pstOPTM->pf_pos_event = p_pf_pos_event;

	p_pstOPTM->pf_malloc = p_pf_malloc;
	p_pstOPTM->pf_free = p_pf_free;
	p_pstOPTM->pf_memset = p_pf_memset;
	p_pstOPTM->pf_memcpy = p_pf_memcpy;
	p_pstOPTM->pf_memmove = p_pf_memmove;
	p_pstOPTM->pf_memcmp = p_pf_memcmp;

	p_pstOPTM->pf_sleep = p_pf_sleep;
	p_pstOPTM->pf_get_tick = p_pf_get_tick;
	
	p_pstOPTM->pvBasket = p_pvBasket;
	
	p_pstOPTM->ucProcState = OPTM_PROC_STATE_HANDSHAKE_CODE;
	p_pstOPTM->iMagic = OPTM_MAGIC_CODE;

	return 0x00;
}

void optm_term (OPTM *p_pstOPTM)
{
	if (OPTM_MAGIC_CODE != p_pstOPTM->iMagic || OPTM_PROC_STATE_TERM_CODE == p_pstOPTM->ucProcState)
	{
		return;
	}

	optm_del_frame (p_pstOPTM, &p_pstOPTM->stRecvFrame);
	optm_del_frame (p_pstOPTM, &p_pstOPTM->stSendFrame);
	optm_del_frame (p_pstOPTM, &p_pstOPTM->stSendCache.stFrame);
	optm_del_frame (p_pstOPTM, &p_pstOPTM->stSendEvent.stFrame);

	p_pstOPTM->pf_memset (p_pstOPTM, 0x00, sizeof (OPTM));
}

unsigned char optm_parse (OPTM *p_pstOPTM, int *p_puiUsedLen)
{
	unsigned char	ucRet				= 0x00,
					ucLRC,
					*pucParamPtr,
					*pucParamFrm,
					*pucParamEnd;

	int				iParamLen			= -1,
					iParamCount			= 0;

	unsigned int	uiDataLen,
					uiCount				= 0;

	*p_puiUsedLen = 0;

	if (0 == p_pstOPTM->uiRecvBufLen)
	{
		goto CleanUp;
	}

	// Clear data not start with OPTM_FRAME_STX_CODE
	while (OPTM_FRAME_STX_CODE != p_pstOPTM->aucRecvBuf [uiCount])
	{
		if (++uiCount == p_pstOPTM->uiRecvBufLen)
		{
			break;
		}
	}

	if (0 != uiCount)
	{
		*p_puiUsedLen = uiCount;
		ucRet = 0x01;
		goto CleanUp;
	}

	// Skip uncomplete data length
	if (7 > p_pstOPTM->uiRecvBufLen)
	{
		goto CleanUp;
	}

	uiDataLen = (unsigned int) (p_pstOPTM->aucRecvBuf [1] * 256) + p_pstOPTM->aucRecvBuf [2];

	// Invalid data length remove all
	if (OPTM_MAX_RECV_BUF < uiDataLen)
	{
		*p_puiUsedLen = p_pstOPTM->uiRecvBufLen;
		ucRet = 0x02;
		goto CleanUp;
	}

	// Data not complete
	if (uiDataLen + 3 > p_pstOPTM->uiRecvBufLen)
	{
		goto CleanUp;
	}

	*p_puiUsedLen = uiDataLen + 3;

	// Invalid ETX remove all
	if (OPTM_FRAME_ETX_CODE != p_pstOPTM->aucRecvBuf [*p_puiUsedLen - 2])
	{
		ucRet = 0x03;
		goto CleanUp;
	}

	ucLRC = optm_gen_lrc (p_pstOPTM, p_pstOPTM->aucRecvBuf + 1, *p_puiUsedLen - 2);

	// LRC error
	if (ucLRC != *(p_pstOPTM->aucRecvBuf + *p_puiUsedLen - 1))
	{
		p_pstOPTM->stRecvFrame.ucClass = OPTM_FRAME_CLS_CORRUPTED_CODE;
		goto CleanUp;
	}
	else
	{
		p_pstOPTM->stRecvFrame.ucClass = *(p_pstOPTM->aucRecvBuf + 3);
	}

	p_pstOPTM->stRecvFrame.ucCmd_Status = *(p_pstOPTM->aucRecvBuf + 4);

	// Parse Parameters
	optm_del_param (p_pstOPTM, 0xFF, &p_pstOPTM->stRecvFrame.stParam);

	if (7 < *p_puiUsedLen)
	{
		pucParamFrm = 0;
		pucParamEnd = p_pstOPTM->aucRecvBuf + (*p_puiUsedLen - 1);
		pucParamPtr = p_pstOPTM->aucRecvBuf + 4;

		while (pucParamPtr < pucParamEnd)
		{
			if (0xFF == *pucParamPtr || pucParamPtr == (pucParamEnd - 1))
			{
				if (0 == pucParamFrm)
				{
					pucParamFrm = pucParamPtr + 1;
				}
				else
				{
					iParamLen = pucParamPtr - pucParamFrm;

					if (0 < iParamLen) //Previous parameter exist, Add Parameter to List
					{
						p_pstOPTM->stRecvFrame.stParam.astItem [iParamCount].iDataLen = iParamLen - 1;
						p_pstOPTM->stRecvFrame.stParam.astItem [iParamCount].ucIsUsed = 0x01;
						p_pstOPTM->stRecvFrame.stParam.astItem [iParamCount].ucCode = *pucParamFrm;

						p_pstOPTM->pf_free (p_pstOPTM->stRecvFrame.stParam.astItem [iParamCount].pucData);
						p_pstOPTM->stRecvFrame.stParam.astItem [iParamCount].pucData = 0;

						if (1 < iParamLen)
						{
							p_pstOPTM->stRecvFrame.stParam.astItem [iParamCount].pucData = p_pstOPTM->pf_malloc (iParamLen - 1);
							p_pstOPTM->pf_memcpy (p_pstOPTM->stRecvFrame.stParam.astItem [iParamCount].pucData, pucParamFrm + 1, iParamLen - 1);
						}

						iParamCount++;
						pucParamFrm = pucParamPtr + 1;
					}
				}
			}

			pucParamPtr++;
		}
	}

	p_pstOPTM->stRecvFrame.stParam.iCount = iParamCount;

CleanUp:

	return ucRet;
}

unsigned char optm_build (OPTM *p_pstOPTM, unsigned char *p_pucBuf, unsigned int p_uiBufSize, unsigned int *p_puiBufLen)
{
	unsigned char	ucRet			= 0x00;

	unsigned int	uiBufLen		= 0;

	int				iParamCount;

	OPTM_PARAM_ITEM	*pstItem;

	*p_pucBuf = OPTM_FRAME_STX_CODE;
	uiBufLen++;

	// Skip data lenght add in before lrc generation
	uiBufLen += 2;

	*(p_pucBuf + uiBufLen) = p_pstOPTM->stSendFrame.ucClass;
	uiBufLen++;
	*(p_pucBuf + uiBufLen) = p_pstOPTM->stSendFrame.ucCmd_Status;
	uiBufLen++;

	for (iParamCount = 0; iParamCount < OPTM_MAX_PARAM; iParamCount++)
	{
		if (0x01 == p_pstOPTM->stSendFrame.stParam.astItem [iParamCount].ucIsUsed)
		{
			pstItem = &p_pstOPTM->stSendFrame.stParam.astItem [iParamCount];

			if (p_uiBufSize < uiBufLen + pstItem->iDataLen + 1)
			{
				ucRet = 0x01;
				goto CleanUp;
			}

			*(p_pucBuf + uiBufLen) = 0xFF;
			uiBufLen++;

			*(p_pucBuf + uiBufLen) = pstItem->ucCode;
			uiBufLen++;

			p_pstOPTM->pf_memcpy (p_pucBuf + uiBufLen, pstItem->pucData, pstItem->iDataLen);
			uiBufLen += pstItem->iDataLen;
		}
	}

	*(p_pucBuf + uiBufLen) = OPTM_FRAME_ETX_CODE;
	uiBufLen++;

	*(p_pucBuf + 1)	= (unsigned char) ((uiBufLen - 2) / 256);
	*(p_pucBuf + 2)	= (unsigned char) ((uiBufLen - 2) % 256);

	*(p_pucBuf + uiBufLen) = optm_gen_lrc (p_pstOPTM, p_pucBuf + 1, uiBufLen - 1);
	uiBufLen++;

	*p_puiBufLen = uiBufLen;

CleanUp:

	return ucRet;
}

unsigned char optm_gen_lrc (OPTM *p_pstOPTM, unsigned char *p_pucData, unsigned int p_uiDataLen)
{
	unsigned char	ucRet = 0x00;

	unsigned int	uiIndex;

	for (uiIndex = 0; uiIndex < p_uiDataLen; uiIndex++)
	{
		ucRet ^= *(p_pucData + uiIndex);
	}

	return ucRet;
}

unsigned char optm_proc_handshake (OPTM *p_pstOPTM)
{
	unsigned char	ucRet;

	unsigned long	ulCurTime;

	if (OPTM_FRAME_CLS_HANDSHAKE_CODE == p_pstOPTM->stRecvFrame.ucClass)
	{
		p_pstOPTM->pf_get_tick (&p_pstOPTM->ulLastOpTime);

		switch (p_pstOPTM->stRecvFrame.ucCmd_Status)
		{
		case OPTM_FRAME_STA_ACK_CODE:
			p_pstOPTM->pf_log_event (p_pstOPTM, OPTM_LOG_HANDSHAKE_SUCCESS_CODE, __LINE__, 0x00);
			p_pstOPTM->ucProcState = OPTM_PROC_STATE_IDLE_CODE;
			p_pstOPTM->ucHandshake = 0x01;
			break;

		case OPTM_FRAME_STA_NAK_CODE:
			p_pstOPTM->pf_log_event (p_pstOPTM, OPTM_LOG_HANDSHAKE_ERROR_CODE, __LINE__, 0x00);
		}
	}
	else if (OPTM_FRAME_CLS_NONE_CODE == p_pstOPTM->stRecvFrame.ucClass)
	{
		if (0x00 == p_pstOPTM->ucHandshake)
		{
			p_pstOPTM->pf_get_tick (&ulCurTime);

			if ((ulCurTime - p_pstOPTM->ulLastOpTime >= OPTM_TIME_HANDSHAKE_TIMEOUT && ulCurTime >= p_pstOPTM->ulLastOpTime) || 0 == p_pstOPTM->ulLastOpTime)
			{
				if (0 != p_pstOPTM->ulLastOpTime)
				{
					p_pstOPTM->pf_log_event (p_pstOPTM, OPTM_LOG_HANDSHAKE_TIMEOUT_CODE, __LINE__, 0x00);
				}

				p_pstOPTM->ulLastOpTime = ulCurTime;

				// Prepare HANDSHAKE frame
				ucRet = p_pstOPTM->pf_handshake (p_pstOPTM);

				if (0x00 != ucRet)
				{
					p_pstOPTM->pf_log_event (p_pstOPTM, OPTM_LOG_HANDSHAKE_ERROR_CODE, __LINE__, ucRet);
				}
			}
			else if (ulCurTime < p_pstOPTM->ulLastOpTime)
			{
				p_pstOPTM->ulLastOpTime = ulCurTime;
			}
		}
	}
	else
	{
		p_pstOPTM->ucProcState = OPTM_PROC_STATE_IDLE_CODE;
		p_pstOPTM->ucLoopState = 0x01;
	}

	return 0x00;
}

unsigned char optm_proc_idle (OPTM *p_pstOPTM)
{
	switch (p_pstOPTM->stRecvFrame.ucClass)
	{
	case OPTM_FRAME_CLS_HANDSHAKE_CODE:
		p_pstOPTM->ucProcState = OPTM_PROC_STATE_HANDSHAKE_CODE;
		p_pstOPTM->ucLoopState = 0x01;
		break;

	case OPTM_FRAME_CLS_ADMIN_CODE:
		p_pstOPTM->ucProcState = OPTM_PROC_STATE_ADMIN_CODE;
		p_pstOPTM->ucLoopState = 0x01;
		break;

	case OPTM_FRAME_CLS_AUTH_CODE:
		p_pstOPTM->ucProcState = OPTM_PROC_STATE_AUTH_CODE;
		p_pstOPTM->ucLoopState = 0x01;
		break;

	case OPTM_FRAME_CLS_SPEED_DIAL_CODE:
		p_pstOPTM->ucProcState = OPTM_PROC_STATE_SPEED_DIAL_CODE;
		p_pstOPTM->ucLoopState = 0x01;
		break;

	case OPTM_FRAME_CLS_EVT_TEL_CODE:
		p_pstOPTM->ucProcState = OPTM_PROC_STATE_TEL_CODE;
		p_pstOPTM->ucLoopState = 0x01;
		break;

	case OPTM_FRAME_CLS_EVT_POS_CODE:
		p_pstOPTM->ucProcState = OPTM_PROC_STATE_POS_CODE;
		p_pstOPTM->ucLoopState = 0x01;
		break;

	case OPTM_FRAME_CLS_EVT_FAX_CODE:
		p_pstOPTM->ucProcState = OPTM_PROC_STATE_FAX_CODE;
		p_pstOPTM->ucLoopState = 0x01;
		break;
	}

	return 0x00;
}

unsigned char optm_proc_admin (OPTM *p_pstOPTM)
{
	unsigned char		ucRet;

	OPTM_PARAM_ITEM		*pstParam1		= 0;

	if (OPTM_FRAME_CLS_ADMIN_CODE == p_pstOPTM->stRecvFrame.ucClass)
	{
		switch (p_pstOPTM->stRecvFrame.ucCmd_Status)
		{
		case OPTM_FRAME_ADMIN_CMD_LOGIN_CODE:
			ucRet = 0xFF;

			optm_get_param (p_pstOPTM, 0x01, &p_pstOPTM->stRecvFrame.stParam, &pstParam1);

			if (0 != pstParam1)
			{
				if (0 != pstParam1->pucData)
				{
					// Check admin password
					ucRet = p_pstOPTM->pf_admin_login (p_pstOPTM, pstParam1->pucData, pstParam1->iDataLen);
				}
			}

			switch (ucRet)
			{
			case 0x00:	// Success
				p_pstOPTM->pf_log_event (p_pstOPTM, OPTM_LOG_ADMIN_LOGIN_SUCCESS_CODE, __LINE__, ucRet);
				break;

			case 0x01:	// Rejected
				p_pstOPTM->pf_log_event (p_pstOPTM, OPTM_LOG_ADMIN_LOGIN_REJECT_CODE, __LINE__, ucRet);
				break;

			default:	// Error
				p_pstOPTM->pf_log_event (p_pstOPTM, OPTM_LOG_ADMIN_LOGIN_ERROR_CODE, __LINE__, ucRet);
				break;
			}

			// Prepare admin login response frame
			p_pstOPTM->stSendFrame.ucClass = OPTM_FRAME_CLS_ADMIN_CODE;
			p_pstOPTM->stSendFrame.ucCmd_Status = (0x00 == ucRet) ? OPTM_FRAME_STA_ACK_CODE : OPTM_FRAME_STA_NAK_CODE;

			p_pstOPTM->ucProcState = OPTM_PROC_STATE_IDLE_CODE;
			break;

		case OPTM_FRAME_ADMIN_CMD_LOGOUT_CODE:
			ucRet = 0xFF;

			ucRet = p_pstOPTM->pf_admin_logout (p_pstOPTM);

			switch (ucRet)
			{
			case 0x00:	// Success
				p_pstOPTM->pf_log_event (p_pstOPTM, OPTM_LOG_ADMIN_LOGOUT_SUCCESS_CODE, __LINE__, ucRet);
				break;

			case 0x01:	// Rejected
				p_pstOPTM->pf_log_event (p_pstOPTM, OPTM_LOG_ADMIN_LOGOUT_REJECT_CODE, __LINE__, ucRet);
				break;

			default:	// Error
				p_pstOPTM->pf_log_event (p_pstOPTM, OPTM_LOG_ADMIN_LOGOUT_ERROR_CODE, __LINE__, ucRet);
				break;
			}

			// Prepare admin login response frame
			p_pstOPTM->stSendFrame.ucClass = OPTM_FRAME_CLS_ADMIN_CODE;
			p_pstOPTM->stSendFrame.ucCmd_Status = (0x00 == ucRet) ? OPTM_FRAME_STA_ACK_CODE : OPTM_FRAME_STA_NAK_CODE;

			// Handshake after admin logout
			p_pstOPTM->ucHandshake = 0x00;
			p_pstOPTM->ucProcState = OPTM_PROC_STATE_HANDSHAKE_CODE;
			break;

		case OPTM_FRAME_ADMIN_CMD_CHG_PWD_CODE:
			ucRet = 0x01;

			optm_get_param (p_pstOPTM, 0x01, &p_pstOPTM->stRecvFrame.stParam, &pstParam1);

			if (0 != pstParam1)
			{
				if (0 != pstParam1->pucData)
				{
					// Change admin password
					ucRet = p_pstOPTM->pf_admin_chg_pwd (p_pstOPTM, pstParam1->pucData, pstParam1->iDataLen);
				}
			}

			switch (ucRet)
			{
			case 0x00:	// Success
				p_pstOPTM->pf_log_event (p_pstOPTM, OPTM_LOG_ADMIN_CHG_PWD_SUCCESS_CODE, __LINE__, ucRet);
				break;

			case 0x01:	// Rejected
				p_pstOPTM->pf_log_event (p_pstOPTM, OPTM_LOG_ADMIN_CHG_PWD_REJECT_CODE, __LINE__, ucRet);
				break;

			default:	// Error
				p_pstOPTM->pf_log_event (p_pstOPTM, OPTM_LOG_ADMIN_CHG_PWD_ERROR_CODE, __LINE__, ucRet);
				break;
			}

			// Prepare admin change password response frame
			p_pstOPTM->stSendFrame.ucClass = OPTM_FRAME_CLS_ADMIN_CODE;
			p_pstOPTM->stSendFrame.ucCmd_Status = (0x00 == ucRet) ? OPTM_FRAME_STA_ACK_CODE : OPTM_FRAME_STA_NAK_CODE;

			p_pstOPTM->ucProcState = OPTM_PROC_STATE_IDLE_CODE;
			break;

		default:
			// Prepare admin change password response frame
			p_pstOPTM->stSendFrame.ucClass = OPTM_FRAME_CLS_ADMIN_CODE;
			p_pstOPTM->stSendFrame.ucCmd_Status = OPTM_FRAME_STA_NAK_CODE;

			p_pstOPTM->ucProcState = OPTM_PROC_STATE_IDLE_CODE;
		}
	}
	else
	{
		p_pstOPTM->ucProcState = OPTM_PROC_STATE_IDLE_CODE;
		p_pstOPTM->ucLoopState = 0x01;
	}

	return 0x00;
}

unsigned char optm_proc_auth (OPTM *p_pstOPTM)
{
	unsigned char		ucRet;

	OPTM_PARAM_ITEM		*pstParam1		= 0,
						*pstParam2		= 0;

	if (OPTM_FRAME_CLS_AUTH_CODE == p_pstOPTM->stRecvFrame.ucClass)
	{
		switch (p_pstOPTM->stRecvFrame.ucCmd_Status)
		{
		case OPTM_FRAME_AUTH_CMD_AUTH_PWD_CODE:
			ucRet = 0xFF;

			optm_get_param (p_pstOPTM, 0x01, &p_pstOPTM->stRecvFrame.stParam, &pstParam1);

			if (0 != pstParam1)
			{
				if (0 != pstParam1->pucData)
				{
					// Check auth password
					ucRet = p_pstOPTM->pf_auth_pwd (p_pstOPTM, pstParam1->pucData, pstParam1->iDataLen);
				}
			}

			switch (ucRet)
			{
			case 0x00:	// Success
				p_pstOPTM->pf_log_event (p_pstOPTM, OPTM_LOG_AUTH_SUCCESS_CODE, __LINE__, ucRet);
				break;

			case 0x01:	// Rejected
				p_pstOPTM->pf_log_event (p_pstOPTM, OPTM_LOG_AUTH_REJECT_CODE, __LINE__, ucRet);
				break;

			default:	// Error
				p_pstOPTM->pf_log_event (p_pstOPTM, OPTM_LOG_AUTH_ERROR_CODE, __LINE__, ucRet);
				break;
			}

			// Prepare AUTH_EVENT response frame
			p_pstOPTM->stSendFrame.ucClass = OPTM_FRAME_CLS_AUTH_CODE;
			p_pstOPTM->stSendFrame.ucCmd_Status = (0x00 == ucRet) ? OPTM_FRAME_STA_ACK_CODE : OPTM_FRAME_STA_NAK_CODE;

			p_pstOPTM->ucProcState = OPTM_PROC_STATE_IDLE_CODE;
			break;

		case OPTM_FRAME_AUTH_CMD_ALT_AUTH_PWD_CODE:
			ucRet = 0x01;

			if (0x01 == p_pstOPTM->ucAdmin)
			{
				optm_get_param (p_pstOPTM, 0x01, &p_pstOPTM->stRecvFrame.stParam, &pstParam1);
				optm_get_param (p_pstOPTM, 0x02, &p_pstOPTM->stRecvFrame.stParam, &pstParam2);

				if (0 != pstParam1)
				{
					if (0 != pstParam1->pucData)
					{
						if (0 == pstParam2)
						{
							// Alter auth password (new format Param1=NEW PIN)
							ucRet = p_pstOPTM->pf_alt_auth_pwd (p_pstOPTM, pstParam1->pucData, pstParam1->iDataLen);
						}
						else
						{
							if (0 != pstParam2->pucData)
							{
								// Alter auth password (old format Param1=OLD PIN, Param2=NEW PIN)
								ucRet = p_pstOPTM->pf_alt_auth_pwd (p_pstOPTM, pstParam2->pucData, pstParam2->iDataLen);
							}
						}
					}
				}
			}

			switch (ucRet)
			{
			case 0x00:	// Success
				p_pstOPTM->pf_log_event (p_pstOPTM, OPTM_LOG_ALT_AUTH_SUCCESS_CODE, __LINE__, ucRet);
				break;

			case 0x01:	// Rejected
				p_pstOPTM->pf_log_event (p_pstOPTM, OPTM_LOG_ALT_AUTH_REJECT_CODE, __LINE__, ucRet);
				break;

			default:	// Error
				p_pstOPTM->pf_log_event (p_pstOPTM, OPTM_LOG_ALT_AUTH_ERROR_CODE, __LINE__, ucRet);
				break;
			}

			// Prepare AUTH_EVENT response frame
			p_pstOPTM->stSendFrame.ucClass = OPTM_FRAME_CLS_AUTH_CODE;
			p_pstOPTM->stSendFrame.ucCmd_Status = (0x00 == ucRet) ? OPTM_FRAME_STA_ACK_CODE : OPTM_FRAME_STA_NAK_CODE;

			p_pstOPTM->ucProcState = OPTM_PROC_STATE_IDLE_CODE;
			break;

		default:
			// Prepare AUTH_EVENT response frame
			p_pstOPTM->stSendFrame.ucClass = OPTM_FRAME_CLS_AUTH_CODE;
			p_pstOPTM->stSendFrame.ucCmd_Status = OPTM_FRAME_STA_NAK_CODE;

			p_pstOPTM->ucProcState = OPTM_PROC_STATE_IDLE_CODE;
		}
	}
	else
	{
		p_pstOPTM->ucProcState = OPTM_PROC_STATE_IDLE_CODE;
		p_pstOPTM->ucLoopState = 0x01;
	}

	return 0x00;
}

unsigned char optm_proc_speed_dial (OPTM *p_pstOPTM)
{
	unsigned char		ucRet;

	OPTM_PARAM_ITEM		*pstParam1		= 0,
						*pstParam2		= 0;

	if (OPTM_FRAME_CLS_SPEED_DIAL_CODE == p_pstOPTM->stRecvFrame.ucClass)
	{
		switch (p_pstOPTM->stRecvFrame.ucCmd_Status)
		{
		case OPTM_FRAME_SPEED_DIAL_CMD_ADD_CODE:
			ucRet = 0x01;

			if (0x01 == p_pstOPTM->ucAdmin)
			{
				optm_get_param (p_pstOPTM, 0x01, &p_pstOPTM->stRecvFrame.stParam, &pstParam1);
				optm_get_param (p_pstOPTM, 0x02, &p_pstOPTM->stRecvFrame.stParam, &pstParam2);

				if (0 != pstParam1 && 0 != pstParam2)
				{
					if (0 != pstParam1->pucData && 0 != pstParam2->pucData)
					{
						// Add/edit speed dial
						ucRet = p_pstOPTM->pf_speed_dial_add (p_pstOPTM, *pstParam1->pucData, pstParam2->pucData, pstParam2->iDataLen);
					}
				}
			}

			switch (ucRet)
			{
			case 0x00:	// Success
				p_pstOPTM->pf_log_event (p_pstOPTM, OPTM_LOG_SPEED_DIAL_ADD_SUCCESS_CODE, __LINE__, ucRet);
				break;

			case 0x01:	// Rejected
				p_pstOPTM->pf_log_event (p_pstOPTM, OPTM_LOG_SPEED_DIAL_ADD_REJECT_CODE, __LINE__, ucRet);
				break;

			default:	// Error
				p_pstOPTM->pf_log_event (p_pstOPTM, OPTM_LOG_SPEED_DIAL_ADD_ERROR_CODE, __LINE__, ucRet);
				break;
			}

			// Prepare AUTH_EVENT response frame
			p_pstOPTM->stSendFrame.ucClass = OPTM_FRAME_CLS_SPEED_DIAL_CODE;
			p_pstOPTM->stSendFrame.ucCmd_Status = (0x00 == ucRet) ? OPTM_FRAME_STA_ACK_CODE : OPTM_FRAME_STA_NAK_CODE;

			p_pstOPTM->ucProcState = OPTM_PROC_STATE_IDLE_CODE;
			break;

		case OPTM_FRAME_SPEED_DIAL_CMD_DEL_CODE:
			ucRet = 0x01;

			if (0x01 == p_pstOPTM->ucAdmin)
			{
				optm_get_param (p_pstOPTM, 0x01, &p_pstOPTM->stRecvFrame.stParam, &pstParam1);

				if (0 != pstParam1)
				{
					if (0 != pstParam1->pucData)
					{
						// Delete speed dial
						ucRet = p_pstOPTM->pf_speed_dial_del (p_pstOPTM, *pstParam1->pucData);
					}
				}
			}

			switch (ucRet)
			{
			case 0x00:	// Success
				p_pstOPTM->pf_log_event (p_pstOPTM, OPTM_LOG_SPEED_DIAL_DEL_SUCCESS_CODE, __LINE__, ucRet);
				break;

			case 0x01:	// Rejected
				p_pstOPTM->pf_log_event (p_pstOPTM, OPTM_LOG_SPEED_DIAL_DEL_REJECT_CODE, __LINE__, ucRet);
				break;

			default:	// Error
				p_pstOPTM->pf_log_event (p_pstOPTM, OPTM_LOG_SPEED_DIAL_DEL_ERROR_CODE, __LINE__, ucRet);
				break;
			}

			// Prepare AUTH_EVENT response frame
			p_pstOPTM->stSendFrame.ucClass = OPTM_FRAME_CLS_SPEED_DIAL_CODE;
			p_pstOPTM->stSendFrame.ucCmd_Status = (0x00 == ucRet) ? OPTM_FRAME_STA_ACK_CODE : OPTM_FRAME_STA_NAK_CODE;

			p_pstOPTM->ucProcState = OPTM_PROC_STATE_IDLE_CODE;
			break;

		default:
			// Prepare AUTH_EVENT response frame
			p_pstOPTM->stSendFrame.ucClass = OPTM_FRAME_CLS_SPEED_DIAL_CODE;
			p_pstOPTM->stSendFrame.ucCmd_Status = OPTM_FRAME_STA_NAK_CODE;

			p_pstOPTM->ucProcState = OPTM_PROC_STATE_IDLE_CODE;
		}
	}
	else
	{
		p_pstOPTM->ucProcState = OPTM_PROC_STATE_IDLE_CODE;
		p_pstOPTM->ucLoopState = 0x01;
	}

	return 0x00;
}

unsigned char optm_proc_tel (OPTM *p_pstOPTM)
{
	unsigned char		ucRet			= 0x00;

	char				acCallID [128]	= { 0 };

	int					iSpeedDial;

	OPTM_PARAM_ITEM		*pstParam1		= 0,
						*pstParam2		= 0;

	if (OPTM_FRAME_CLS_EVT_TEL_CODE == p_pstOPTM->stRecvFrame.ucClass)
	{
		switch (p_pstOPTM->stRecvFrame.ucCmd_Status)
		{
		case OPTM_FRAME_TEL_CMD_MCU_DIALING_CODE:
			p_pstOPTM->pf_get_tick (&p_pstOPTM->ulLastOpTime);

			ucRet = p_pstOPTM->pf_tel_event (p_pstOPTM, OPTM_FRAME_TEL_EVT_MCU_DAILING_CODE, __LINE__, 0x00);

			// Prepare TEL_EVENT response frame
			p_pstOPTM->stSendFrame.ucClass = OPTM_FRAME_CLS_EVT_TEL_CODE;
			p_pstOPTM->stSendFrame.ucCmd_Status = (0x00 == ucRet) ? OPTM_FRAME_STA_ACK_CODE : OPTM_FRAME_STA_NAK_CODE;

			ucRet = 0x00;
			break;

		case OPTM_FRAME_TEL_CMD_MCU_DIAL_NO_CODE:
			p_pstOPTM->pf_get_tick (&p_pstOPTM->ulLastOpTime);

			optm_get_param (p_pstOPTM, 0x01, &p_pstOPTM->stRecvFrame.stParam, &pstParam1);
			optm_get_param (p_pstOPTM, 0x02, &p_pstOPTM->stRecvFrame.stParam, &pstParam2);

			if (0 != pstParam1)
			{
				if (0 != pstParam1->pucData)
				{
					strncpy (acCallID, (char *) pstParam1->pucData, pstParam1->iDataLen);
				}
			}

			iSpeedDial = 0;

			if (0 != pstParam2)
			{
				if (0 != pstParam2->pucData)
				{
					iSpeedDial = 1;	// Speed dial indicator
				}
			}

			ucRet = p_pstOPTM->pf_tel_event (p_pstOPTM, OPTM_FRAME_TEL_EVT_MCU_DIAL_NO_CODE, __LINE__, 0, acCallID, iSpeedDial);

			// Prepare TEL_EVENT response frame
			p_pstOPTM->stSendFrame.ucClass = OPTM_FRAME_CLS_EVT_TEL_CODE;
			p_pstOPTM->stSendFrame.ucCmd_Status = (0x00 == ucRet) ? OPTM_FRAME_STA_ACK_CODE : OPTM_FRAME_STA_NAK_CODE;

			ucRet = 0x00;
			break;

		case OPTM_FRAME_TEL_CMD_MCU_HANG_UP_CODE:
			p_pstOPTM->pf_get_tick (&p_pstOPTM->ulLastOpTime);

			ucRet = p_pstOPTM->pf_tel_event (p_pstOPTM, OPTM_FRAME_TEL_EVT_MCU_HANG_UP_CODE, __LINE__, 0);

			// Prepare TEL_EVENT response frame
			p_pstOPTM->stSendFrame.ucClass = OPTM_FRAME_CLS_EVT_TEL_CODE;
			p_pstOPTM->stSendFrame.ucCmd_Status = (0x00 == ucRet) ? OPTM_FRAME_STA_ACK_CODE : OPTM_FRAME_STA_NAK_CODE;

			if (0x00 == ucRet)
			{
				p_pstOPTM->ucProcState = OPTM_PROC_STATE_CLOSE_CODE;
			}

			break;

		case OPTM_FRAME_TEL_CMD_MCU_CONNECT_CODE:
			p_pstOPTM->pf_get_tick (&p_pstOPTM->ulLastOpTime);

			ucRet = p_pstOPTM->pf_tel_event (p_pstOPTM, OPTM_FRAME_TEL_EVT_MCU_CONNECT_CODE, __LINE__, 0);

			// Prepare TEL_EVENT response frame
			p_pstOPTM->stSendFrame.ucClass = OPTM_FRAME_CLS_EVT_TEL_CODE;
			p_pstOPTM->stSendFrame.ucCmd_Status = (0x00 == ucRet) ? OPTM_FRAME_STA_ACK_CODE : OPTM_FRAME_STA_NAK_CODE;
			break;

		case OPTM_FRAME_STA_ACK_CODE:
		case OPTM_FRAME_STA_NAK_CODE:
			if (0x01 == p_pstOPTM->stSendEvent.ucPending)
			{
				switch (p_pstOPTM->stSendEvent.stFrame.ucCmd_Status)
				{
				case OPTM_FRAME_TEL_CMD_GSM_RINGING_CODE:
					p_pstOPTM->pf_get_tick (&p_pstOPTM->ulLastOpTime);
					
					if (OPTM_FRAME_STA_ACK_CODE == p_pstOPTM->stRecvFrame.ucCmd_Status)
					{
						ucRet = p_pstOPTM->pf_tel_event (p_pstOPTM, OPTM_FRAME_TEL_EVT_MCU_RINGING_CODE, __LINE__, 0x00);
					}
					else
					{
						ucRet = p_pstOPTM->pf_tel_event (p_pstOPTM, OPTM_FRAME_TEL_EVT_MCU_RINGING_CODE, __LINE__, 0x01);
					}

					break;

				case OPTM_FRAME_TEL_CMD_GSM_CONNECT_CODE:
					// GSM incoming call rejected by MCU
					if (OPTM_FRAME_STA_NAK_CODE == p_pstOPTM->stRecvFrame.ucCmd_Status)
					{
						ucRet = p_pstOPTM->pf_tel_event (p_pstOPTM, OPTM_FRAME_TEL_EVT_MCU_HANG_UP_CODE, __LINE__, 0x00);
					}
				}

				// Clear the event after handled
				optm_del_frame (p_pstOPTM, &p_pstOPTM->stSendEvent.stFrame);
				p_pstOPTM->pf_memset (&p_pstOPTM->stSendEvent, 0x00, sizeof (p_pstOPTM->stSendEvent));
			}
			else
			{
				p_pstOPTM->pf_log_event (p_pstOPTM, OPTM_LOG_FRAME_UNPROCESS_CODE, __LINE__, 0x00);
				ucRet = 0x01;
			}

			break;
		}
	}
	else
	{
		p_pstOPTM->ucProcState = OPTM_PROC_STATE_IDLE_CODE;
		p_pstOPTM->ucLoopState = 0x01;
	}

	return ucRet;
}

unsigned char optm_proc_pos (OPTM *p_pstOPTM)
{
	unsigned char		ucRet			= 0x00;

	int					iSlot			= 0;

	char				acCallID [128]	= { 0 };

	OPTM_PARAM_ITEM		*pstParam1		= 0,
						*pstParam2		= 0;

	if (OPTM_FRAME_CLS_EVT_POS_CODE == p_pstOPTM->stRecvFrame.ucClass)
	{
		switch (p_pstOPTM->stRecvFrame.ucCmd_Status)
		{
		case OPTM_FRAME_POS_CMD_MCU_DIALING_CODE:
			p_pstOPTM->pf_get_tick (&p_pstOPTM->ulLastOpTime);

			optm_get_param (p_pstOPTM, 0x01, &p_pstOPTM->stRecvFrame.stParam, &pstParam1);

			iSlot = 0;

			if (0 != pstParam1)
			{
				if (0 != pstParam1->pucData)
				{
					iSlot = *pstParam1->pucData;

					switch (iSlot)
					{
					case 2:
					case 3:
					case 4:
						iSlot -= 1;
					}
				}
			}

			ucRet = p_pstOPTM->pf_pos_event (p_pstOPTM, OPTM_FRAME_POS_EVT_MCU_DAILING_CODE, __LINE__, 0x00, iSlot);

			// Prepare POS_EVENT response frame
			p_pstOPTM->stSendFrame.ucClass = OPTM_FRAME_CLS_EVT_POS_CODE;
			p_pstOPTM->stSendFrame.ucCmd_Status = (0x00 == ucRet) ? OPTM_FRAME_STA_ACK_CODE : OPTM_FRAME_STA_NAK_CODE;
			break;

		case OPTM_FRAME_POS_CMD_MCU_DIAL_NO_CODE:
			p_pstOPTM->pf_get_tick (&p_pstOPTM->ulLastOpTime);

			optm_get_param (p_pstOPTM, 0x01, &p_pstOPTM->stRecvFrame.stParam, &pstParam1);
			optm_get_param (p_pstOPTM, 0x02, &p_pstOPTM->stRecvFrame.stParam, &pstParam2);

			if (0 != pstParam1)
			{
				if (0 != pstParam1->pucData)
				{
					strncpy (acCallID, (char *) pstParam1->pucData, pstParam1->iDataLen);
				}
			}

			iSlot = 0;

			if (0 != pstParam2)
			{
				if (0 != pstParam2->pucData)
				{
					iSlot = *pstParam2->pucData;

					switch (iSlot)
					{
					case 2:
					case 3:
					case 4:
						iSlot -= 1;
					}
				}
			}

			ucRet = p_pstOPTM->pf_pos_event (p_pstOPTM, OPTM_FRAME_POS_EVT_MCU_DIAL_NO_CODE, __LINE__, 0x00, acCallID, iSlot);

			// Prepare POS_EVENT response frame
			p_pstOPTM->stSendFrame.ucClass = OPTM_FRAME_CLS_EVT_POS_CODE;
			p_pstOPTM->stSendFrame.ucCmd_Status = (0x00 == ucRet) ? OPTM_FRAME_STA_ACK_CODE : OPTM_FRAME_STA_NAK_CODE;
			break;

		case OPTM_FRAME_POS_CMD_MCU_HANG_UP_CODE:
			p_pstOPTM->pf_get_tick (&p_pstOPTM->ulLastOpTime);

			optm_get_param (p_pstOPTM, 0x01, &p_pstOPTM->stRecvFrame.stParam, &pstParam1);

			iSlot = 0;

			if (0 != pstParam1)
			{
				if (0 != pstParam1->pucData)
				{
					iSlot = *pstParam1->pucData;

					switch (iSlot)
					{
					case 2:
					case 3:
					case 4:
						iSlot -= 1;
					}
				}
			}

			ucRet = p_pstOPTM->pf_pos_event (p_pstOPTM, OPTM_FRAME_POS_EVT_MCU_HANG_UP_CODE, __LINE__, 0x00, iSlot);

			// Prepare POS_EVENT response frame
			p_pstOPTM->stSendFrame.ucClass = OPTM_FRAME_CLS_EVT_POS_CODE;
			p_pstOPTM->stSendFrame.ucCmd_Status = (0x00 == ucRet) ? OPTM_FRAME_STA_ACK_CODE : OPTM_FRAME_STA_NAK_CODE;
			break;

		default:
			p_pstOPTM->pf_log_event (p_pstOPTM, OPTM_LOG_FRAME_UNPROCESS_CODE, __LINE__, 0x00);
			ucRet = 0x01;
		}
	}
	else
	{
		p_pstOPTM->ucProcState = OPTM_PROC_STATE_IDLE_CODE;
		p_pstOPTM->ucLoopState = 0x01;
	}

	return ucRet;
}

unsigned char optm_proc_close (OPTM *p_pstOPTM)
{
	unsigned char	ucRet	= 0x00;

	switch (p_pstOPTM->stRecvFrame.ucClass)
	{
	case OPTM_FRAME_CLS_EVT_TEL_CODE:
		ucRet = p_pstOPTM->pf_tel_event (p_pstOPTM, OPTM_FRAME_TEL_EVT_MCU_HANG_UP_CODE, __LINE__, 0x00);
	}

	if (0x00 == ucRet)
	{
		p_pstOPTM->ucProcState = OPTM_PROC_STATE_IDLE_CODE;
		p_pstOPTM->ucLoopState = 0x01;
	}

	return ucRet;
}

void optm_get_param (OPTM *p_pstOPTM, unsigned char p_ucCode, OPTM_PARAM *p_pstParam, OPTM_PARAM_ITEM **p_ppstItem)
{
	int		iIndex;

	*p_ppstItem = 0;

	for (iIndex = 0; OPTM_MAX_PARAM > iIndex; iIndex++)
	{
		if (0x01 == p_pstParam->astItem [iIndex].ucIsUsed && p_ucCode == p_pstParam->astItem [iIndex].ucCode)
		{
			*p_ppstItem = &p_pstParam->astItem [iIndex];
			break;
		}
	}
}

void optm_del_param (OPTM *p_pstOPTM, unsigned char p_ucCode, OPTM_PARAM *p_pstParam)
{
	int		iIndex;

	for (iIndex = 0; OPTM_MAX_PARAM > iIndex; iIndex++)
	{
		if (0x01 == p_pstParam->astItem [iIndex].ucIsUsed && (0xFF == p_ucCode || p_ucCode == p_pstParam->astItem [iIndex].ucCode))
		{
			p_pstOPTM->pf_free (p_pstParam->astItem [iIndex].pucData);
			p_pstParam->astItem [iIndex].pucData = 0;

			p_pstOPTM->pf_memset (&p_pstParam->astItem [iIndex], 0x00, sizeof (p_pstParam->astItem [iIndex]));
		}
	}
}

void optm_del_frame (OPTM *p_pstOPTM, OPTM_FRAME *p_pstFrame)
{
	optm_del_param (p_pstOPTM, 0xFF, &p_pstFrame->stParam);
	p_pstOPTM->pf_memset (p_pstFrame, 0x00, sizeof (OPTM_FRAME));
}

unsigned char optm_dup_frame (OPTM *p_pstOPTM, OPTM_FRAME *p_pstDestFrame, OPTM_FRAME *p_pstSrcFrame)
{
	unsigned char	ucRet		= 0x00;

	int				iIndex,
					iCount		= 0;

	// Clear destination frame
	optm_del_frame (p_pstOPTM, p_pstDestFrame);

	p_pstDestFrame->ucClass = p_pstSrcFrame->ucClass;
	p_pstDestFrame->ucCmd_Status = p_pstSrcFrame->ucCmd_Status;
	p_pstDestFrame->stParam.iCount = p_pstSrcFrame->stParam.iCount;

	for (iIndex = 0; iIndex < OPTM_MAX_PARAM; iIndex++)
	{
		if (0x01 == p_pstSrcFrame->stParam.astItem [iIndex].ucIsUsed)
		{
			iCount++;

			p_pstDestFrame->stParam.astItem[iIndex].iDataLen = p_pstSrcFrame->stParam.astItem[iIndex].iDataLen;
			p_pstDestFrame->stParam.astItem[iIndex].ucCode   = p_pstSrcFrame->stParam.astItem[iIndex].ucCode;
			p_pstDestFrame->stParam.astItem[iIndex].ucIsUsed = p_pstSrcFrame->stParam.astItem[iIndex].ucIsUsed;

			p_pstDestFrame->stParam.astItem[iIndex].pucData = p_pstOPTM->pf_malloc (p_pstSrcFrame->stParam.astItem[iIndex].iDataLen);

			p_pstOPTM->pf_memcpy (p_pstDestFrame->stParam.astItem[iIndex].pucData, 
									p_pstSrcFrame->stParam.astItem[iIndex].pucData, 
									p_pstSrcFrame->stParam.astItem[iIndex].iDataLen);
		}
	}

	p_pstDestFrame->stParam.iCount = iCount;

	return ucRet;
}

unsigned char optm_event (OPTM *p_pstOPTM, OPTM_FRAME *p_pstFrame)
{
	if (0x00 == p_pstOPTM->stSendEvent.ucPending)
	{
		switch (p_pstFrame->ucClass)
		{
		case OPTM_FRAME_CLS_EVT_TEL_CODE:
			p_pstOPTM->ucProcState = OPTM_PROC_STATE_TEL_CODE;
		}

		optm_dup_frame (p_pstOPTM, &p_pstOPTM->stSendEvent.stFrame, p_pstFrame);

		return 0x00;
	}

	return 0x01;
}

unsigned char optm_main (OPTM *p_pstOPTM)
{
	unsigned char	ucRet,
					ucDeadLoop		= 0x00;

	unsigned int	uiUsedLen		= 0,
					uiSendLen		= 0,
					uiLen;

	unsigned long	ulCurTime;

	if (OPTM_MAGIC_CODE != p_pstOPTM->iMagic)
	{
		return OPTM_RET_STATUS_ERROR;
	}

	if (OPTM_PROC_STATE_TERM_CODE == p_pstOPTM->ucProcState)
	{
		ucRet = OPTM_RET_STATUS_CLOSE;
		goto CleanUp;
	}

	// =====================
	// ** RECEIVE SECTION **
	// =====================

	ucRet = p_pstOPTM->pf_raw_recv ((void *) p_pstOPTM, (unsigned char *) (p_pstOPTM->aucRecvBuf + p_pstOPTM->uiRecvBufLen),
		OPTM_MAX_RECV_BUF - p_pstOPTM->uiRecvBufLen, &uiLen);

	if (0x00 != ucRet)
	{
		p_pstOPTM->pf_log_event (p_pstOPTM, OPTM_LOG_RECV_FAILED_CODE, __LINE__, ucRet);

		p_pstOPTM->uiRecvBufLen = 0;

		ucRet = OPTM_RET_STATUS_FAILED;
		goto CleanUp;
	}
	else
	{
		if (0 != uiLen)
		{
			p_pstOPTM->pf_get_tick (&(p_pstOPTM->ulLastRecv));
			p_pstOPTM->uiRecvBufLen += uiLen;
		}
		else if (0 < p_pstOPTM->uiRecvBufLen)
		{
			p_pstOPTM->pf_get_tick (&ulCurTime);

			if (ulCurTime - p_pstOPTM->ulLastRecv >= OPTM_TIME_PROTO_RECV_TIMEOUT && ulCurTime >= p_pstOPTM->ulLastRecv)
			{
				p_pstOPTM->pf_log_event (p_pstOPTM, OPTM_LOG_RECV_TIMEOUT_CODE, __LINE__, 0x00);
				p_pstOPTM->uiRecvBufLen = 0;
				p_pstOPTM->ulLastRecv = ulCurTime;
			}
			else if (ulCurTime < p_pstOPTM->ulLastRecv)
			{
				p_pstOPTM->ulLastRecv = ulCurTime;
			}
		}
	}


	// =====================
	// ** ANALYSE SECTION **
	// =====================

	p_pstOPTM->stRecvFrame.ucClass = OPTM_FRAME_CLS_NONE_CODE;

	if (0 != p_pstOPTM->uiRecvBufLen)
	{
		ucRet = optm_parse (p_pstOPTM, (int *) &uiUsedLen);

		if (0x00 != ucRet)
		{
			if (0 != uiUsedLen)
			{
				p_pstOPTM->pf_log_event (p_pstOPTM, OPTM_LOG_FRAME_CORRUPTED_CODE, __LINE__, 0x00, p_pstOPTM->aucRecvBuf, uiUsedLen);
			}
			else
			{
				p_pstOPTM->pf_log_event (p_pstOPTM, OPTM_LOG_PARSE_FAILED_CODE, __LINE__, ucRet);
			}

			p_pstOPTM->stRecvFrame.ucClass = OPTM_FRAME_CLS_CORRUPTED_CODE;

			// Reset the send cache flag
			p_pstOPTM->stSendCache.ucPending = 0x00;
		}
	}

	if (OPTM_FRAME_CLS_CORRUPTED_CODE == p_pstOPTM->stRecvFrame.ucClass)
	{
		p_pstOPTM->pf_log_event (p_pstOPTM, OPTM_LOG_FRAME_CORRUPTED_CODE, __LINE__, 0x00, p_pstOPTM->aucRecvBuf, uiUsedLen);

		// Reset all event
		optm_del_frame (p_pstOPTM, &p_pstOPTM->stSendEvent.stFrame);
		p_pstOPTM->pf_memset (&p_pstOPTM->stSendEvent, 0x00, sizeof (OPTM_CACHE));

		if (0x01 == p_pstOPTM->stSendCache.ucPending)
		{
			optm_dup_frame (p_pstOPTM, &p_pstOPTM->stSendFrame, &p_pstOPTM->stSendCache.stFrame);
			p_pstOPTM->stSendEvent.ucProcState = p_pstOPTM->stSendCache.ucProcState;
			p_pstOPTM->stSendEvent.ucPending = 0x01;
		}
		else
		{
			// Prepare FRAMING ERROR frame
			p_pstOPTM->stSendFrame.ucClass = OPTM_FRAME_CLS_CORRUPTED_CODE;
			p_pstOPTM->stSendFrame.ucCmd_Status = OPTM_FRAME_STA_NAK_CODE;
		}

		p_pstOPTM->uiRecvBufLen = 0;
	}
	else
	{
		// =====================
		// ** PROCESS SECTION **
		// =====================

		do
		{
			optm_del_frame (p_pstOPTM, &p_pstOPTM->stSendFrame);

			p_pstOPTM->ucLoopState = 0x00;

			switch (p_pstOPTM->ucProcState)
			{
			case OPTM_PROC_STATE_HANDSHAKE_CODE:
				ucRet = optm_proc_handshake (p_pstOPTM);
				break;

			case OPTM_PROC_STATE_ADMIN_CODE:
				ucRet = optm_proc_admin (p_pstOPTM);
				break;

			case OPTM_PROC_STATE_AUTH_CODE:
				ucRet = optm_proc_auth (p_pstOPTM);
				break;

			case OPTM_PROC_STATE_SPEED_DIAL_CODE:
				ucRet = optm_proc_speed_dial (p_pstOPTM);
				break;

			case OPTM_PROC_STATE_TEL_CODE:
				ucRet = optm_proc_tel (p_pstOPTM);
				break;

			case OPTM_PROC_STATE_POS_CODE:
				ucRet = optm_proc_pos (p_pstOPTM);
				break;

			case OPTM_PROC_STATE_CLOSE_CODE:
				ucRet = optm_proc_close (p_pstOPTM);
				break;

			case OPTM_PROC_STATE_NONE_CODE:
				p_pstOPTM->ucProcState = OPTM_PROC_STATE_HANDSHAKE_CODE;
				p_pstOPTM->ucLoopState = 0x01;
				ucRet = 0x00;
				break;

			case OPTM_PROC_STATE_IDLE_CODE:
			default:
				ucRet = optm_proc_idle (p_pstOPTM);
				break;
			}

			if (0x00 != ucRet)
			{
				p_pstOPTM->pf_log_event (p_pstOPTM, OPTM_LOG_PROC_FAILED_CODE, __LINE__, ucRet);
				
				p_pstOPTM->ucProcState = OPTM_PROC_STATE_TERM_CODE;
				
				ucRet = OPTM_RET_STATUS_ERROR;
				goto CleanUp;
			}

			// Process any passed-in message
			if (OPTM_FRAME_CLS_NONE_CODE == p_pstOPTM->stSendFrame.ucClass && OPTM_FRAME_CLS_NONE_CODE != p_pstOPTM->stSendEvent.stFrame.ucClass &&
				0x00 == p_pstOPTM->stSendEvent.ucPending)
			{
				optm_dup_frame (p_pstOPTM, &p_pstOPTM->stSendFrame, &p_pstOPTM->stSendEvent.stFrame);
				p_pstOPTM->ucProcState = p_pstOPTM->stSendEvent.ucProcState;
				IGET_TICKCOUNT (p_pstOPTM->stSendEvent.ulLastOpTime);
				p_pstOPTM->stSendEvent.ucPending = 0x01;
				break;
			}
			else if (0x01 == p_pstOPTM->stSendEvent.ucPending)
			{
				IGET_TICKCOUNT (ulCurTime);

				if (ulCurTime - p_pstOPTM->stSendEvent.ulLastOpTime >= 5000 && ulCurTime >= p_pstOPTM->stSendEvent.ulLastOpTime && 0 != p_pstOPTM->stSendEvent.ulLastOpTime)
				{
					// CLear pending event after timeout
					optm_del_frame (p_pstOPTM, &p_pstOPTM->stSendEvent.stFrame);
					p_pstOPTM->pf_memset (&p_pstOPTM->stSendEvent, 0x00, sizeof (OPTM_CACHE));
				}
				else if (ulCurTime < p_pstOPTM->stSendEvent.ulLastOpTime || 0 == p_pstOPTM->stSendEvent.ulLastOpTime)
				{
					p_pstOPTM->stSendEvent.ulLastOpTime = ulCurTime;
				}
			}
		}
		while (0x01 == p_pstOPTM->ucLoopState && 0xFF > ++ucDeadLoop);
	}

	if (0xFF == ucDeadLoop)
	{
		p_pstOPTM->pf_log_event (p_pstOPTM, OPTM_LOG_PROC_DEAD_LOOP_CODE, __LINE__, ucRet);
	}

	// ===================
	// ** BUILD SECTION **
	// ===================

	if (OPTM_FRAME_CLS_NONE_CODE != p_pstOPTM->stSendFrame.ucClass)
	{
		ucRet = optm_build (p_pstOPTM, p_pstOPTM->aucSendBuf, OPTM_MAX_SEND_BUF, &uiSendLen);

		if (0x00 != ucRet)
		{
			p_pstOPTM->pf_log_event (p_pstOPTM, OPTM_LOG_BUILD_FAILED_CODE, __LINE__, ucRet);

			ucRet = OPTM_RET_STATUS_FAILED;
			goto CleanUp;
		}


		// ==================
		// ** SEND SECTION **
		// ==================

		if (0 != uiSendLen)
		{
			ucRet = p_pstOPTM->pf_raw_send ((void *) p_pstOPTM, p_pstOPTM->aucSendBuf, OPTM_MAX_SEND_BUF, &uiSendLen);

			if (0x00 != ucRet)
			{
				p_pstOPTM->pf_log_event (p_pstOPTM, OPTM_LOG_SEND_FAILED_CODE, __LINE__, ucRet);

				ucRet = OPTM_RET_STATUS_FAILED;
				goto CleanUp;
			}
			else
			{
				optm_dup_frame (p_pstOPTM, &p_pstOPTM->stSendCache.stFrame, &p_pstOPTM->stSendFrame);
				p_pstOPTM->stSendCache.ucProcState = p_pstOPTM->ucProcState;
				p_pstOPTM->stSendCache.ucPending = 0x01;
			}
		}
	}

	ucRet = OPTM_RET_STATUS_READY;

CleanUp:
	// Adjust buffer after analyzed
	if (0 != uiUsedLen)
	{
		if (uiUsedLen < p_pstOPTM->uiRecvBufLen)
		{
			p_pstOPTM->uiRecvBufLen -= uiUsedLen;
			p_pstOPTM->pf_memmove (p_pstOPTM->aucRecvBuf, p_pstOPTM->aucRecvBuf + uiUsedLen, p_pstOPTM->uiRecvBufLen);
		}
		else
		{
			p_pstOPTM->uiRecvBufLen = 0;
		}
	}

	optm_del_frame (p_pstOPTM, &p_pstOPTM->stSendFrame);

	return ucRet;
}
