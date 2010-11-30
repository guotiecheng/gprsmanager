#include "hal_cx930xx.h"

unsigned char hal_cx930xx_init (HAL_CX930XX *p_pstHAL, int p_iSize)
{
	unsigned char	ucRet;

	memset (p_pstHAL, 0x00, sizeof (HAL_CX930XX));

	ucRet = array_init (&p_pstHAL->stSlots, p_iSize, ARRAY_ORDER_DIRECT, 0x00, 0);

	if (0x00 != ucRet)
	{
		return 0x01;
	}

	thread_init (&p_pstHAL->stThread, (void *) p_pstHAL);

	p_pstHAL->ucInit = 0x01;

	return 0x00;
}

void hal_cx930xx_term (HAL_CX930XX *p_pstHAL)
{
	unsigned char		ucRet;

	ARRAY_DATA			*pstItem;

	HAL_CX930XX_SLOT	*pstSlot;

	if (0x01 == p_pstHAL->ucInit)
	{
		hal_cx930xx_log (p_pstHAL, HAL_CX930XX_LOG_TERMINATING_CODE, 0x00, __LINE__);

		// Kill worker process
		hal_cx930xx_close (p_pstHAL);

		// Loop thru all slots
		ucRet = array_get_first (&p_pstHAL->stSlots, &pstItem);

		while (0x00 == ucRet && 0 != pstItem)
		{
			pstSlot = (HAL_CX930XX_SLOT *) pstItem->pvData;

			// Close all usable ports
			for (ucRet = 0x00; HAL_CX930XX_MAX_DATA_PORT > ucRet; ucRet++)
			{
				hal_cx930xx_port (p_pstHAL, pstSlot, ucRet, 0x00, HAL_CX930XX_DATA_PORT_MODE_CLOSE);
			}

			comport_close_voice (&pstSlot->stCtrlPort);
			critical_term (&pstSlot->stLock);

			ucRet = array_get_next (&p_pstHAL->stSlots, pstItem, &pstItem);
		}

		memset (p_pstHAL, 0x00, sizeof (HAL_CX930XX));

		hal_cx930xx_log (p_pstHAL, HAL_CX930XX_LOG_TERMINATED_CODE, 0x00, __LINE__);
	}
}

unsigned char hal_cx930xx_add_slot (HAL_CX930XX *p_pstHAL, char *p_pcSlot, char *p_pcCtrlDrv, char *p_pcDataDrv, char *p_pcCountryReg)
{
	unsigned char		ucRet;

	HAL_CX930XX_SLOT	*pstSlot	= 0;

	if (0x01 != p_pstHAL->ucInit)
	{
		return 0x01;
	}

	MEM_ALLOC (pstSlot, sizeof (HAL_CX930XX_SLOT), (HAL_CX930XX_SLOT *));

	pstSlot->ucStatus = SYS_STATUS_IDLE;

	comport_init_voice (&pstSlot->stCtrlPort, p_pcCtrlDrv, 0, 0, 0, 0, 0, 0, 0, 0, 0);

	if (0x00 != comport_open_voice (&pstSlot->stCtrlPort))
	{
		ucRet = 0x02;
		goto CleanUp;
	}

	if (0 != p_pcCountryReg)
	{
		strncpy (pstSlot->acCountryReg, p_pcCountryReg, sizeof (pstSlot->acCountryReg) - 1);
	}

	for (ucRet = 0x00; HAL_CX930XX_MAX_DATA_PORT > ucRet; ucRet++)
	{
		pstSlot->astDataPort [ucRet].ucIndex = ucRet;
	}

	comport_init_voice (&pstSlot->astDataPort [0].stPort, p_pcDataDrv, HAL_CX930XX_DEF_BAUDRATE, HAL_CX930XX_DEF_DATA_BIT, HAL_CX930XX_DEF_STOP_BIT,
		HAL_CX930XX_DEF_PARITY, HAL_CX930XX_DEF_FLOW_CTRL, HAL_CX930XX_DEF_FRAG_SIZE, hal_cx930xx_send_delay, 0, pstSlot);

	critical_init (&pstSlot->stLock);

	if (0x00 != array_add (&p_pstHAL->stSlots, pstSlot, p_pcSlot, 0x01, 0))
	{
		ucRet = 0x03;
		goto CleanUp;
	}

	pstSlot = 0;

	ucRet = 0x00;

CleanUp:
	if (0 != pstSlot)
	{
		comport_term_voice (&pstSlot->stCtrlPort);
		critical_term (&pstSlot->stLock);

		MEM_CLEAN (pstSlot);
	}

	return ucRet;
}

unsigned char hal_cx930xx_start (HAL_CX930XX *p_pstHAL)
{
	if (0x01 == p_pstHAL->ucInit)
	{
		return thread_start (&p_pstHAL->stThread, hal_cx930xx_body);
	}

	return 0x00;
}

unsigned char hal_cx930xx_close (HAL_CX930XX *p_pstHAL)
{
	if (0x01 == p_pstHAL->ucInit)
	{
		return thread_close (&p_pstHAL->stThread, SIGINT, 15000);
	}

	return 0x00;
}

