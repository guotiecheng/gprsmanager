#include "hal_uc864e.h"

unsigned char hal_uc864e_init (HAL_UC864E *p_pstHAL, unsigned char p_ucNetworkMode)
{
	unsigned char	ucIndex;

	char			acDrvFile [64]	= { 0 };

	memset (p_pstHAL, 0x00, sizeof (HAL_UC864E));

  p_pstHAL->ucStatus = SYS_STATUS_HARDWARE_FAILURE;

	for (ucIndex = 0; HAL_UC864E_MAX_DATA_PORT > ucIndex; ucIndex++)
	{
		p_pstHAL->astDataPort [ucIndex].ucIndex = ucIndex;
	}

	hal_uc864e_get_ctrl_port_file (p_pstHAL, acDrvFile, sizeof (acDrvFile) - 1);

	if (0 != acDrvFile [0])
	{
		comport_init_voice (&p_pstHAL->stCtrlPort, acDrvFile, 0, 0, 0, 0, 0, 0, 0, 0, 0);

		if (0x00 != comport_open_voice (&p_pstHAL->stCtrlPort))
		{
			return 0x03;
		}
	}

	critical_init (&p_pstHAL->stLock);
	thread_init (&p_pstHAL->stThread, (void *) p_pstHAL);

	p_pstHAL->ucNetworkMode = p_ucNetworkMode;
	p_pstHAL->ucCurSIM = 0x01;	// Default is to select SIM1

	p_pstHAL->ucInit = 0x01;

  p_pstHAL->ucStatus = 0x00;

	return 0x00;
}

void hal_uc864e_term (HAL_UC864E *p_pstHAL)
{
	if (0x01 == p_pstHAL->ucInit)
	{
		hal_uc864e_log (p_pstHAL, HAL_UC864E_LOG_TERMINATING_CODE, 0x00, __LINE__);

		// Kill worker process
		hal_uc864e_close (p_pstHAL);

		// Close all usable ports
		hal_uc864e_port (p_pstHAL, 0x01, 0x00, HAL_UC864E_DATA_PORT_MODE_CLOSE);
		hal_uc864e_port (p_pstHAL, 0x02, 0x00, HAL_UC864E_DATA_PORT_MODE_CLOSE);

		// Close ctrl driver
		comport_term_voice (&p_pstHAL->stCtrlPort);

		memset (p_pstHAL, 0x00, sizeof (HAL_UC864E));

		hal_uc864e_log (p_pstHAL, HAL_UC864E_LOG_TERMINATED_CODE, 0x00, __LINE__);
	}
}

unsigned char hal_uc864e_start (HAL_UC864E *p_pstHAL)
{
	return thread_start (&p_pstHAL->stThread, hal_uc864e_body);
}

unsigned char hal_uc864e_close (HAL_UC864E *p_pstHAL)
{
	return thread_close (&p_pstHAL->stThread, SIGINT, 15000);
}

unsigned char hal_uc864e_attach (HAL_UC864E *p_pstHAL, unsigned char p_ucPort, unsigned char p_ucPriority, unsigned char p_ucInterrupt,
								  HAL_UC864E_NOTIFY p_pf_notify, void *p_pvPtr, int *p_piSID)
{
	unsigned char			ucRet,
							ucIndex;

	HAL_UC864E_DATA_PORT	*pstDataPort	= 0;

	if (HAL_UC864E_MAX_DATA_PORT < p_ucPort || 0 >= p_ucPort)
	{
		return 0x01;
	}

	pstDataPort = &p_pstHAL->astDataPort [p_ucPort - 1];

	if (0x00 != critical_lock (&p_pstHAL->stLock, 0x00))
	{
		return 0x01;
	}

	if (0 != pstDataPort->stSession.iSID)
	{
		// Check the holder priority with caller priority
		if (p_ucPriority > pstDataPort->stSession.ucPriority || (0x01 == p_ucInterrupt && p_ucPriority == pstDataPort->stSession.ucPriority))
		{
			// Request holder to detach the data port
			pstDataPort->stSession.pf_notify (pstDataPort->stSession.pvPtr, HAL_UC864E_NOTIFY_REQ_DETACH, 0x00, pstDataPort->stSession.iSID);

			ucRet = 0xA0;	// Attach is pending
			goto CleanUp;
		}

		ucRet = 0xF0;	// Attach is rejected
		goto CleanUp;
	}

	if (0x01 != p_pstHAL->ucReady)
	{
		ucRet = 0xA1;	// Attach is not ready
		goto CleanUp;
	}

	ucIndex = pstDataPort->ucIndex + 0x01;

	ucRet = hal_uc864e_port (p_pstHAL, ucIndex, 0x00, HAL_UC864E_DATA_PORT_MODE_OPEN);

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
	critical_unlock (&p_pstHAL->stLock);

	return ucRet;
}

unsigned char hal_uc864e_detach (HAL_UC864E *p_pstHAL, int p_iSID)
{
	unsigned char			ucRet;

	HAL_UC864E_DATA_PORT	*pstDataPort	= 0;

	if (0x00 != critical_lock (&p_pstHAL->stLock, 0x00))
	{
		return 0x01;
	}

	if (0 < p_iSID)
	{
		for (ucRet = 0x00; HAL_UC864E_MAX_DATA_PORT > ucRet; ucRet++)
		{
			if (p_iSID == p_pstHAL->astDataPort [ucRet].stSession.iSID)
			{
				pstDataPort = &p_pstHAL->astDataPort [ucRet];
				break;
			}
		}

		// Caller is not the owner
		if (0 == pstDataPort)
		{
			ucRet = 0x01;
			goto CleanUp;
		}

		// Close port
		ucRet = pstDataPort->ucIndex + 0x01;
		hal_uc864e_port (p_pstHAL, ucRet, 0x00, HAL_UC864E_DATA_PORT_MODE_CLOSE);

		// Caller going to detach data port
		pstDataPort->stSession.iSID = 0;
		pstDataPort->stSession.ucPriority = 0;

		pstDataPort->stSession.pf_notify = 0;
		pstDataPort->stSession.pvPtr = 0;
	}

	ucRet = 0x00;

CleanUp:
	critical_unlock (&p_pstHAL->stLock);

	return ucRet;
}