unsigned char hal_cx930xx_attach (HAL_CX930XX *p_pstHAL, char *p_pcSlot, unsigned char p_ucPort, unsigned char p_ucPriority, unsigned char p_ucInterrupt,
								 HAL_CX930XX_NOTIFY p_pf_notify, void *p_pvPtr, int *p_piSID)
{
	unsigned char			ucRet,
							ucIndex;

	ARRAY_DATA				*pstItem;

	HAL_CX930XX_SLOT		*pstSlot;

	HAL_CX930XX_DATA_PORT	*pstDataPort	= 0;

	if (HAL_CX930XX_MAX_DATA_PORT < p_ucPort || 0 >= p_ucPort)
	{
		return 0x01;
	}

	ucRet = array_get_by_id (&p_pstHAL->stSlots, p_pcSlot, &pstItem);

	if (0x00 != ucRet || 0 == pstItem)
	{
		return 0x01;
	}

	pstSlot = (HAL_CX930XX_SLOT *) pstItem->pvData;

	pstDataPort = &pstSlot->astDataPort [p_ucPort - 1];

	if (0x00 != critical_lock (&pstSlot->stLock, 0x00))
	{
		return 0x01;
	}

	if (0 != pstDataPort->stSession.iSID)
	{
		// Check the holder priority with caller priority
		if (p_ucPriority > pstDataPort->stSession.ucPriority || (0x01 == p_ucInterrupt && p_ucPriority == pstDataPort->stSession.ucPriority))
		{
			// Request holder to detach the data port
			pstDataPort->stSession.pf_notify (pstDataPort->stSession.pvPtr, HAL_CX930XX_NOTIFY_REQ_DETACH, 0x00);

			ucRet = 0xA0;	// Attach is pending
			goto CleanUp;
		}

		ucRet = 0xF0;	// Attach is rejected
		goto CleanUp;
	}

	if (0x01 != pstSlot->ucReady)
	{
		ucRet = 0xA1;	// Attach is not ready
		goto CleanUp;
	}

	ucIndex = pstDataPort->ucIndex + 0x01;

	ucRet = hal_cx930xx_port (p_pstHAL, pstSlot, ucIndex, 0x00, HAL_CX930XX_DATA_PORT_MODE_OPEN);

	if (0x00 != ucRet)
	{
		ucRet = 0x02;
		goto CleanUp;
	}

	// ========================================
	// ** Caller going to hold the data port **
	// ========================================

	do
	{
		pstDataPort->stSession.iSID = ++(p_pstHAL->iSeqNo);
	}
	while (0 == pstDataPort->stSession.iSID);	// To prevent seq no recycle back to zero

	pstDataPort->stSession.ucPriority = p_ucPriority;

	pstDataPort->stSession.pf_notify = p_pf_notify;
	pstDataPort->stSession.pvPtr = p_pvPtr;

	*p_piSID = pstDataPort->stSession.iSID;
	ucRet = 0x00;

CleanUp:
	critical_unlock (&pstSlot->stLock);

	return ucRet;
}

unsigned char hal_cx930xx_detach (HAL_CX930XX *p_pstHAL, char *p_pcSlot, int p_iSID)
{
	unsigned char			ucRet			= 0x01,
							ucIndex;

	ARRAY_DATA				*pstItem;

	HAL_CX930XX_SLOT		*pstSlot		= 0;

	HAL_CX930XX_DATA_PORT	*pstDataPort	= 0;

	if (0 != p_pcSlot)
	{
		ucRet = array_get_by_id (&p_pstHAL->stSlots, p_pcSlot, &pstItem);

		if (0x00 == ucRet && 0 != pstItem)
		{
			pstSlot = (HAL_CX930XX_SLOT *) pstItem->pvData;
		}
	}

	if (0 == pstSlot && 0 < p_iSID)
	{
		ucRet = hal_cx930xx_get_slot_by_sid (p_pstHAL, p_iSID, &pstSlot);
	}

	if (0x00 != ucRet || 0 == pstSlot)
	{
		return 0x00;
	}

	if (0x00 != critical_lock (&pstSlot->stLock, 0x00))
	{
		return 0x01;
	}

	if (0 < p_iSID)
	{
		for (ucRet = 0x00; HAL_CX930XX_MAX_DATA_PORT > ucRet; ucRet++)
		{
			if (p_iSID == pstSlot->astDataPort [ucRet].stSession.iSID)
			{
				pstDataPort = &pstSlot->astDataPort [ucRet];
				break;
			}
		}

		// Caller is not the owner
		if (0 == pstDataPort)
		{
			ucRet = 0x02;
			goto CleanUp;
		}

		// Close port
		ucIndex = pstDataPort->ucIndex + 0x01;
		hal_cx930xx_port (p_pstHAL, pstSlot, ucIndex, 0x00, HAL_CX930XX_DATA_PORT_MODE_CLOSE);

		// Caller going to detach data port
		pstDataPort->stSession.iSID = 0;
		pstDataPort->stSession.ucPriority = 0;

		pstDataPort->stSession.pf_notify = 0;
		pstDataPort->stSession.pvPtr = 0;
	}

	ucRet = 0x00;

CleanUp:
	if (0 != pstSlot)
	{
		critical_unlock (&pstSlot->stLock);
	}

	return ucRet;
}

unsigned char hal_cx930xx_request (HAL_CX930XX *p_pstHAL, char *p_pcSlot, int p_iSID, unsigned char p_ucOpID, ...)
{
	unsigned char			ucRet				= 0x00,
							ucIndex,
							ucDetach			= 0x00;

	int						iFlag;

	char					*pcRet;

	va_list					args;

	ARRAY_DATA				*pstItem;

	HAL_CX930XX_SLOT		*pstSlot			= 0;

	HAL_CX930XX_DATA_PORT	*pstDataPort		= 0;

	va_start (args, p_ucOpID);

	if (0 != p_pcSlot)
	{
		ucRet = array_get_by_id (&p_pstHAL->stSlots, p_pcSlot, &pstItem);

		if (0x00 == ucRet && 0 != pstItem)
		{
			pstSlot = (HAL_CX930XX_SLOT *) pstItem->pvData;
		}
	}

	if (0 == pstSlot && 0 < p_iSID)
	{
		ucRet = hal_cx930xx_get_slot_by_sid (p_pstHAL, p_iSID, &pstSlot);
	}

	if (0x00 != ucRet || 0 == pstSlot)
	{
		return 0x01;
	}

	if (-1 == p_iSID)
	{
		pstDataPort = &pstSlot->astDataPort [0];
	}
	else if (0 < p_iSID)
	{
		if (0x00 != critical_lock (&pstSlot->stLock, 0x01))
		{
			return 0x02;
		}

		for (ucRet = 0x00; HAL_CX930XX_MAX_DATA_PORT > ucRet; ucRet++)
		{
			if (p_iSID == pstSlot->astDataPort [ucRet].stSession.iSID)
			{
				pstDataPort = &pstSlot->astDataPort [ucRet];
				break;
			}
		}

		critical_unlock (&pstSlot->stLock);
	}

	if (0 == pstDataPort)
	{
		ucRet = 0x03;
		goto CleanUp;
	}

	switch (p_ucOpID)
	{
	case HAL_CX930XX_REQ_RESET:
		// Check all the holder priority with caller priority
		if (-1 != p_iSID)
		{
			for (ucRet = 0x00; HAL_CX930XX_MAX_DATA_PORT > ucRet; ucRet++)
			{
				if (pstDataPort->stSession.iSID != pstSlot->astDataPort [ucRet].stSession.iSID && 0 != pstSlot->astDataPort [ucRet].stSession.iSID &&
					pstDataPort->stSession.ucPriority < pstSlot->astDataPort [ucRet].stSession.ucPriority)
				{
					ucRet = 0x04;
					goto CleanUp;
				}
			}
		}

		hal_cx930xx_log (p_pstHAL, HAL_CX930XX_LOG_RESETTING_CODE, 0x00, __LINE__);

		pstSlot->ucReset = 0x01;
		pstSlot->ucReady = 0x00;
		ucDetach = 0x01;
		break;

	case HAL_CX930XX_REQ_DETACH:
		// Check all the holder priority with caller priority
		if (-1 != p_iSID)
		{
			for (ucRet = 0x00; HAL_CX930XX_MAX_DATA_PORT > ucRet; ucRet++)
			{
				if (pstDataPort->stSession.iSID != pstSlot->astDataPort [ucRet].stSession.iSID && 0 != pstSlot->astDataPort [ucRet].stSession.iSID &&
					pstDataPort->stSession.ucPriority < pstSlot->astDataPort [ucRet].stSession.ucPriority)
				{
					ucRet = 0x04;
					goto CleanUp;
				}
			}
		}

		pstSlot->ucReady = 0x00;
		ucDetach = 0x01;
		break;

	case HAL_CX930XX_REQ_OPEN_DATA_PORT:
		ucIndex = pstDataPort->ucIndex + 0x01;
		ucRet = hal_cx930xx_port (p_pstHAL, pstSlot, ucIndex, 0x00, HAL_CX930XX_DATA_PORT_MODE_OPEN);
		break;

	case HAL_CX930XX_REQ_CLOSE_DATA_PORT:
		ucIndex = pstDataPort->ucIndex + 0x01;
		ucRet = hal_cx930xx_port (p_pstHAL, pstSlot, ucIndex, 0x00, HAL_CX930XX_DATA_PORT_MODE_CLOSE);
		break;

	case HAL_CX930XX_REQ_GET_DATA_PORT_FILE:
		pcRet = va_arg (args, char *);

		strcpy (pcRet, pstDataPort->stPort.acDevFile);

		ucRet = 0x00;
		break;

	case HAL_CX930XX_REQ_DTR:
		iFlag = va_arg (args, int);

		hal_cx930xx_dtr (p_pstHAL, pstSlot, (unsigned char) iFlag);
		break;

	default:
		ucRet = 0xF0;
		goto CleanUp;
	}

	if (0x01 == ucDetach)
	{
		// Notify all holder to detach port
		for (ucRet = 0x00; HAL_CX930XX_MAX_DATA_PORT > ucRet; ucRet++)
		{
			if (0 != pstSlot->astDataPort [ucRet].stSession.iSID)
			{
				pstSlot->astDataPort [ucRet].stSession.pf_notify (pstSlot->astDataPort [ucRet].stSession.pvPtr, HAL_CX930XX_NOTIFY_FORCED_DETACH, 0x00);
			}
		}
	}

	ucRet = 0x00;

CleanUp:
	va_end (args);

	return ucRet;
}

unsigned char hal_cx930xx_at_cmd (HAL_CX930XX *p_pstHAL, char *p_pcSlot, int p_iSID, char *p_pcReq, char *p_pcRes, char *p_pcErr, char *p_pcRet, int p_iRetSize,
								  unsigned long p_ulDelay, unsigned long p_ulGap, unsigned long p_ulTimeout, HAL_CX930XX_PROC_TOKEN p_pf_proc_token, void *p_pvPtr, unsigned int p_uiLine, char *p_pcName)
{
	unsigned char			ucRet				= 0x01;

	ARRAY_DATA				*pstItem;

	HAL_CX930XX_SLOT		*pstSlot			= 0;

	if (0 != p_pcSlot)
	{
		ucRet = array_get_by_id (&p_pstHAL->stSlots, p_pcSlot, &pstItem);

		if (0x00 == ucRet && 0 != pstItem)
		{
			pstSlot = (HAL_CX930XX_SLOT *) pstItem->pvData;
		}
	}

	if (0 == pstSlot && 0 < p_iSID)
	{
		ucRet = hal_cx930xx_get_slot_by_sid (p_pstHAL, p_iSID, &pstSlot);
	}

	if (0x00 != ucRet || 0 == pstSlot)
	{
		return 0x01;
	}

	ucRet = hal_cx930xx_at_cmd_ex (p_pstHAL, pstSlot, p_iSID, p_pcReq, p_pcRes, p_pcErr, p_pcRet, p_iRetSize,
		p_ulDelay, p_ulGap, p_ulTimeout, p_pf_proc_token, p_pvPtr, p_uiLine, p_pcName);

	return ucRet;
}