unsigned char hal_uc864e_request (HAL_UC864E *p_pstHAL, int p_iSID, unsigned char p_ucOpID, ...)
{
  unsigned char         ucRet,
                        ucDetach        = 0x00,
                        *pucProcToken;

  char					        *pcCaller,
                        *pcRet;

  int                   iSim,
                        iFlag,
                        iNetworkMode,
                        iPort,

                        *piRet;

  va_list               args;

  HAL_UC864E_DATA_PORT	*pstDataPort    = 0;

	va_start (args, p_ucOpID);

	if (-1 == p_iSID)
	{
		pstDataPort = &p_pstHAL->astDataPort [0];
	}
	else if (0 < p_iSID)
	{
		if (0x00 != critical_lock (&p_pstHAL->stLock, 0x01))
		{
			return 0x01;
		}

		for (ucRet = 0x00; HAL_UC864E_MAX_DATA_PORT > ucRet; ucRet++)
		{
			if (p_iSID == p_pstHAL->astDataPort [ucRet].stSession.iSID)
			{
				pstDataPort = &p_pstHAL->astDataPort [ucRet];
				break;
			}
		}

		critical_unlock (&p_pstHAL->stLock);
	}

	if (0 == pstDataPort)
	{
		ucRet = 0x01;
		goto CleanUp;
	}

	switch (p_ucOpID)
	{
	case HAL_UC864E_REQ_RESET:
		// Check all the holder priority with caller priority
		if (-1 != p_iSID)
		{
			for (ucRet = 0x00; HAL_UC864E_MAX_DATA_PORT > ucRet; ucRet++)
			{
				if (pstDataPort->stSession.iSID != p_pstHAL->astDataPort [ucRet].stSession.iSID && 0 != p_pstHAL->astDataPort [ucRet].stSession.iSID &&
					pstDataPort->stSession.ucPriority < p_pstHAL->astDataPort [ucRet].stSession.ucPriority)
				{
					ucRet = 0x02;
					goto CleanUp;
				}
			}
		}

		hal_uc864e_log (p_pstHAL, HAL_UC864E_LOG_RESETTING_CODE, 0x00, __LINE__);

		p_pstHAL->ucReset = 0x01;
		p_pstHAL->ucReady = 0x00;
		ucDetach = 0x01;

		ucRet = 0x00;
		break;

	case HAL_UC864E_REQ_DETACH:
		// Check all the holder priority with caller priority
		if (-1 != p_iSID)
		{
			for (ucRet = 0x00; HAL_UC864E_MAX_DATA_PORT > ucRet; ucRet++)
			{
				if (pstDataPort->stSession.iSID != p_pstHAL->astDataPort [ucRet].stSession.iSID && 0 != p_pstHAL->astDataPort [ucRet].stSession.iSID &&
					pstDataPort->stSession.ucPriority < p_pstHAL->astDataPort [ucRet].stSession.ucPriority)
				{
					ucRet = 0x02;
					goto CleanUp;
				}
			}
		}

		p_pstHAL->ucReady = 0x00;
		ucDetach = 0x01;

		ucRet = 0x00;
		break;

	case HAL_UC864E_REQ_CHG_SIM:
		// Check all the holder priority with caller priority
		if (-1 != p_iSID)
		{
			for (ucRet = 0x00; HAL_UC864E_MAX_DATA_PORT > ucRet; ucRet++)
			{
				if (pstDataPort->stSession.iSID != p_pstHAL->astDataPort [ucRet].stSession.iSID && 0 != p_pstHAL->astDataPort [ucRet].stSession.iSID &&
					pstDataPort->stSession.ucPriority < p_pstHAL->astDataPort [ucRet].stSession.ucPriority)
				{
					ucRet = 0x02;
					goto CleanUp;
				}
			}
		}

		piRet = va_arg (args, int *);	// Requested SIM slot
		iSim = *piRet;

		#if defined (ARCH_L100) || defined (ARCH_L300)

			// Restrict use on SIM 1 only
			if (0x01 != iSim)
			{
				iSim = 0x01;
			}

		#endif

		if ((0x01 == iSim || 0x02 == iSim) && iSim != p_pstHAL->ucCurSIM)
		{
			if (0x00 != hal_uc864e_sim_door (p_pstHAL, (unsigned char) iSim))
			{
				*piRet = (int) p_pstHAL->ucCurSIM;	// Forced to use current SIM
				ucRet = 0x04;
				goto CleanUp;
			}

			hal_uc864e_log (p_pstHAL, HAL_UC864E_LOG_CHANGE_SIM_CODE, 0x00, __LINE__, (int) p_pstHAL->ucCurSIM, iSim);

			p_pstHAL->ucCurSIM = (unsigned char) iSim;
			*piRet = iSim;

			p_pstHAL->ucReset = 0x01;
			p_pstHAL->ucReady = 0x00;
			ucDetach = 0x01;

			// Notify all holder switching sim
			for (ucRet = 0x00; HAL_UC864E_MAX_DATA_PORT > ucRet; ucRet++)
			{
				if (0 != p_pstHAL->astDataPort [ucRet].stSession.iSID)
				{
					p_pstHAL->astDataPort [ucRet].stSession.pf_notify (p_pstHAL->astDataPort [ucRet].stSession.pvPtr, HAL_UC864E_NOTIFY_SIM, 0x00, iSim);
				}
			}
		}

		ucRet = 0x00;
		break;

	case HAL_UC864E_REQ_OPEN_DATA_PORT:
		ucRet = pstDataPort->ucIndex + 0x01;
		ucRet = hal_uc864e_port (p_pstHAL, ucRet, 0x00, HAL_UC864E_DATA_PORT_MODE_OPEN);
		break;

	case HAL_UC864E_REQ_CLOSE_DATA_PORT:
		ucRet = pstDataPort->ucIndex + 0x01;
		ucRet = hal_uc864e_port (p_pstHAL, ucRet, 0x00, HAL_UC864E_DATA_PORT_MODE_CLOSE);
		break;

	case HAL_UC864E_REQ_SET_NETWORK_MODE:
		iNetworkMode = va_arg (args, int);

		if (0 != iNetworkMode && HAL_UC864E_NETWORK_MODE_GPRS != iNetworkMode && HAL_UC864E_NETWORK_MODE_HSPDA != iNetworkMode)
		{
			ucRet = 0x05;
			goto CleanUp;
		}
		else if (0 == iNetworkMode)
		{
			iNetworkMode = HAL_UC864E_NETWORK_MODE_DEFAULT;	// Set to default value
		}

		p_pstHAL->ucNetworkMode = (unsigned char) iNetworkMode;

		// Notify all holder switching sim
		for (ucRet = 0x00; HAL_UC864E_MAX_DATA_PORT > ucRet; ucRet++)
		{
			if (0 != p_pstHAL->astDataPort [ucRet].stSession.iSID)
			{
				p_pstHAL->astDataPort [ucRet].stSession.pf_notify (p_pstHAL->astDataPort [ucRet].stSession.pvPtr, HAL_UC864E_NOTIFY_SET_NETWORK_MODE, 0x00);
			}
		}

		ucRet = 0x00;
		break;

	case HAL_UC864E_REQ_NETWORK_SIGNAL:
		iPort = va_arg (args, int);

		piRet = va_arg (args, int *);
		pucProcToken = va_arg (args, unsigned char *);
		pcCaller = va_arg (args, char *);

		*piRet = 0;

		if (HAL_UC864E_MAX_DATA_PORT >= iPort || 0 < iPort)
		{
			if (0x00 == critical_lock (&p_pstHAL->stLock, 0x01))
			{
				hal_uc864e_signal (p_pstHAL, p_iSID, (unsigned char) iPort, piRet, pucProcToken, pcCaller);

				critical_unlock (&p_pstHAL->stLock);
			}
		}

		ucRet = 0x00;
		break;

	case HAL_UC864E_REQ_GET_DATA_PORT_FILE:
		pcRet = va_arg (args, char *);

		strcpy (pcRet, pstDataPort->stPort.acDevFile);

		ucRet = 0x00;
		break;

	case HAL_UC864E_REQ_SET_SIM_AVAILABILITY:
		iSim = va_arg (args, int);
		iFlag = va_arg (args, int);

		if (0 == iFlag)
		{
			if (1 == iSim)
			{
				p_pstHAL->ucGotSIM_1 = 0x00;
			}
			else if (2 == iSim)
			{
				p_pstHAL->ucGotSIM_2 = 0x00;
			}

			if (iSim == p_pstHAL->ucCurSIM)
			{
				ucDetach = 0x01;
			}
		}

		ucRet = 0x00;
		break;

	default:
		ucRet = 0xF0;
	}

	if (0x01 == ucDetach)
	{
		// Notify all holder to detach port
		for (ucRet = 0x00; HAL_UC864E_MAX_DATA_PORT > ucRet; ucRet++)
		{
			if (0 != p_pstHAL->astDataPort [ucRet].stSession.iSID)
			{
				p_pstHAL->astDataPort [ucRet].stSession.pf_notify (p_pstHAL->astDataPort [ucRet].stSession.pvPtr, HAL_UC864E_NOTIFY_FORCED_DETACH, 0x00, p_pstHAL->astDataPort [ucRet].stSession.iSID);
			}
		}
	}

CleanUp:
	va_end (args);

	return ucRet;
}