unsigned char hal_cx930xx_at_cmd_ex (HAL_CX930XX *p_pstHAL, HAL_CX930XX_SLOT *p_pstSlot, int p_iSID, char *p_pcReq, char *p_pcRes, char *p_pcErr, char *p_pcRet, int p_iRetSize,
									 unsigned long p_ulDelay, unsigned long p_ulGap, unsigned long p_ulTimeout, HAL_CX930XX_PROC_TOKEN p_pf_proc_token, void *p_pvPtr, unsigned int p_uiLine, char *p_pcName)
{
	unsigned char			ucRet,
	                        ucProcToken         = 0x01;

	unsigned long			ulCurTime,
							ulStartTime;

	int						iCmdLen,
							iRecvLen,
							iRecvSize,
							iRet,
							iLogSize;

	char					acRecv [64],
							acLog [128],
							*pcRecvPtr;

	HAL_CX930XX_DATA_PORT	*pstDataPort		= 0;

	if (-1 == p_iSID)
	{
		pstDataPort = &p_pstSlot->astDataPort [0];
	}
	else if (0 < p_iSID)
	{
		if (0x00 != critical_lock (&p_pstSlot->stLock, 0x01))
		{
			return 0x02;
		}

		for (ucRet = 0x00; HAL_CX930XX_MAX_DATA_PORT > ucRet; ucRet++)
		{
			if (p_iSID == p_pstSlot->astDataPort [ucRet].stSession.iSID)
			{
				pstDataPort = &p_pstSlot->astDataPort [ucRet];
				break;
			}
		}

		critical_unlock (&p_pstSlot->stLock);
	}

	if (0 == pstDataPort)
	{
		ucRet = 0x03;
		goto CleanUp;
	}

	memset (acLog, 0x00, sizeof (acLog));
	iLogSize = sizeof (acLog) - 1;

	// ===================
	// ** Pause a while **
	// ===================

	if (0 != p_ulDelay)
	{
		IGET_TICKCOUNT (ulCurTime);
		ulStartTime = ulCurTime;

		// Pause a while before matching response
		while (ulCurTime - ulStartTime < p_ulDelay && ulStartTime <= ulCurTime)
		{
		    if (p_pf_proc_token && !(ucProcToken = p_pf_proc_token (p_pvPtr)))
		    {
		        break;
		    }

			ISLEEP (1);
			IGET_TICKCOUNT (ulCurTime);
		}

		if (!ucProcToken)
		{
			ucRet = 0xFF;
			goto CleanUp;
		}
	}


	// ===================
	// == Throw rubbish ==
	// ===================

	IGET_TICKCOUNT (ulCurTime);
	ulStartTime = ulCurTime;

	while (ulCurTime - ulStartTime < p_ulTimeout && ulCurTime >= ulStartTime)
	{
	    if (p_pf_proc_token && !(ucProcToken = p_pf_proc_token (p_pvPtr)))
	    {
	        break;
	    }

		ucRet = comport_recv_voice (&pstDataPort->stPort, (unsigned char *) acRecv, 1, &iRet, 50);

		if (0x00 != ucRet || 0 == iRet)
		{
			break;
		}

		ISLEEP (1);
		IGET_TICKCOUNT (ulCurTime);
	}

	// =====================
	// ** Send AT command **
	// =====================

	if (0 != p_pcReq)
	{
		iCmdLen = strlen (p_pcReq);

		// Append to return string
		if (0 < iLogSize)
		{
			strncpy (acLog, p_pcReq, iLogSize);
			iLogSize -= iCmdLen;
		}

		ucRet = comport_send_voice (&pstDataPort->stPort, (unsigned char *) p_pcReq, iCmdLen);

		if (0 != ucRet)
		{
			if (0 != p_pcName)
			{
				hal_cx930xx_log (p_pstHAL, HAL_CX930XX_LOG_DATA_PORT_FAILURE_CODE, ucRet, __LINE__);
			}

			ucRet = 0x02;
			goto CleanUp;
		}
	}

	// ===================
	// ** Pause a while **
	// ===================

	if (0 != p_ulGap)
	{
		IGET_TICKCOUNT (ulCurTime);
		ulStartTime = ulCurTime;

		// Pause a while before matching response
		while (ulCurTime - ulStartTime < p_ulGap && ulStartTime <= ulCurTime)
		{
    	    if (p_pf_proc_token && !(ucProcToken = p_pf_proc_token (p_pvPtr)))
    	    {
    	        break;
    	    }

			ISLEEP (1);
			IGET_TICKCOUNT (ulCurTime);
		}

		if (!ucProcToken)
		{
			ucRet = 0xFF;
			goto CleanUp;
		}
	}

	// ===================
	// ** Recv response **
	// ===================

	memset (acRecv, 0, sizeof (acRecv));
	pcRecvPtr = acRecv;
	iRecvLen = 0;
	iRecvSize = sizeof (acRecv);

	IGET_TICKCOUNT (ulCurTime);
	ulStartTime = ulCurTime;

	while (ulCurTime - ulStartTime < p_ulTimeout && ulCurTime >= ulStartTime)
	{
	    if (p_pf_proc_token && !(ucProcToken = p_pf_proc_token (p_pvPtr)))
	    {
	        break;
	    }

		if (iRecvLen < iRecvSize - 1)
		{
			ucRet = comport_recv_voice (&pstDataPort->stPort, (unsigned char *) pcRecvPtr, 1, &iRet, 50);

			// ==================================
            // ** Match received contains "RING" **
            // ==================================
            // If AT command response contains 'RING', remove the head of 'RING\r'

            while (0x00 == at_match_voice (pcRecvPtr, "RING"))
			{
			    pcRecvPtr = strstr(pcRecvPtr, "RING") + sizeof("RING\r");
			}

			if (0x00 == ucRet && 0 < iRet)
			{
				// Append to return string
				if (0 < iLogSize)
				{
					strncat (acLog, pcRecvPtr, iLogSize);
					iLogSize -= ((iRet < iLogSize) ? iRet : iLogSize);
				}

				iRecvLen += iRet;
				pcRecvPtr += iRet;
				acRecv [iRecvSize - 1] = 0;

				// ==================================
				// ** Match received with response **
				// ==================================

				if (0 != p_pcRes)
				{
					if (0x00 == at_match_voice (acRecv, p_pcRes))
					{
						ucRet = 0x00;
						goto CleanUp;
					}
				}

				// ===============================
				// ** Match received with error **
				// ===============================

				if (0 != p_pcErr)
				{
					if (0x00 == at_match_voice (acRecv, p_pcErr))
					{
						ucRet = 0x03;
						goto CleanUp;
					}
				}
			}
		}

		ISLEEP (1);
		IGET_TICKCOUNT (ulCurTime);
	}

	if (!ucProcToken)
	{
		if (0 == p_pcRes)
		{
			ucRet = 0x00;
		}
		else if (0 != p_pcReq)
		{
			// Request same as echo length then return timeout
			ucRet = (strlen (p_pcReq) == strlen (acLog)) ? 0x04 : 0x05;
		}
		else if (0 == strlen (acLog))
		{
			ucRet = 0x04;
		}
		else
		{
			ucRet = 0x06;
		}
	}
	else
	{
		ucRet = 0xFF;
	}

CleanUp:
	acLog [sizeof (acLog) - 1] = 0;

	if (0 != p_pcRet)
	{
		strncpy (p_pcRet, acLog, p_iRetSize);
	}

	if (0 != p_pcName)
	{
		for (iRet = 0; sizeof (acLog) > iRet; iRet++)
		{
			if ('\r' == acLog [iRet] || '\n' == acLog [iRet])
			{
				acLog [iRet] = ' ';
			}
			else if (0 == acLog [iRet])
			{
				break;
			}
		}

		if (0 == strstr(p_pcReq, "AT-TRV"))
		{
		    hal_cx930xx_log (p_pstHAL, HAL_CX930XX_LOG_AT_CMD_CODE, ucRet, p_uiLine, p_pstSlot->stCtrlPort.acDevFile, acLog);
		    //hal_cx930xx_log (p_pstHAL, HAL_CX930XX_LOG_AT_CMD_CODE, ucRet, p_uiLine, p_pcName, acLog);
		}
	}

	return ucRet;
}

unsigned char hal_cx930xx_send (HAL_CX930XX *p_pstHAL, char *p_pcSlot, int p_iSID, unsigned char *p_pucData, int p_iLen)
{
	unsigned char			ucRet				= 0x01;

	ARRAY_DATA				*pstItem;

	HAL_CX930XX_SLOT		*pstSlot			= 0;

	HAL_CX930XX_DATA_PORT	*pstDataPort		= 0;

	if (0 != p_pcSlot)
	{
		ucRet = array_get_by_id (&p_pstHAL->stSlots, p_pcSlot, &pstItem);

		if (0x00 == ucRet && 0 != pstItem)
		{
			pstSlot = (HAL_CX930XX_SLOT *) pstItem->pvData;
		}
	}

	if (0 == pstSlot && 0 < p_iSID)
	{
		ucRet = hal_cx930xx_get_slot_by_sid (p_pstHAL, p_iSID, &pstSlot);
	}

	if (0x00 != ucRet || 0 == pstSlot)
	{
		return 0xF1;
	}

	if (-1 == p_iSID)
	{
		pstDataPort = &pstSlot->astDataPort [0];
	}
	else if (0 < p_iSID)
	{
		if (0x00 != critical_lock (&pstSlot->stLock, 0x01))
		{
			return 0xF2;
		}

		for (ucRet = 0x00; HAL_CX930XX_MAX_DATA_PORT > ucRet; ucRet++)
		{
			if (p_iSID == pstSlot->astDataPort [ucRet].stSession.iSID)
			{
				pstDataPort = &pstSlot->astDataPort [ucRet];
				break;
			}
		}

		critical_unlock (&pstSlot->stLock);
	}

	if (0 == pstDataPort)
	{
		return 0xF3;
	}

	return comport_send_voice (&pstDataPort->stPort, p_pucData, p_iLen);
}

unsigned char hal_cx930xx_recv (HAL_CX930XX *p_pstHAL, char *p_pcSlot, int p_iSID, unsigned char *p_pucData, int p_iSize, int *p_piLen, unsigned long p_ulTimeout)
{
	unsigned char			ucRet				= 0x01;

	ARRAY_DATA				*pstItem;

	HAL_CX930XX_SLOT		*pstSlot			= 0;

	HAL_CX930XX_DATA_PORT	*pstDataPort		= 0;

	if (0 != p_pcSlot)
	{
		ucRet = array_get_by_id (&p_pstHAL->stSlots, p_pcSlot, &pstItem);

		if (0x00 == ucRet && 0 != pstItem)
		{
			pstSlot = (HAL_CX930XX_SLOT *) pstItem->pvData;
		}
	}

	if (0 == pstSlot && 0 < p_iSID)
	{
		ucRet = hal_cx930xx_get_slot_by_sid (p_pstHAL, p_iSID, &pstSlot);
	}

	if (0x00 != ucRet || 0 == pstSlot)
	{
		return 0x01;
	}

	if (-1 == p_iSID)
	{
		pstDataPort = &pstSlot->astDataPort [0];
	}
	else if (0 < p_iSID)
	{
		if (0x00 != critical_lock (&pstSlot->stLock, 0x01))
		{
			return 0x02;
		}

		for (ucRet = 0x00; HAL_CX930XX_MAX_DATA_PORT > ucRet; ucRet++)
		{
			if (p_iSID == pstSlot->astDataPort [ucRet].stSession.iSID)
			{
				pstDataPort = &pstSlot->astDataPort [ucRet];
				break;
			}
		}

		critical_unlock (&pstSlot->stLock);
	}

	if (0 == pstDataPort)
	{
		return 0x03;
	}

	return comport_recv_voice (&pstDataPort->stPort, p_pucData, p_iSize, p_piLen, p_ulTimeout);
}