unsigned char hal_uc864e_signal (HAL_UC864E *p_pstHAL, int p_iSID, unsigned char p_ucPort, int *p_piSignal, unsigned char *p_pucProcToken, char *p_pcCaller)
{
	unsigned char			ucRet;

	char					acRet [64],
							acBuf [16],
							*pcStart,
							*pcEnd;

	int						iSignal;

	HAL_UC864E_DATA_PORT	*pstDataPort		= &p_pstHAL->astDataPort [p_ucPort - 1];

	*p_piSignal = 0;

	if (0x01 != pstDataPort->stPort.ucIsConnected)
	{
		ucRet = hal_uc864e_port (p_pstHAL, p_ucPort, 0x00, HAL_UC864E_DATA_PORT_MODE_OPEN);

		if (0x00 != ucRet)
		{
			ucRet = 0x01;
			goto CleanUp;
		}
	}

	ucRet = hal_uc864e_at_cmd (p_pstHAL, 0 - p_ucPort, "AT+CSQ\r", "OK", "ERROR", acRet, sizeof (acRet),
		0, 0, 5000, p_pucProcToken, __LINE__, p_pcCaller);

	hal_uc864e_port (p_pstHAL, p_ucPort, 0x00, HAL_UC864E_DATA_PORT_MODE_CLOSE);

	if (0x00 == ucRet)
	{
		// Get the signal strength value
		pcStart = strstr (acRet, "+CSQ: ");

		if (0 != pcStart)
		{
			pcStart += 6;
			pcEnd = strchr (pcStart, ',');

			if (0 != pcEnd && 0 < (pcEnd - pcStart) && sizeof (acBuf) > (pcEnd - pcStart))
			{
				memset (acBuf, 0x00, sizeof (acBuf));
				strncpy (acBuf, pcStart, pcEnd - pcStart);

				iSignal = atoi (acBuf);

				if (0 < iSignal && 31 >= iSignal)
				{
					*p_piSignal = iSignal;
				}
			}
		}
	}

CleanUp:

	return ucRet;
}