unsigned char hal_cx930xx_switch_on (HAL_CX930XX *p_pstHAL, HAL_CX930XX_SLOT *p_pstSlot)
{
	unsigned char		ucRet			= 0x01,
						ucIndex;

	unsigned long		ulCurTime,
						ulStartTime		= 0;

	// Close all ports
	for (ucIndex = 0x00; HAL_CX930XX_MAX_DATA_PORT > ucIndex; ucIndex++)
	{
		hal_cx930xx_port (p_pstHAL, p_pstSlot, ucIndex, 0x00, HAL_CX930XX_DATA_PORT_MODE_CLOSE);
	}

	if (0x01 == p_pstSlot->ucReset)
	{
		//hal_cx930xx_log (p_pstHAL, HAL_CX930XX_LOG_RESET_CODE, 0x00, __LINE__);
		//hal_cx930xx_log (p_pstHAL, HAL_CX930XX_LOG_RESET_CODE, 0x00, __LINE__, p_pstSlot->astDataPort->stPort.acDevFile);
		hal_cx930xx_log (p_pstHAL, HAL_CX930XX_LOG_RESET_CODE, 0x00, __LINE__, p_pstSlot->stCtrlPort.acDevFile);
		hal_cx930xx_reset (p_pstHAL, p_pstSlot);
	}
	else
	{
		hal_cx930xx_log (p_pstHAL, HAL_CX930XX_LOG_POWERING_ON_CODE, 0x00, __LINE__);
		hal_cx930xx_power_on (p_pstHAL, p_pstSlot);
	}

	IGET_TICKCOUNT (ulCurTime);
	ulStartTime = ulCurTime;

	while (hal_cx930xx_proc_token (p_pstHAL))
	{
		IGET_TICKCOUNT (ulCurTime);

		if (ulCurTime - ulStartTime >= 10000 && ulCurTime >= ulStartTime)
		{
			break;
		}
		else if (ulCurTime - ulStartTime >= 1000 && ulCurTime - ulStartTime < 10000 && ulCurTime >= ulStartTime)
		{
			// Open tty ports
			ucRet = hal_cx930xx_port (p_pstHAL, p_pstSlot, 0x01, 0x00, HAL_CX930XX_DATA_PORT_MODE_OPEN);

			if (0x00 != ucRet)
			{
				ISLEEP (1);
			}
			else
			{
				// Try AT command response on tty port
				ucRet = hal_cx930xx_at_cmd_ex (p_pstHAL, p_pstSlot, -1, "AT\r", "OK", "ERROR", 0, 0, 0, 0, 1000, hal_cx930xx_proc_token, (void *) p_pstHAL, __LINE__, HAL_CX930XX_NAME);

				if (0x00 == ucRet)
				{
					hal_cx930xx_log (p_pstHAL, HAL_CX930XX_LOG_POWERED_ON_CODE, 0x00, __LINE__);
					goto CleanUp;
				}
				else if (0x02 == ucRet)
				{
					//hal_cx930xx_log (p_pstHAL, HAL_CX930XX_LOG_POWER_ON_FAILURE_CODE, ucRet, __LINE__);
					//hal_cx930xx_log (p_pstHAL, HAL_CX930XX_LOG_POWER_ON_FAILURE_CODE, ucRet, __LINE__, p_pstSlot->astDataPort->stPort.acDevFile);
					hal_cx930xx_log (p_pstHAL, HAL_CX930XX_LOG_POWER_ON_FAILURE_CODE, ucRet, __LINE__, p_pstSlot->stCtrlPort.acDevFile);
					p_pstSlot->ucReset = 0x01;
					goto CleanUp;
				}
			}
		}
		else if (ulCurTime < ulStartTime)
		{
			ulStartTime = ulCurTime;
		}
	}

	//hal_cx930xx_log (p_pstHAL, HAL_CX930XX_LOG_POWER_ON_FAILURE_CODE, ucRet, __LINE__);
	//hal_cx930xx_log (p_pstHAL, HAL_CX930XX_LOG_POWER_ON_FAILURE_CODE, ucRet, __LINE__, p_pstSlot->astDataPort->stPort.acDevFile);
	hal_cx930xx_log (p_pstHAL, HAL_CX930XX_LOG_POWER_ON_FAILURE_CODE, ucRet, __LINE__, p_pstSlot->stCtrlPort.acDevFile);
	p_pstSlot->ucReset = 0x01;

CleanUp:
	// Close all ports
	for (ucIndex = 0x00; HAL_CX930XX_MAX_DATA_PORT > ucIndex; ucIndex++)
	{
		hal_cx930xx_port (p_pstHAL, p_pstSlot, ucIndex, 0x00, HAL_CX930XX_DATA_PORT_MODE_CLOSE);
	}

	return ucRet;
}

unsigned char hal_cx930xx_switch_off (HAL_CX930XX *p_pstHAL, HAL_CX930XX_SLOT *p_pstSlot)
{
	unsigned char	ucIndex;

	hal_cx930xx_log (p_pstHAL, HAL_CX930XX_LOG_POWERING_OFF_CODE, 0x00, __LINE__);

	p_pstSlot->ucReady = 0x00;

	// Close all ports
	for (ucIndex = 0x00; HAL_CX930XX_MAX_DATA_PORT > ucIndex; ucIndex++)
	{
		hal_cx930xx_port (p_pstHAL, p_pstSlot, ucIndex, 0x00, HAL_CX930XX_DATA_PORT_MODE_CLOSE);
	}

	hal_cx930xx_power_off (p_pstHAL, p_pstSlot);

	hal_cx930xx_log (p_pstHAL, HAL_CX930XX_LOG_POWERED_OFF_CODE, 0x00, __LINE__);

	return 0x00;
}

unsigned char hal_cx930xx_get_ready (HAL_CX930XX *p_pstHAL, HAL_CX930XX_SLOT *p_pstSlot)
{
	return 0x00;
}

void hal_cx930xx_power_on (HAL_CX930XX *p_pstHAL, HAL_CX930XX_SLOT *p_pstSlot)
{
	hal_cx930xx_reset (p_pstHAL, p_pstSlot);
}

void hal_cx930xx_reset (HAL_CX930XX *p_pstHAL, HAL_CX930XX_SLOT *p_pstSlot)
{
	#if defined (ARCH_L200) || defined (ARCH_L300) || defined (ARCH_L350) || defined (ARCH_N300) || defined (ARCH_L300B)

		int		iRet	= 0;

		if (0x00 == p_pstSlot->stCtrlPort.ucIsConnected)
		{
			comport_open_voice (&p_pstSlot->stCtrlPort);
		}

		if (0 < p_pstSlot->stCtrlPort.hComPort)
		{
			iRet = ioctl (p_pstSlot->stCtrlPort.hComPort, 7, 0);

			if (-1 == iRet)
			{
				comport_open_voice (&p_pstSlot->stCtrlPort);	// Re-open
			}
		}

	#endif
}

void hal_cx930xx_power_off (HAL_CX930XX *p_pstHAL, HAL_CX930XX_SLOT *p_pstSlot)
{

}

unsigned char hal_cx930xx_ring (HAL_CX930XX *p_pstHAL, HAL_CX930XX_SLOT *p_pstSlot)
{
    #if defined (ARCH_N300)/* Disable RI detected here for N300 test */
    return 0x01;
    #endif
    
	unsigned char	ucRet			= 0x01;

	int				iRet = 0;

	if (0x00 == p_pstSlot->stCtrlPort.ucIsConnected)
	{
		comport_open_voice (&p_pstSlot->stCtrlPort);
	}

	if (0 < p_pstSlot->stCtrlPort.hComPort && 0x01 == p_pstSlot->ucReady)
	{
		iRet = ioctl (p_pstSlot->stCtrlPort.hComPort, 3, 0);

		if (1 == iRet)
		{
			ucRet = 0x00;	// RING

			#ifdef USBPBX

			char *pcNum;
			int iNumMaxLen = MAX_TINY_BUFFER_SZ;
			
			pcNum = (char *)malloc(iNumMaxLen);
			memset(pcNum, 0, iNumMaxLen);
			
			ioctl (p_pstSlot->stCtrlPort.hComPort, 0x16, pcNum);
			logger_log (&g_stVoiceLog, LOG_NORMAL, "@%04d %s: Ring NUM %s", __LINE__, HAL_CX930XX_NAME, pcNum);
			memset(pcNum, 0, iNumMaxLen);

			#endif
			
		}
		else if (-1 == iRet)
		{
			comport_open_voice (&p_pstSlot->stCtrlPort);	// Re-open
		}
	}

	return ucRet;
}

void hal_cx930xx_dtr (HAL_CX930XX *p_pstHAL, HAL_CX930XX_SLOT *p_pstSlot, unsigned char p_ucFlag)
{
	if (0x00 == p_pstSlot->stCtrlPort.ucIsConnected)
	{
		comport_open_voice (&p_pstSlot->stCtrlPort);
	}

	if (0 < p_pstSlot->stCtrlPort.hComPort && 0x01 == p_pstSlot->ucReady)
	{
		#if defined (ARCH_L200)

			if (0x01 == p_ucFlag)	// HIGH
			{
				hal_cx930xx_log (p_pstHAL, HAL_CX930XX_LOG_DTR_CODE, 0x01, __LINE__);
				ioctl (p_pstSlot->stCtrlPort.hComPort, 1, 1);
			}
			else	// LOW
			{
				hal_cx930xx_log (p_pstHAL, HAL_CX930XX_LOG_DTR_CODE, 0x00, __LINE__);
				ioctl (p_pstSlot->stCtrlPort.hComPort, 1, 0);
			}

		#elif defined (ARCH_L300) || defined (ARCH_L350) || defined (ARCH_N300) || defined (ARCH_L300B)

			if (0x01 == p_ucFlag)	// HIGH
			{
				hal_cx930xx_log (p_pstHAL, HAL_CX930XX_LOG_DTR_CODE, 0x01, __LINE__);
				ioctl (p_pstSlot->stCtrlPort.hComPort, 2, 1);
			}
			else	// LOW
			{
				hal_cx930xx_log (p_pstHAL, HAL_CX930XX_LOG_DTR_CODE, 0x00, __LINE__);
				ioctl (p_pstSlot->stCtrlPort.hComPort, 2, 0);
			}

		#endif
	}
}

unsigned char hal_cx930xx_get_slot_by_sid (HAL_CX930XX *p_pstHAL, int p_iSID, HAL_CX930XX_SLOT **p_ppstSlot)
{
	unsigned char		ucRet;

	ARRAY_DATA			*pstItem;

	HAL_CX930XX_SLOT	*pstSlot;

	ucRet = array_get_first (&p_pstHAL->stSlots, &pstItem);

	while (0x00 == ucRet && 0 != pstItem)
	{
		pstSlot = (HAL_CX930XX_SLOT *) pstItem->pvData;

		for (ucRet = 0x00; HAL_CX930XX_MAX_DATA_PORT > ucRet; ucRet++)
		{
			if (p_iSID == pstSlot->astDataPort [ucRet].stSession.iSID)
			{
				*p_ppstSlot = pstSlot;
				return 0x00;
			}
		}

		ucRet = array_get_next (&p_pstHAL->stSlots, pstItem, &pstItem);
	}

	return 0x01;
}

unsigned char hal_cx930xx_port (HAL_CX930XX *p_pstHAL, HAL_CX930XX_SLOT *p_pstSlot, unsigned char p_ucPort, unsigned char p_ucSetting, unsigned char p_ucMode)
{
	unsigned char			ucRet			= 0x01;

	HAL_CX930XX_DATA_PORT	*pstPort		= 0;

	if (0x01 != p_pstHAL->ucInit || 0x00 == p_ucPort || HAL_CX930XX_MAX_DATA_PORT < p_ucPort)
	{
		ucRet = 0x01;
		goto CleanUp;
	}

	pstPort = &p_pstSlot->astDataPort [p_ucPort - 0x01];

	// Open mode
	if (HAL_CX930XX_DATA_PORT_MODE_OPEN == p_ucMode)
	{
		ucRet = comport_open_voice (&pstPort->stPort);
	}
	else
	{
		comport_close_voice (&pstPort->stPort);
		ucRet = 0x00;
	}

CleanUp:

	return ucRet;
}

void hal_cx930xx_log (HAL_CX930XX *p_pstHAL, int p_iLogID, unsigned char p_ucRet, ...)
{
	unsigned int	uiLine;

	char			*pcName,
					*pcBuf;

	va_list			args;

	va_start (args, p_ucRet);

	uiLine = va_arg (args, unsigned int);

	switch (p_iLogID)
	{
	case HAL_CX930XX_LOG_DATA_PORT_FAILURE_CODE:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Data port failure. RetCode [0x%02X]", uiLine, HAL_CX930XX_NAME, p_ucRet);
		break;

	case HAL_CX930XX_LOG_POWER_ON_FAILURE_CODE:
		pcName = strrchr(va_arg (args, char *), '/');
		pcName++;
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %-10s: Module power on failure. RetCode [0x%02X]", uiLine, pcName, p_ucRet);
		//logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Module power on failure. RetCode [0x%02X]", uiLine, HAL_CX930XX_NAME, p_ucRet);
		break;

	case HAL_CX930XX_LOG_POWERING_ON_CODE:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Module powering on", uiLine, HAL_CX930XX_NAME);
		break;

	case HAL_CX930XX_LOG_POWERED_ON_CODE:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Module powered on", uiLine, HAL_CX930XX_NAME);
		break;

	case HAL_CX930XX_LOG_POWERING_OFF_CODE:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Module switching off", uiLine, HAL_CX930XX_NAME);
		break;

	case HAL_CX930XX_LOG_POWERED_OFF_CODE:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Module switched off", uiLine, HAL_CX930XX_NAME);
		break;

	case HAL_CX930XX_LOG_RESETTING_CODE:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Module resetting", uiLine, HAL_CX930XX_NAME);
		break;

	case HAL_CX930XX_LOG_RESET_CODE:
	    pcName = strrchr(va_arg (args, char *), '/');
	    pcName++;
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %-10s: Module reset", uiLine, pcName);
		//logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Module reset", uiLine, HAL_CX930XX_NAME);
		break;

	case HAL_CX930XX_LOG_TERMINATING_CODE:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Module terminating", uiLine, HAL_CX930XX_NAME);
		break;

	case HAL_CX930XX_LOG_TERMINATED_CODE:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Module terminated", uiLine, HAL_CX930XX_NAME);
		break;

	case HAL_CX930XX_LOG_DTR_CODE:
		logger_log (&g_stVoiceLog, LOG_DETAIL, "@%04d %s: DTR is %s", uiLine, HAL_CX930XX_NAME, (0x00 == p_ucRet) ? "LOW" : "HIGH");
		break;

	case HAL_CX930XX_LOG_DATA_PORT_RAW_CODE:
		pcName = va_arg (args, char *);
		pcBuf = va_arg (args, char *);

		logger_log (&g_stVoiceLog, LOG_DETAIL, "@%04d %s: %s", uiLine, pcName, pcBuf);
		break;

	case HAL_CX930XX_LOG_AT_CMD_CODE:
		pcName = strrchr(va_arg (args, char *), '/');
		pcName++;
		pcBuf = va_arg (args, char *);

		logger_log (&g_stVoiceLog, LOG_DETAIL, "@%04d %-10s: %s", uiLine, pcName, pcBuf);
		break;

	default:
		logger_log (&g_stVoiceLog, LOG_DETAIL, "@%04d %s: Event %d happen. RetCode [0x%02X]", uiLine, HAL_CX930XX_NAME, p_iLogID, p_ucRet);
	}

	va_end (args);
}