unsigned char hal_uc864e_at_cmd (HAL_UC864E *p_pstHAL, int p_iSID, char *p_pcReq, char *p_pcRes, char *p_pcErr, char *p_pcRet, int p_iRetSize,
								  unsigned long p_ulDelay, unsigned long p_ulGap, unsigned long p_ulTimeout, unsigned char *p_pucProcToken, unsigned int p_uiLine, char *p_pcName)
{
	unsigned char			ucRet;

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

	HAL_UC864E_DATA_PORT	*pstDataPort		= 0;

	if (-1 == p_iSID)
	{
		pstDataPort = &p_pstHAL->astDataPort [0];	// Default port for controller
	}
	else if (-2 == p_iSID)
	{
		pstDataPort = &p_pstHAL->astDataPort [1];	// Default port for controller when powering
	}
	else if (0 < p_iSID)
	{
		if (0x00 != critical_lock (&p_pstHAL->stLock, 0x00))
		{
			return 0x01;
		}

		for (ucRet = 0x00; HAL_UC864E_MAX_DATA_PORT > ucRet; ucRet++)
		{
			if (p_iSID == p_pstHAL->astDataPort [ucRet].stSession.iSID)
			{
				pstDataPort = &p_pstHAL->astDataPort [ucRet];
				break;
			}
		}

		critical_unlock (&p_pstHAL->stLock);
	}

	if (0 == pstDataPort)
	{
		return 0x01;
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
		while (ulCurTime - ulStartTime < p_ulDelay && ulStartTime <= ulCurTime && 0x01 == *p_pucProcToken)
		{
			ISLEEP (1);
			IGET_TICKCOUNT (ulCurTime);
		}

		if (0x01 != *p_pucProcToken)
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

	while (ulCurTime - ulStartTime < p_ulTimeout && ulCurTime >= ulStartTime && 0x01 == *p_pucProcToken)
	{
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

    //special handle +++ (TN)
    if (0 == strcmp(p_pcReq, "+++"))
    {
        iCmdLen = 1;
        strcpy (p_pcReq, "+");
      	ucRet = comport_send_voice (&pstDataPort->stPort, (unsigned char *) p_pcReq, iCmdLen);
        ISLEEP (20); //Pause 50ms
      	ucRet = comport_send_voice (&pstDataPort->stPort, (unsigned char *) p_pcReq, iCmdLen);
        ISLEEP (20); //Pause 50ms
    }
    //special handle +++ (TN)

		ucRet = comport_send_voice (&pstDataPort->stPort, (unsigned char *) p_pcReq, iCmdLen);

		if (0 != ucRet)
		{
			if (0 != p_pcName)
			{
				hal_uc864e_log (p_pstHAL, HAL_UC864E_LOG_DATA_PORT_FAILURE_CODE, ucRet, __LINE__, (int) pstDataPort->ucIndex);
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
		while (ulCurTime - ulStartTime < p_ulGap && ulStartTime <= ulCurTime && 0x01 == *p_pucProcToken)
		{
			ISLEEP (1);
			IGET_TICKCOUNT (ulCurTime);
		}

		if (0x01 != *p_pucProcToken)
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

	while (ulCurTime - ulStartTime < p_ulTimeout && ulCurTime >= ulStartTime && 0x01 == *p_pucProcToken)
	{
		if (iRecvLen < iRecvSize - 1)
		{
			ucRet = comport_recv_voice (&pstDataPort->stPort, (unsigned char *) pcRecvPtr, 1, &iRet, 50);

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

	if (0x01 == *p_pucProcToken)
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
	  memset (p_pcRet, 0x00, p_iRetSize);
		strncpy (p_pcRet, acLog, p_iRetSize - 1);
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

		hal_uc864e_log (p_pstHAL, HAL_UC864E_LOG_AT_CMD_CODE, ucRet, p_uiLine, p_pcName, acLog);
	}

	return ucRet;
}


unsigned char hal_uc864e_send (HAL_UC864E *p_pstHAL, int p_iSID, unsigned char *p_pucData, int p_iLen)
{
	unsigned char			ucRet;

	HAL_UC864E_DATA_PORT	*pstDataPort		= 0;

	if (0x00 != critical_lock (&p_pstHAL->stLock, 0x01))
	{
		return 0x01;
	}

	if (-1 == p_iSID)
	{
		pstDataPort = &p_pstHAL->astDataPort [0];	// Default port for controller
	}
	else if (0 != p_iSID)
	{
		for (ucRet = 0x00; HAL_UC864E_MAX_DATA_PORT > ucRet; ucRet++)
		{
			if (p_iSID == p_pstHAL->astDataPort [ucRet].stSession.iSID)
			{
				pstDataPort = &p_pstHAL->astDataPort [ucRet];
				break;
			}
		}
	}

	critical_unlock (&p_pstHAL->stLock);

	if (0 == pstDataPort)
	{
		return 0x01;
	}

	return comport_send_voice (&pstDataPort->stPort, p_pucData, p_iLen);
}

unsigned char hal_uc864e_recv (HAL_UC864E *p_pstHAL, int p_iSID, unsigned char *p_pucData, int p_iSize, int *p_piLen, unsigned long p_ulTimeout)
{
	unsigned char			ucRet;

	HAL_UC864E_DATA_PORT	*pstDataPort		= 0;

	if (0x00 != critical_lock (&p_pstHAL->stLock, 0x01))
	{
		return 0x01;
	}

	if (-1 == p_iSID)
	{
		pstDataPort = &p_pstHAL->astDataPort [0];	// Default port for controller
	}
	else if (0 != p_iSID)
	{
		for (ucRet = 0x00; HAL_UC864E_MAX_DATA_PORT > ucRet; ucRet++)
		{
			if (p_iSID == p_pstHAL->astDataPort [ucRet].stSession.iSID)
			{
				pstDataPort = &p_pstHAL->astDataPort [ucRet];
				break;
			}
		}
	}

	critical_unlock (&p_pstHAL->stLock);

	if (0 == pstDataPort)
	{
		return 0x01;
	}

	return comport_recv_voice (&pstDataPort->stPort, p_pucData, p_iSize, p_piLen, p_ulTimeout);
}

unsigned char hal_uc864e_switch_on (HAL_UC864E *p_pstHAL, unsigned char p_ucSim, unsigned char *p_pucProcToken)
{
	unsigned char		ucRet			= 0x01;

	unsigned long		ulCurTime,
						ulStartTime		= 0;

	// Close all ports
	hal_uc864e_port (p_pstHAL, 0x01, 0x00, HAL_UC864E_DATA_PORT_MODE_CLOSE);
	hal_uc864e_port (p_pstHAL, 0x02, 0x00, HAL_UC864E_DATA_PORT_MODE_CLOSE);

	if (0x01 == p_pstHAL->ucReset)
	{
		hal_uc864e_log (p_pstHAL, HAL_UC864E_LOG_RESET_CODE, 0x00, __LINE__);
		hal_uc864e_reset (p_pstHAL, p_ucSim);
	}
	else
	{
		hal_uc864e_log (p_pstHAL, HAL_UC864E_LOG_POWERING_ON_CODE, 0x00, __LINE__);
		hal_uc864e_power_on (p_pstHAL, p_ucSim);
	}

	IGET_TICKCOUNT (ulCurTime);
	ulStartTime = ulCurTime;

	while (0x01 == p_pstHAL->stThread.ucProcToken)
	{
		IGET_TICKCOUNT (ulCurTime);

		if (ulCurTime - ulStartTime >= 10000 && ulCurTime >= ulStartTime)
		{
			break;
		}
		else if (ulCurTime - ulStartTime >= 5000 && ulCurTime - ulStartTime < 10000 && ulCurTime >= ulStartTime)
		{
			// Open tty ports
			ucRet = hal_uc864e_port (p_pstHAL, 0x02, 0x00, HAL_UC864E_DATA_PORT_MODE_OPEN);

			if (0x00 != ucRet)
			{
				ISLEEP (1);
			}
			else
			{
				// Try AT command response on tty port
				ucRet = hal_uc864e_at_cmd (p_pstHAL, -2, "AT\r", "OK", "ERROR", 0, 0, 0, 0, 1000, p_pucProcToken, __LINE__, 0);

				if (0x00 == ucRet)
				{
					hal_uc864e_log (p_pstHAL, HAL_UC864E_LOG_POWERED_ON_CODE, 0x00, __LINE__, p_ucSim);
					goto CleanUp;
				}
				else if (0x02 == ucRet)
				{
					hal_uc864e_log (p_pstHAL, HAL_UC864E_LOG_POWER_ON_FAILURE_CODE, ucRet, __LINE__);
					p_pstHAL->ucReset = 0x01;
					goto CleanUp;
				}
			}
		}
		else if (ulCurTime < ulStartTime)
		{
			ulStartTime = ulCurTime;
		}
	}

	hal_uc864e_log (p_pstHAL, HAL_UC864E_LOG_POWER_ON_FAILURE_CODE, ucRet, __LINE__);
	p_pstHAL->ucReset = 0x01;

CleanUp:
	// Close all ports
	hal_uc864e_port (p_pstHAL, 0x01, 0x00, HAL_UC864E_DATA_PORT_MODE_CLOSE);
	hal_uc864e_port (p_pstHAL, 0x02, 0x00, HAL_UC864E_DATA_PORT_MODE_CLOSE);

	return ucRet;
}

unsigned char hal_uc864e_switch_off (HAL_UC864E *p_pstHAL, unsigned char *p_pucProcToken)
{
	unsigned char		ucGo;

	unsigned long		ulCurTime,
						ulLastOp;

	int					iRet;

	hal_uc864e_log (p_pstHAL, HAL_UC864E_LOG_POWERING_OFF_CODE, 0x00, __LINE__);

	p_pstHAL->ucReady = 0x00;

	// == Notify holder to detach port ==
	for (iRet = 0; HAL_UC864E_MAX_DATA_PORT > iRet; iRet++)
	{
		if (0 != p_pstHAL->astDataPort [iRet].stSession.iSID)
		{
			p_pstHAL->astDataPort [iRet].stSession.pf_notify (p_pstHAL->astDataPort [iRet].stSession.pvPtr, HAL_UC864E_NOTIFY_FORCED_DETACH, 0x00, p_pstHAL->astDataPort [iRet].stSession.iSID);
		}
	}

	// == Wait every holder to detach port
	IGET_TICKCOUNT (ulCurTime);
	ulLastOp = ulCurTime;

	while (ulCurTime - ulLastOp < 5000 && ulCurTime >= ulLastOp)
	{
		ucGo = 0x01;

		for (iRet = 0; HAL_UC864E_MAX_DATA_PORT > iRet; iRet++)
		{
			if (0 != p_pstHAL->astDataPort [iRet].stSession.iSID)
			{
				ucGo = 0x00;
				break;
			}
		}

		if (0x01 == ucGo)
		{
			break;
		}

		ISLEEP (50);
		IGET_TICKCOUNT (ulCurTime);
	}

	hal_uc864e_port (p_pstHAL, 0x01, 0x00, HAL_UC864E_DATA_PORT_MODE_CLOSE);
	hal_uc864e_port (p_pstHAL, 0x02, 0x00, HAL_UC864E_DATA_PORT_MODE_CLOSE);

	hal_uc864e_power_off (p_pstHAL);

	hal_uc864e_log (p_pstHAL, HAL_UC864E_LOG_POWERED_OFF_CODE, 0x00, __LINE__);

	return 0x00;
}

unsigned char hal_uc864e_get_ready (HAL_UC864E *p_pstHAL)
{
	unsigned char	ucRet;

	int				iIndex;

	// Open first port
	ucRet = hal_uc864e_port (p_pstHAL, 0x01, 0x00, HAL_UC864E_DATA_PORT_MODE_OPEN);

	if (0x00 != ucRet)
	{
		hal_uc864e_log (p_pstHAL, HAL_UC864E_LOG_DATA_PORT_FAILURE_CODE, ucRet, __LINE__);
		goto CleanUp;
	}


	// ================
	// == Detect SIM ==
	// ================

	logger_log (&g_stVoiceLog, LOG_NORMAL, "@%04d %s: Detecting SIM %d", __LINE__, HAL_UC864E_NAME, p_pstHAL->ucCurSIM);

	for (iIndex = 0; 5 > iIndex && 0x01 == p_pstHAL->stThread.ucProcToken; iIndex++)
	{
		ucRet = hal_uc864e_at_cmd (p_pstHAL, -1, "AT+CPIN?\r", "+CPIN: READY\r\n\r\nOK", "ERROR",
			0, 0, 0, 0, 5000, &p_pstHAL->stThread.ucProcToken, __LINE__, HAL_UC864E_NAME);

		if (0x00 == ucRet)
		{
			break;
		}
		else if (0x03 == ucRet)
		{
			ISLEEP (1000);
		}
		else if (0x04 == ucRet || 0x02 == ucRet)	// timeout with no response or data port failure
		{
			break;
		}
	}

	if (0x01 != p_pstHAL->stThread.ucProcToken)
	{
		ucRet = 0xFF;
		goto CleanUp;
	}

	if (0x00 != ucRet)
	{
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Current active SIM %d is not ready. RetCode [0x%02X]", __LINE__, HAL_UC864E_NAME, p_pstHAL->ucCurSIM, ucRet);

		ucRet = 0x02;
		goto CleanUp;
	}

	logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Current active SIM %d is ready", __LINE__, HAL_UC864E_NAME, p_pstHAL->ucCurSIM);
	ucRet = 0x00;

CleanUp:
	// Close port
	hal_uc864e_port (p_pstHAL, 0x01, 0x00, HAL_UC864E_DATA_PORT_MODE_CLOSE);

	return ucRet;
}

void hal_uc864e_power_on (HAL_UC864E *p_pstHAL, unsigned char p_ucSim)
{
	if (0x00 == p_pstHAL->stCtrlPort.ucIsConnected)
	{
		comport_open_voice (&p_pstHAL->stCtrlPort);
	}

	if (0 < p_pstHAL->stCtrlPort.hComPort)
	{
		#if defined (ARCH_L300)

			if (0x01 == p_ucSim)
			{
				p_ucSim = 0x02;	// The first SIM slot is the second slot
			}

		#endif

		#if defined (ARCH_L100) || defined (ARCH_L200) || defined (ARCH_L300) || defined (ARCH_L350) || defined (ARCH_N300)

			if (-1 == ioctl (p_pstHAL->stCtrlPort.hComPort, 1, p_ucSim))
			{
				comport_open_voice (&p_pstHAL->stCtrlPort);	// Re-open
			}

		#endif
	}
}

void hal_uc864e_reset (HAL_UC864E *p_pstHAL, unsigned char p_ucSim)
{
	if (0x00 == p_pstHAL->stCtrlPort.ucIsConnected)
	{
		comport_open_voice (&p_pstHAL->stCtrlPort);
	}

	if (0 < p_pstHAL->stCtrlPort.hComPort)
	{
		#if defined (ARCH_L300)

			if (0x01 == p_ucSim)
			{
				p_ucSim = 0x02;	// The first SIM slot is the second slot
			}

		#endif

		#if defined (ARCH_L100) || defined (ARCH_L200) || defined (ARCH_L300) || defined (ARCH_L350) || defined (ARCH_N300)

			if (-1 == ioctl (p_pstHAL->stCtrlPort.hComPort, 18, p_ucSim))
			{
				comport_open_voice (&p_pstHAL->stCtrlPort);	// Re-open
			}

		#endif
	}
}

void hal_uc864e_power_off (HAL_UC864E *p_pstHAL)
{
	unsigned long	ulCurTime,
					ulLastTime		= 0;

	#if defined (ARCH_L100) || defined (ARCH_L200) || defined (ARCH_L300) || defined (ARCH_L350) || defined (ARCH_N300)

		int			iRet			= 0;

	#endif

	if (0x00 == p_pstHAL->stCtrlPort.ucIsConnected)
	{
		comport_open_voice (&p_pstHAL->stCtrlPort);
	}

	if (0 < p_pstHAL->stCtrlPort.hComPort)
	{
		#if defined (ARCH_L100) || defined (ARCH_L200) || defined (ARCH_L300) || defined (ARCH_L350) || defined (ARCH_N300)

			ioctl (p_pstHAL->stCtrlPort.hComPort, 2, 0);

			while (1)
			{
				IGET_TICKCOUNT (ulCurTime);

				if (ulCurTime - ulLastTime >= 10000 && ulCurTime >= ulLastTime && 0 != ulLastTime)
				{
					break;
				}
				else if (ulCurTime < ulLastTime || 0 == ulLastTime)
				{
					ulLastTime = ulCurTime;
				}

				// power monitor return 0 means power off, else power on
				iRet = ioctl (p_pstHAL->stCtrlPort.hComPort, 22, 0);

				if (0 == iRet)
				{
					break;
				}
				else if (-1 == iRet)
				{
					comport_open_voice (&p_pstHAL->stCtrlPort);	// Re-open
				}

				ISLEEP (1);
			}

		#else

			ulCurTime = 0;
			ulLastTime = 0;

		#endif
	}
}

unsigned char hal_uc864e_ring (HAL_UC864E *p_pstHAL)
{
	unsigned char	ucRet			= 0x01;

	#if defined (ARCH_L100) || defined (ARCH_L200) || defined (ARCH_L300) || defined (ARCH_L350) || defined (ARCH_N300)

		int			iRet			= 0;

	#endif

	if (0x00 == p_pstHAL->stCtrlPort.ucIsConnected)
	{
		comport_open_voice (&p_pstHAL->stCtrlPort);
	}

	if (0 < p_pstHAL->stCtrlPort.hComPort && 0x01 == p_pstHAL->ucReady)
	{
		#if defined (ARCH_L100) || defined (ARCH_L200) || defined (ARCH_L300) || defined (ARCH_L350) || defined (ARCH_N300)

			iRet = ioctl (p_pstHAL->stCtrlPort.hComPort, 7, 0);

			if (1 == iRet)
			{
				ucRet = 0x00;	// RING
			}
			else if (-1 == iRet)
			{
				comport_open_voice (&p_pstHAL->stCtrlPort);	// Re-open
			}

		#endif
	}

	return ucRet;
}

// Return 0x01: sim door removed, else sim door closed
unsigned char hal_uc864e_sim_door (HAL_UC864E *p_pstHAL, unsigned char p_ucSim)
{
	unsigned char	ucRet			= 0x00;

	#if defined (ARCH_L100) || defined (ARCH_L200) || defined (ARCH_L300) || defined (ARCH_L350) || defined (ARCH_N300)

		int			iRet			= 0;

	#endif

	if (0x00 == p_pstHAL->stCtrlPort.ucIsConnected)
	{
		comport_open_voice (&p_pstHAL->stCtrlPort);
	}

	if (0 < p_pstHAL->stCtrlPort.hComPort)
	{
		#if defined (ARCH_L300)

			if (0x01 == p_ucSim)
			{
				p_ucSim = 0x02;	// The first SIM slot is the second slot
			}

		#endif

		#if defined (ARCH_L100) || defined (ARCH_L200) || defined (ARCH_L300) || defined (ARCH_L350) || defined (ARCH_N300)

			// return 0 means sim slot door is close, else sim slot door is open
			iRet = ioctl (p_pstHAL->stCtrlPort.hComPort, 13, p_ucSim);

			if (1 == iRet)
			{
				ucRet = 0x01;	// sim door removed
			}
			else if (-1 == iRet)
			{
				comport_open_voice (&p_pstHAL->stCtrlPort);	// Re-open
			}

		#endif
	}

	return ucRet;
}

void hal_uc864e_dtr (HAL_UC864E *p_pstHAL, unsigned char p_ucFlag)
{
	if (0x00 == p_pstHAL->stCtrlPort.ucIsConnected)
	{
		comport_open_voice (&p_pstHAL->stCtrlPort);
	}

	if (0 < p_pstHAL->stCtrlPort.hComPort && 0x01 == p_pstHAL->ucReady)
	{
		#if defined (ARCH_L100) || defined (ARCH_L200)

			if (0x01 == p_ucFlag)	// HIGH
			{
				hal_uc864e_log (p_pstHAL, HAL_UC864E_LOG_DTR_CODE, 0x01, __LINE__);
				ioctl (p_pstHAL->stCtrlPort.hComPort, 19, 1);
			}
			else	// LOW
			{
				hal_uc864e_log (p_pstHAL, HAL_UC864E_LOG_DTR_CODE, 0x00, __LINE__);
				ioctl (p_pstHAL->stCtrlPort.hComPort, 19, 0);
			}

		#endif
	}
}

unsigned char hal_uc864e_sim (HAL_UC864E *p_pstHAL, unsigned char p_ucSim)
{
	if (0x01 == p_ucSim)
	{
		return p_pstHAL->ucGotSIM_1;
	}
	else
	{
		return p_pstHAL->ucGotSIM_2;
	}
}

unsigned char hal_uc864e_port (HAL_UC864E *p_pstHAL, unsigned char p_ucPort, unsigned char p_ucSetting, unsigned char p_ucMode)
{
	unsigned char			ucRet				= 0x01;

	char					acDrvFile [64]		= { 0 };

	HAL_UC864E_DATA_PORT	*pstPort			= 0;

	if (0x01 != p_pstHAL->ucInit || (0x01 != p_ucPort && 0x02 != p_ucPort))
	{
		ucRet = 0x01;
		goto CleanUp;
	}

	pstPort = &p_pstHAL->astDataPort [p_ucPort - 0x01];

	// Open mode
	if (HAL_UC864E_DATA_PORT_MODE_OPEN == p_ucMode)
	{
		hal_uc864e_get_data_port_file (p_pstHAL, p_ucPort, acDrvFile, sizeof (acDrvFile));

		if (0 == acDrvFile [0])
		{
			ucRet = 0x02;
			goto CleanUp;
		}

		comport_close_voice (&pstPort->stPort);

		if (0x01 == p_ucSetting)
		{
			comport_init_voice (&pstPort->stPort, acDrvFile, HAL_UC864E_FAC_BAUDRATE, HAL_UC864E_FAC_DATA_BIT,
				HAL_UC864E_FAC_STOP_BIT, HAL_UC864E_FAC_PARITY, HAL_UC864E_FAC_FLOW_CTRL, HAL_UC864E_FAC_FRAG_SIZE, 0, 0, 0);
		}
		else
		{
			comport_init_voice (&pstPort->stPort, acDrvFile, HAL_UC864E_DEF_BAUDRATE, HAL_UC864E_DEF_DATA_BIT,
				HAL_UC864E_DEF_STOP_BIT, HAL_UC864E_DEF_PARITY, HAL_UC864E_DEF_FLOW_CTRL, HAL_UC864E_DEF_FRAG_SIZE, 0, 0, 0);
		}

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

void hal_uc864e_get_data_port_file (HAL_UC864E *p_pstHAL, unsigned char p_ucPort, char *p_pcDrvFile, int p_iSize)
{
	memset (p_pcDrvFile, 0x00, p_iSize);

	#if defined (ARCH_L100) || defined (ARCH_L200)

		switch (p_ucPort)
		{
		case 0x01:
			strncpy (p_pcDrvFile, "/dev/ttyUC864E0", p_iSize - 1);
			break;

		case 0x02:
			strncpy (p_pcDrvFile, "/dev/ttyS1", p_iSize - 1);
			break;
		}

	#elif defined (ARCH_L300) || defined (ARCH_L350)

		switch (p_ucPort)
		{
		case 0x01:
			strncpy (p_pcDrvFile, "/dev/ttyUSB0", p_iSize - 1);
			break;

		case 0x02:
			strncpy (p_pcDrvFile, "/dev/ttyS2", p_iSize - 1);
			break;
		}

	#elif defined (ARCH_N300)

		switch (p_ucPort)
		{
		case 0x01:
			strncpy (p_pcDrvFile, "/dev/ttyUSB2", p_iSize - 1);
			break;
		case 0x02:
			strncpy (p_pcDrvFile, "/dev/ttyUSB0", p_iSize - 1);
			break;
		}

	#elif defined (WIN32)

		switch (p_ucPort)
		{
		case 0x01:
			strncpy (p_pcDrvFile, "COM1", p_iSize - 1);
			break;

		case 0x02:
			strncpy (p_pcDrvFile, "COM2", p_iSize - 1);
			break;
		}

	#endif
}

void hal_uc864e_get_ctrl_port_file (HAL_UC864E *p_pstHAL, char *p_pcDrvFile, int p_iSize)
{
	memset (p_pcDrvFile, 0x00, p_iSize);

	#if defined (ARCH_L100) || defined (ARCH_L200)

		strncpy (p_pcDrvFile, "/dev/ttyUC864E1", p_iSize - 1);

	#elif defined (ARCH_L300) || defined (ARCH_L350) || defined (ARCH_N300)

		strncpy (p_pcDrvFile, "/dev/ttyUSB1", p_iSize - 1);

	#endif
}

void hal_uc864e_log (HAL_UC864E *p_pstHAL, int p_iLogID, unsigned char p_ucRet, ...)
{
	unsigned int	uiLine;

	int				iCurSim,
					iNewSim,
					iSlot;

	char			*pcName,
					*pcBuf;

	va_list			args;

	va_start (args, p_ucRet);

	uiLine = va_arg (args, unsigned int);

	switch (p_iLogID)
	{
	case HAL_UC864E_LOG_DATA_PORT_FAILURE_CODE:
		iSlot = va_arg (args, int);

		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Data port %d failure. RetCode [0x%02X]", uiLine, HAL_UC864E_NAME, iSlot, p_ucRet);
		break;

	case HAL_UC864E_LOG_POWER_ON_FAILURE_CODE:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Module power on failure. RetCode [0x%02X]", uiLine, HAL_UC864E_NAME, p_ucRet);
		break;

	case HAL_UC864E_LOG_POWERING_ON_CODE:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Module powering on", uiLine, HAL_UC864E_NAME);
		break;

	case HAL_UC864E_LOG_POWERED_ON_CODE:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Module powered on", uiLine, HAL_UC864E_NAME);
		break;

	case HAL_UC864E_LOG_POWERING_OFF_CODE:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Module switching off", uiLine, HAL_UC864E_NAME);
		break;

	case HAL_UC864E_LOG_POWERED_OFF_CODE:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Module switched off", uiLine, HAL_UC864E_NAME);
		break;

	case HAL_UC864E_LOG_RESETTING_CODE:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Module resetting", uiLine, HAL_UC864E_NAME);
		break;

	case HAL_UC864E_LOG_RESET_CODE:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Module reset", uiLine, HAL_UC864E_NAME);
		break;

	case HAL_UC864E_LOG_TERMINATING_CODE:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Module terminating", uiLine, HAL_UC864E_NAME);
		break;

	case HAL_UC864E_LOG_TERMINATED_CODE:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Module terminated", uiLine, HAL_UC864E_NAME);
		break;

	case HAL_UC864E_LOG_CHANGE_SIM_CODE:
		iCurSim = va_arg (args, int);
		iNewSim = va_arg (args, int);

		if (iCurSim != iNewSim)
		{
			logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Module change SIM%d -> SIM%d", uiLine, HAL_UC864E_NAME, iCurSim, iNewSim);
		}
		else
		{
			logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Module change SIM%d", uiLine, HAL_UC864E_NAME, iNewSim);
		}
		break;

	case HAL_UC864E_LOG_DTR_CODE:
		logger_log (&g_stVoiceLog, LOG_DETAIL, "@%04d %s: DTR is %s", uiLine, HAL_UC864E_NAME, (0x00 == p_ucRet) ? "LOW" : "HIGH");
		break;

	case HAL_UC864E_LOG_DATA_PORT_RAW_CODE:
		pcName = va_arg (args, char *);
		pcBuf = va_arg (args, char *);

		logger_log (&g_stVoiceLog, LOG_DETAIL, "@%04d %s: %s", uiLine, pcName, pcBuf);
		break;

	case HAL_UC864E_LOG_AT_CMD_CODE:
		pcName = va_arg (args, char *);
		pcBuf = va_arg (args, char *);

		logger_log (&g_stVoiceLog, LOG_DETAIL, "@%04d %s: %s", uiLine, pcName, pcBuf);
		break;

	default:
		logger_log (&g_stVoiceLog, LOG_DETAIL, "@%04d %s: Event %d happen. RetCode [0x%02X]", uiLine, HAL_UC864E_NAME, p_iLogID, p_ucRet);
	}

	va_end (args);
}

void hal_uc864e_body (void *p_pvThread)
{
	unsigned char		ucRet,
						ucGo;
//						ucWaitSIM			= 0x00;

	unsigned long		ulCurTime,
						ulLastSIM_1			= 0;

	int					iRet;

	HAL_UC864E			*pstHAL				= ((WRK_THREAD *) p_pvThread)->pvPtr;

	#if defined (ARCH_L200) || defined (ARCH_L350) || defined (ARCH_N300)

		unsigned long	ulLastSIM_2			= 0;

	#endif

    if (0x00 == pstHAL)
    {
        logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Module not found", __LINE__, HAL_UC864E_NAME);
        return;
    }

	pstHAL->ucSIM_1_Door = hal_uc864e_sim_door (pstHAL, 0x01);
	pstHAL->ucGotSIM_1 = (0x00 == pstHAL->ucSIM_1_Door) ? 0x01 : 0x00;

	#if defined (ARCH_L200) || defined (ARCH_L350) || defined (ARCH_N300)

		pstHAL->ucSIM_2_Door = hal_uc864e_sim_door (pstHAL, 0x02);
		pstHAL->ucGotSIM_2 = (0x00 == pstHAL->ucSIM_2_Door) ? 0x01 : 0x00;

	#else

		pstHAL->ucSIM_2_Door = 0x01;
		pstHAL->ucGotSIM_2 = 0x00;

	#endif

	// Default is SIM 1 if available
	if (0x01 == pstHAL->ucGotSIM_1)
	{
		pstHAL->ucCurSIM = 0x01;
	}
	else if (0x01 == pstHAL->ucGotSIM_2)
	{
		pstHAL->ucCurSIM = 0x02;
	}
	else
	{
		pstHAL->ucCurSIM = 0x01;
	}

//  pstHAL->ucStatus = 0x00;

	while (0x01 == pstHAL->stThread.ucProcToken)
	{
		if (0x01 != pstHAL->ucReady && (0x01 == pstHAL->ucGotSIM_1 || 0x01 == pstHAL->ucGotSIM_2))
		{
			critical_lock (&pstHAL->stLock, 0x01);

			ucGo = 0x01;

			// Make sure all holder are detached port
			for (iRet = 0; HAL_UC864E_MAX_DATA_PORT > iRet; iRet++)
			{
				if (0 != pstHAL->astDataPort [iRet].stSession.iSID)
				{
					ucGo = 0x00;
					break;
				}
			}

			critical_unlock (&pstHAL->stLock);

			if (0x01 == ucGo)
			{
				ucRet = hal_uc864e_switch_on (pstHAL, pstHAL->ucCurSIM, &pstHAL->stThread.ucProcToken);

				if (0x00 == ucRet)
				{
					pstHAL->ucReady = 0x01;
					pstHAL->ucReset = 0x00;
          if (SYS_STATUS_HARDWARE_FAILURE == pstHAL->ucStatus)
              pstHAL->ucStatus = 0x00;
				}
				else
				{
					pstHAL->ucStatus = SYS_STATUS_HARDWARE_FAILURE;
				}
			}
		}

		// ==================================
		// ** Detect SIM slot 1 door event **
		// ==================================

		ucRet = hal_uc864e_sim_door (pstHAL, 0x01);

		if (0x01 == ucRet)	// Now door is removed
		{
			if (0x00 == pstHAL->ucSIM_1_Door)	// Previous door is restored
			{
				IGET_TICKCOUNT (ulCurTime);

				// Delay a bit before set to sim slot removed to prevent franction
				if (ulCurTime - ulLastSIM_1 >= 2000 && ulCurTime >= ulLastSIM_1 && 0 != ulLastSIM_1)
				{
					pstHAL->ucSIM_1_Door = 0x01;
					pstHAL->ucGotSIM_1 = 0x00;

					if (0x00 == pstHAL->ucGotSIM_1 && 0x01 == pstHAL->ucCurSIM)
					{
						hal_uc864e_request (pstHAL, -1, HAL_UC864E_REQ_DETACH);
					}

					ulLastSIM_1 = 0;

					logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: SIM slot 1 is removed", __LINE__, HAL_UC864E_NAME);
				}
				else if (ulCurTime < ulLastSIM_1 || 0 == ulLastSIM_1)
				{
					ulLastSIM_1 = ulCurTime;
				}
			}
		}
		else	// Now door is restored
		{
			if (0x01 == pstHAL->ucSIM_1_Door)	// Previous door is removed
			{
				IGET_TICKCOUNT (ulCurTime);

				// Delay a bit before set to sim slot restored to prevent franction
				if (ulCurTime - ulLastSIM_1 >= 500 && ulCurTime >= ulLastSIM_1 && 0 != ulLastSIM_1)
				{
					pstHAL->ucSIM_1_Door = 0x00;
					pstHAL->ucGotSIM_1 = 0x01;

					ulLastSIM_1 = 0;

					logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: SIM slot 1 is restored", __LINE__, HAL_UC864E_NAME);
				}
				else if (ulCurTime < ulLastSIM_1 || 0 == ulLastSIM_1)
				{
					ulLastSIM_1 = ulCurTime;
				}
			}
		}

		#if defined (ARCH_L200) || defined (ARCH_L350) || defined (ARCH_N300)

			// ==================================
			// ** Detect SIM slot 2 door event **
			// ==================================

			ucRet = hal_uc864e_sim_door (pstHAL, 0x02);

			if (0x01 == ucRet)	// Now door is removed
			{
				if (0x00 == pstHAL->ucSIM_2_Door)	// Previous door is restored
				{
					IGET_TICKCOUNT (ulCurTime);

					// Delay a bit before set to sim slot removed to prevent franction
					if (ulCurTime - ulLastSIM_2 >= 2000 && ulCurTime >= ulLastSIM_2 && 0 != ulLastSIM_2)
					{
						pstHAL->ucSIM_2_Door = 0x01;
						pstHAL->ucGotSIM_2 = 0x00;

						if (0x00 == pstHAL->ucGotSIM_2 && 0x02 == pstHAL->ucCurSIM)
						{
							hal_uc864e_request (pstHAL, -1, HAL_UC864E_REQ_DETACH);
						}

						ulLastSIM_2 = 0;

						logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: SIM slot 2 is removed", __LINE__, HAL_UC864E_NAME);
					}
					else if (ulCurTime < ulLastSIM_2 || 0 == ulLastSIM_2)
					{
						ulLastSIM_2 = ulCurTime;
					}
				}
			}
			else	// Now door is restored
			{
				if (0x01 == pstHAL->ucSIM_2_Door)	// Previous door is removed
				{
					IGET_TICKCOUNT (ulCurTime);

					// Delay a bit before set to sim slot restored to prevent franction
					if (ulCurTime - ulLastSIM_2 >= 500 && ulCurTime >= ulLastSIM_2 && 0 != ulLastSIM_2)
					{
						pstHAL->ucSIM_2_Door = 0x00;
						pstHAL->ucGotSIM_2 = 0x01;

						ulLastSIM_2 = 0;

						logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: SIM slot 2 is restored", __LINE__, HAL_UC864E_NAME);
					}
					else if (ulCurTime < ulLastSIM_2 || 0 == ulLastSIM_2)
					{
						ulLastSIM_2 = ulCurTime;
					}
				}
			}

		#endif

		// ===========================
		// ** Both SIMs are removed **
		// ===========================
		// Reset the simslot and reboot the module
		if ((0x00 == pstHAL->ucGotSIM_1 && 0x00 == pstHAL->ucGotSIM_2) || (0x01 == pstHAL->ucSIM_1_Door && 0x01 == pstHAL->ucSIM_2_Door))
    {
    	pstHAL->ucSIM_1_Door = hal_uc864e_sim_door (pstHAL, 0x01);
    	pstHAL->ucGotSIM_1 = (0x00 == pstHAL->ucSIM_1_Door) ? 0x01 : 0x00;

    	#if defined (ARCH_L200) || defined (ARCH_L350) || defined (ARCH_N300)

    		pstHAL->ucSIM_2_Door = hal_uc864e_sim_door (pstHAL, 0x02);
    		pstHAL->ucGotSIM_2 = (0x00 == pstHAL->ucSIM_2_Door) ? 0x01 : 0x00;

    	#else

    		pstHAL->ucSIM_2_Door = 0x01;
    		pstHAL->ucGotSIM_2 = 0x00;

    	#endif

    	// Default is SIM 1 if available
    	if (0x01 == pstHAL->ucGotSIM_1)
    	{
    		pstHAL->ucCurSIM = 0x01;
    	}
    	else if (0x01 == pstHAL->ucGotSIM_2)
    	{
    		pstHAL->ucCurSIM = 0x02;
    	}
    	else
    	{
    		pstHAL->ucCurSIM = 0x01;
    	}

			pstHAL->ucReset = 0x01;	// Reset the modem after sim insertion
		}

/*
		if ((0x00 == pstHAL->ucGotSIM_1 && 0x00 == pstHAL->ucGotSIM_2) || (0x01 == pstHAL->ucSIM_1_Door && 0x01 == pstHAL->ucSIM_2_Door))
		{
      if (0x00 == ucWaitSIM)
			{
				ucWaitSIM = 0x01;

				hal_uc864e_request (pstHAL, -1, HAL_UC864E_REQ_DETACH);
				logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Waiting for SIM insertion", __LINE__, HAL_UC864E_NAME);

				hal_uc864e_switch_off (pstHAL, &pstHAL->stThread.ucProcToken);
			}
		}
		else
		{
			if (0x00 != ucWaitSIM)
			{
				ucWaitSIM = 0x00;

				// Default is SIM 1 if available
				if (0x01 == pstHAL->ucGotSIM_1)
				{
					pstHAL->ucCurSIM = 0x01;
				}
				else if (0x01 == pstHAL->ucGotSIM_2)
				{
					pstHAL->ucCurSIM = 0x02;
				}

				pstHAL->ucReset = 0x01;	// Reset the modem after sim insertion
			}
		}
*/
		ISLEEP (50);
	}

	ucRet = 0x01;
	hal_uc864e_switch_off (pstHAL, &ucRet);
}