unsigned char hal_cx930xx_proc_token (void *p_pvPtr)
{
    HAL_CX930XX *pstHAL = (HAL_CX930XX *) p_pvPtr;

    return pstHAL->stThread.ucProcToken;
}

void hal_cx930xx_body (void *p_pvThread)
{
	unsigned char		ucRet,
						ucGo,
						ucRING			= 0x00,
						ucCache;

	unsigned long		ulCurTime,
						ulLastOp		= 0,
						ulLastRING		= 0;

	ARRAY_DATA			*pstItem		= 0;

	HAL_CX930XX			*pstHAL			= ((WRK_THREAD *) p_pvThread)->pvPtr;

	HAL_CX930XX_SLOT	*pstSlot		= 0;

	while (0x01 == pstHAL->stThread.ucProcToken)
	{
		ucRet = (0 == pstItem) ?
			array_get_first (&pstHAL->stSlots, &pstItem) : array_get_next (&pstHAL->stSlots, pstItem, &pstItem);

		if (0x00 != ucRet || 0 == pstItem)
		{
			pstItem = 0;

			ISLEEP (50);
			continue;
		}

		pstSlot = (HAL_CX930XX_SLOT *) pstItem->pvData;

		if (0x01 != pstSlot->ucReady)
		{
			critical_lock (&pstSlot->stLock, 0x01);

			ucGo = 0x01;

			// Make sure all holder are detached port
			for (ucRet = 0x00; HAL_CX930XX_MAX_DATA_PORT > ucRet; ucRet++)
			{
				if (0 != pstSlot->astDataPort [ucRet].stSession.iSID)
				{
					ucGo = 0x00;
					break;
				}
			}

			critical_unlock (&pstSlot->stLock);

			if (0x01 == ucGo)
			{
				ucRet = hal_cx930xx_switch_on (pstHAL, pstSlot);

				if (0x00 == ucRet)
				{
					pstSlot->ucReady = 0x01;
					pstSlot->ucReset = 0x00;
				}
				else if (0x02 == ucRet)
				{
					pstSlot->ucStatus = SYS_STATUS_FAILURE;
				}
			}
		}

		if (0x01 == pstSlot->ucReady)
		{
			IGET_TICKCOUNT (ulCurTime);

			ucCache = ucRING;

			if (ulCurTime - ulLastOp >= 100 && ulCurTime >= ulLastOp && 0 != ulLastOp)
			{
				ulLastOp = ulCurTime;

				if (0x00 == hal_cx930xx_ring (pstHAL, pstSlot))
				{
					ulLastRING = ulCurTime;
					ucRING = 0x01;
				}
				else
				{
					if (ulCurTime - ulLastRING >= 3000 && ulCurTime >= ulLastRING && 0 != ulLastRING)
					{
						ulLastRING = ulCurTime;
						ucRING = 0x00;
					}
					else if (ulCurTime < ulLastRING || 0 == ulLastRING)
					{
						ulLastRING = ulCurTime;
					}
				}
			}
			else if (ulCurTime < ulLastOp || 0 == ulLastOp)
			{
				ulLastOp = ulCurTime;
			}

			// == Notify port holder ==
			for (ucRet = 0x00; ucCache != ucRING && HAL_CX930XX_MAX_DATA_PORT > ucRet; ucRet++)
			{
				if (0 != pstSlot->astDataPort [ucRet].stSession.iSID)
				{
					pstSlot->astDataPort [ucRet].stSession.pf_notify (pstSlot->astDataPort [ucRet].stSession.pvPtr, HAL_CX930XX_NOTIFY_RING, ucRING);
				}
			}
		}

		ISLEEP (10);
	}

	ucRet = array_get_first (&pstHAL->stSlots, &pstItem);

	while (0x00 == ucRet && 0 != pstItem)
	{
		pstSlot = (HAL_CX930XX_SLOT *) pstItem->pvData;

		// == Notify holder to detach port ==
		for (ucRet = 0x00; HAL_CX930XX_MAX_DATA_PORT > ucRet; ucRet++)
		{
			if (0 != pstSlot->astDataPort [ucRet].stSession.iSID)
			{
				pstSlot->astDataPort [ucRet].stSession.pf_notify (pstSlot->astDataPort [ucRet].stSession.pvPtr, HAL_CX930XX_NOTIFY_FORCED_DETACH, 0x00);
			}
		}

		hal_cx930xx_switch_off (pstHAL, pstSlot);

		ucRet = array_get_next (&pstHAL->stSlots, pstItem, &pstItem);
	}
}

void hal_cx930xx_send_delay (void *p_pvPtr, unsigned int len)
{
  unsigned long delay;

  delay = len * 0.15;
  delay *= 40;

  ISLEEP (delay);
}
