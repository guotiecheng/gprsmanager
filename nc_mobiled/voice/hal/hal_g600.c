#include "hal_g600.h"
#include "arch.h"

unsigned char hal_g600_init (HAL_G600 *p_pstHAL)
{
	char			acDrvFile [64]	= { 0 };

	int				iPort;

	memset (p_pstHAL, 0x00, sizeof (HAL_G600));

	hal_g600_get_ctrl_port_file (p_pstHAL, acDrvFile, sizeof (acDrvFile) - 1);

	if (0 != acDrvFile [0])
	{
		comport_init_voice (&p_pstHAL->stCtrlPort, acDrvFile, 0, 0, 0, 0, 0, 0, 0, 0, 0);

		if (0x00 != comport_open_voice (&p_pstHAL->stCtrlPort))
		{
			return 0x03;
		}
	}

	for (iPort = 0; HAL_G600_MAX_DATA_PORT > iPort; iPort++)
	{
		p_pstHAL->astDataPort [iPort].ucPort = iPort + 1;
	}

	critical_init (&p_pstHAL->stLock);
	thread_init (&p_pstHAL->stThread, (void *) p_pstHAL);

	p_pstHAL->ucCurSIM = 0x01;	// Default is to use SIM1

	p_pstHAL->ucInit = 0x01;

	return 0x00;
}

void hal_g600_term (HAL_G600 *p_pstHAL)
{
	if (0x01 == p_pstHAL->ucInit)
	{
		// Kill worker process
		hal_g600_close (p_pstHAL);

		// Close all usable ports
		hal_g600_port (p_pstHAL, 0x00, 0x00, 0x00);

		// Close ctrl driver
		comport_term_voice (&p_pstHAL->stCtrlPort);

		memset (p_pstHAL, 0x00, sizeof (HAL_G600));
	}
}

unsigned char hal_g600_start (HAL_G600 *p_pstHAL)
{
	return thread_start (&p_pstHAL->stThread, hal_g600_body);
}

unsigned char hal_g600_close (HAL_G600 *p_pstHAL)
{
	return thread_close (&p_pstHAL->stThread, SIGINT, 15000);
}

unsigned char hal_g600_attach (HAL_G600 *p_pstHAL, unsigned char p_ucPort, unsigned char p_ucPriority, unsigned char p_ucInterrupt,
								  HAL_G600_NOTIFY p_pf_notify, void *p_pvPtr, int *p_piSID)
{
	unsigned char			ucRet;

	HAL_G600_DATA_PORT	*pstDataPort	= 0;

	if (HAL_G600_MAX_DATA_PORT < p_ucPort || 0 >= p_ucPort)
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
			pstDataPort->stSession.pf_notify (pstDataPort->stSession.pvPtr, HAL_G600_NOTIFY_REQ_DETACH, 0x00);

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

	ucRet = hal_g600_port (p_pstHAL, p_ucPort, 0x00, 0x01);

	if (0x00 != ucRet)
	{
		ucRet = 0x02;
		goto CleanUp;
	}

	// Caller going to hold the data port
	pstDataPort->stSession.iSID = ++(p_pstHAL->iSeqNo);
	pstDataPort->stSession.ucPriority = p_ucPriority;

	pstDataPort->stSession.pf_notify = p_pf_notify;
	pstDataPort->stSession.pvPtr = p_pvPtr;

	*p_piSID = pstDataPort->stSession.iSID;

	ucRet = 0x00;

CleanUp:
	critical_unlock (&p_pstHAL->stLock);

	return ucRet;
}

unsigned char hal_g600_detach (HAL_G600 *p_pstHAL, int p_iSID)
{
	unsigned char			ucRet;

	int						iPort;

	HAL_G600_DATA_PORT	*pstDataPort	= 0;

	if (0x00 != critical_lock (&p_pstHAL->stLock, 0x00))
	{
		return 0x01;
	}

	if (0 < p_iSID)
	{
		for (iPort = 0; HAL_G600_MAX_DATA_PORT > iPort; iPort++)
		{
			if (p_iSID == p_pstHAL->astDataPort [iPort].stSession.iSID)
			{
				pstDataPort = &p_pstHAL->astDataPort [iPort];
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
		hal_g600_port (p_pstHAL, pstDataPort->ucPort, 0x00, 0x00);

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

unsigned char hal_g600_request (HAL_G600 *p_pstHAL, int p_iSID, unsigned char p_ucOpID, ...)
{
  unsigned char         ucRet           = 0x01,
                        ucDetach        = 0x00;

  char                  acBuf [32],
                        *pcRet;

  int                   iRet,
                        iSim,
                        iFlag,

                        *piRet;

  va_list               args;

	HAL_G600_DATA_PORT *pstDataPort		= 0;

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

		for (iRet = 0; HAL_G600_MAX_DATA_PORT > iRet; iRet++)
		{
			if (p_iSID == p_pstHAL->astDataPort [iRet].stSession.iSID)
			{
				pstDataPort = &p_pstHAL->astDataPort [iRet];
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
	case HAL_G600_REQ_RESET:
		// Check all the holder priority with caller priority
		for (iRet = 0; HAL_G600_MAX_DATA_PORT > iRet; iRet++)
		{
			if (pstDataPort->stSession.iSID != p_pstHAL->astDataPort [iRet].stSession.iSID && 0 != p_pstHAL->astDataPort [iRet].stSession.iSID &&
				pstDataPort->stSession.ucPriority < p_pstHAL->astDataPort [iRet].stSession.ucPriority)
			{
				ucRet = 0x02;
				goto CleanUp;
			}
		}

		hal_g600_log (p_pstHAL, HAL_G600_LOG_RESETTING_CODE, 0x00, __LINE__);

		p_pstHAL->ucReset = 0x01;
		p_pstHAL->ucReady = 0x00;
		ucDetach = 0x01;

		ucRet = 0x00;
		break;

	case HAL_G600_REQ_DETACH:
		// Check all the holder priority with caller priority
		if (-1 != p_iSID)
		{
			for (ucRet = 0x00; HAL_G600_MAX_DATA_PORT > ucRet; ucRet++)
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

	case HAL_G600_REQ_CHG_SIM:
		// Check all the holder priority with caller priority
		for (iRet = 0; HAL_G600_MAX_DATA_PORT > iRet; iRet++)
		{
			if (pstDataPort->stSession.iSID != p_pstHAL->astDataPort [iRet].stSession.iSID && 0 != p_pstHAL->astDataPort [iRet].stSession.iSID &&
				pstDataPort->stSession.ucPriority < p_pstHAL->astDataPort [iRet].stSession.ucPriority)
			{
				ucRet = 0x02;
				goto CleanUp;
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

		if (iSim != p_pstHAL->ucCurSIM)
		{
			if (0x00 != hal_g600_sim_door (p_pstHAL, (unsigned char) iSim))
			{
				*piRet = (int) p_pstHAL->ucCurSIM;	// Forced to use current SIM
				ucRet = 0x04;
				goto CleanUp;
			}

			hal_g600_log (p_pstHAL, HAL_G600_LOG_CHANGE_SIM_CODE, 0x00, __LINE__, (int) p_pstHAL->ucCurSIM, iSim);

			p_pstHAL->ucCurSIM = (unsigned char) iSim;
			*piRet = iSim;

			p_pstHAL->ucReset = 0x01;
			p_pstHAL->ucReady = 0x00;
			ucDetach = 0x01;

			// Notify all holder switching sim
			for (iRet = 0; HAL_G600_MAX_DATA_PORT > iRet; iRet++)
			{
				if (0 != p_pstHAL->astDataPort [iRet].stSession.iSID)
				{
					p_pstHAL->astDataPort [iRet].stSession.pf_notify (p_pstHAL->astDataPort [iRet].stSession.pvPtr, HAL_G600_NOTIFY_SIM, 0x00, iSim);
				}
			}
		}

		ucRet = 0x00;
		break;

	case HAL_G600_REQ_OPEN_DATA_PORT:
		ucRet = hal_g600_port (p_pstHAL, pstDataPort->ucPort, 0x00, 0x01);
		break;

	case HAL_G600_REQ_CLOSE_DATA_PORT:
		ucRet = hal_g600_port (p_pstHAL, pstDataPort->ucPort, 0x00, 0x00);
		break;

	case HAL_G600_REQ_REG_FLAG:
		iFlag = va_arg (args, int);

		if (0 == iFlag)
		{
			p_pstHAL->ucIsReg = 0x00;
			p_pstHAL->ulLastReg = 0;
		}
		else
		{
			p_pstHAL->ucIsReg = 0x01;
		}

		break;

	case HAL_G600_REQ_GET_DATA_PORT_FILE:
		pcRet = va_arg (args, char *);

		if (!(ucRet = hal_g600_get_data_port_file (p_pstHAL, pstDataPort->ucPort, acBuf, sizeof (acBuf))))
      strcpy (pcRet, acBuf);
		break;

	case HAL_G600_REQ_SET_SIM_AVAILABILITY:
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
		for (iRet = 0; HAL_G600_MAX_DATA_PORT > iRet; iRet++)
		{
			if (0 != p_pstHAL->astDataPort [iRet].stSession.iSID)
			{
				p_pstHAL->astDataPort [iRet].stSession.pf_notify (p_pstHAL->astDataPort [iRet].stSession.pvPtr, HAL_G600_NOTIFY_FORCED_DETACH, 0x00);
			}
		}
	}

CleanUp:
	va_end (args);

	return ucRet;
}

unsigned char hal_g600_at_cmd (HAL_G600 *p_pstHAL, int p_iSID, char *p_pcReq, char *p_pcRes, char *p_pcErr, char *p_pcRet, int p_iRetSize,
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

	char					acRecv [256],
							acLog [256],
							*pcRecvPtr;

	HAL_G600_DATA_PORT	*pstDataPort		= 0;

	if (0x00 != critical_lock (&p_pstHAL->stLock, 0x00))
	{
		return 0x01;
	}

	if (-1 == p_iSID)
	{
		pstDataPort = &p_pstHAL->astDataPort [0];	// Default port for controller
	}
	else if (0 != p_iSID)
	{
		for (iRet = 0; HAL_G600_MAX_DATA_PORT > iRet; iRet++)
		{
			if (p_iSID == p_pstHAL->astDataPort [iRet].stSession.iSID)
			{
				pstDataPort = &p_pstHAL->astDataPort [iRet];
				break;
			}
		}
	}

	critical_unlock (&p_pstHAL->stLock);

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

		ucRet = comport_send_voice (&pstDataPort->stPort, (unsigned char *) p_pcReq, iCmdLen);

		if (0 != ucRet)
		{
			hal_g600_log (p_pstHAL, HAL_G600_LOG_DATA_PORT_FAILURE_CODE, ucRet, __LINE__);
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

	hal_g600_log (p_pstHAL, HAL_G600_LOG_DATA_PORT_RAW_CODE, ucRet, p_uiLine, p_pcName, acLog);

	return ucRet;
}

unsigned char hal_g600_send (HAL_G600 *p_pstHAL, int p_iSID, unsigned char *p_pucData, int p_iLen)
{
	int						iIndex;

	HAL_G600_DATA_PORT	*pstDataPort		= 0;

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
		for (iIndex = 0; HAL_G600_MAX_DATA_PORT > iIndex; iIndex++)
		{
			if (p_iSID == p_pstHAL->astDataPort [iIndex].stSession.iSID)
			{
				pstDataPort = &p_pstHAL->astDataPort [iIndex];
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

unsigned char hal_g600_recv (HAL_G600 *p_pstHAL, int p_iSID, unsigned char *p_pucData, int p_iSize, int *p_piLen, unsigned long p_ulTimeout)
{
	int						iIndex;

	HAL_G600_DATA_PORT	*pstDataPort		= 0;

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
		for (iIndex = 0; HAL_G600_MAX_DATA_PORT > iIndex; iIndex++)
		{
			if (p_iSID == p_pstHAL->astDataPort [iIndex].stSession.iSID)
			{
				pstDataPort = &p_pstHAL->astDataPort [iIndex];
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

unsigned char hal_g600_switch_on (HAL_G600 *p_pstHAL, unsigned char p_ucSim, unsigned char *p_pucProcToken)
{
	unsigned char		ucRet;

	int					iIndex		= 0;

	char				acRet [128]	= { 0 },
						acIPR [32]	= { 0 };

	p_pstHAL->ucIsReg = 0x00;
	p_pstHAL->ulLastReg = 0;

	hal_g600_select_sim (p_pstHAL, p_ucSim);
	hal_g600_log (p_pstHAL, HAL_G600_LOG_CURRENT_SIM_CODE, 0x00, __LINE__);

	// Open first ports with default settings [0]0000000
	ucRet = hal_g600_port (p_pstHAL, 0x01, 0x00, 0x01);

	if (0x00 != ucRet)
	{
		hal_g600_log (p_pstHAL, HAL_G600_LOG_DATA_PORT_FAILURE_CODE, ucRet, __LINE__);
		goto CleanUp;
	}

	if (0x01 == p_pstHAL->ucReset)
	{
		p_pstHAL->ucReset = 0x00;

		// If the module is power off or not registered to network, the RING pin will activate
		hal_g600_power_off (p_pstHAL);
		ISLEEP (15000);

		if (0x01 != *p_pucProcToken)
		{
			ucRet = 0xFF;
			goto CleanUp;
		}

		hal_g600_power_on (p_pstHAL);

		// Detect initial message if any
		ucRet = hal_g600_at_cmd (p_pstHAL, -1, 0, "ready", "ERROR", 0, 0, 0, 0, 3000, p_pucProcToken, __LINE__, HAL_G600_NAME);

		if (0x00 == ucRet)
		{
			goto CleanUp;
		}

		// Try AT command response
		ucRet = hal_g600_at_cmd (p_pstHAL, -1, "AT\r", "OK\tready", "ERROR", acRet, sizeof (acRet) - 1, 1000, 0, 2000,
			p_pucProcToken, __LINE__, HAL_G600_NAME);

		if (0x00 == ucRet)
		{
			ucRet = hal_g600_at_cmd (p_pstHAL, -1, "AT+MHUP=16,0\r", "OK\tNO CARRIER\tERROR", "", acRet, sizeof (acRet) - 1, 0, 0, 5000,
				p_pucProcToken, __LINE__, HAL_G600_NAME);

			if (0x00 == ucRet)
			{
				ucRet = 0x00;
				goto CleanUp;
			}
		}
		else if (0x05 == ucRet)
		{
			if (0 != strchr (acRet, '}'))	// Inside have ppp stack
			{
				ucRet = 0x00;
				goto CleanUp;
			}
		}
	}
	else
	{
		// Try AT command response
		ucRet = hal_g600_at_cmd (p_pstHAL, -1, "AT\r", "OK	ready", "ERROR", 0, 0, 0, 0, 2000, p_pucProcToken, __LINE__, HAL_G600_NAME);

		if (0x00 == ucRet || 0x05 == ucRet)
		{
			ucRet = 0x00;
			goto CleanUp;
		}
	}

	hal_g600_log (p_pstHAL, HAL_G600_LOG_POWERING_ON_CODE, 0x00, __LINE__);
	hal_g600_power_on (p_pstHAL);

	// Detect initial message if any
	ucRet = hal_g600_at_cmd (p_pstHAL, -1, 0, "ready", "ERROR", 0, 0, 0, 0, 3000, p_pucProcToken, __LINE__, HAL_G600_NAME);

	if (0x00 == ucRet)
	{
		goto CleanUp;
	}

	// Try AT command response
	ucRet = hal_g600_at_cmd (p_pstHAL, -1, "AT\r", "OK	ready", "ERROR", 0, 0, 0, 0, 2000, p_pucProcToken, __LINE__, HAL_G600_NAME);

	if (0x00 == ucRet || 0x05 == ucRet)
	{
		ucRet = 0x00;
		goto CleanUp;
	}


	// The factory default baudrate is 9600kbps, so need to change to 115200kbps
	hal_g600_log (p_pstHAL, HAL_G600_LOG_CHANGING_BAUDRATE_CODE, 0x00, __LINE__);

	for (iIndex = 0; 2 > iIndex && 0x01 == *p_pucProcToken; iIndex++)
	{
		hal_g600_power_off (p_pstHAL);
		ISLEEP (1000);
		hal_g600_power_on (p_pstHAL);

		// Open first port with factory settings
		ucRet = hal_g600_port (p_pstHAL, 0x01, 0x80, 0x01);

		if (0x00 != ucRet)
		{
			hal_g600_log (p_pstHAL, HAL_G600_LOG_DATA_PORT_FAILURE_CODE, ucRet, __LINE__);
			goto CleanUp;
		}

		sprintf (acIPR, "AT+IPR=%d\r", HAL_G600_DEF_BAUDRATE);

		ucRet = hal_g600_at_cmd (p_pstHAL, -1, acIPR, "OK	ready", "ERROR", 0, 0, 1000, 0, 2000, p_pucProcToken, __LINE__, HAL_G600_NAME);

		if (0x00 == ucRet || 0x05 == ucRet)
		{
			hal_g600_log (p_pstHAL, HAL_G600_LOG_CHANGED_BAUDRATE_CODE, 0x00, __LINE__);
			goto CleanUp;
		}
		else if (0xFF == ucRet)
		{
			goto CleanUp;
		}
	}

	if (0x01 != *p_pucProcToken)
	{
		ucRet = 0xFF;
		goto CleanUp;
	}

	hal_g600_log (p_pstHAL, HAL_G600_LOG_POWER_ON_FAILURE_CODE, ucRet, __LINE__);
	ucRet = 0x01;

CleanUp:
	// Close all ports
	hal_g600_port (p_pstHAL, 0x00, 0x00, 0x00);

	if (0x00 == ucRet)
	{
		hal_g600_log (p_pstHAL, HAL_G600_LOG_POWERED_ON_CODE, 0x00, __LINE__, p_ucSim);
	}

	return ucRet;
}

unsigned char hal_g600_switch_off (HAL_G600 *p_pstHAL, unsigned char *p_pucProcToken)
{
	unsigned char		ucRet,
						ucGo;

	unsigned long		ulCurTime,
						ulLastOp;

	int					iRet;

	p_pstHAL->ucIsReg = 0x00;
	p_pstHAL->ulLastReg = 0;

	hal_g600_log (p_pstHAL, HAL_G600_LOG_POWERING_OFF_CODE, 0x00, __LINE__);

	p_pstHAL->ucReady = 0x00;

	// == Notify holder to detach port ==
	for (iRet = 0; HAL_G600_MAX_DATA_PORT > iRet; iRet++)
	{
		if (0 != p_pstHAL->astDataPort [iRet].stSession.iSID)
		{
			p_pstHAL->astDataPort [iRet].stSession.pf_notify (p_pstHAL->astDataPort [iRet].stSession.pvPtr, HAL_G600_NOTIFY_FORCED_DETACH, 0x00);
		}
	}

	// == Wait every holder to detach port
	IGET_TICKCOUNT (ulCurTime);
	ulLastOp = ulCurTime;

	while (ulCurTime - ulLastOp < 5000 && ulCurTime >= ulLastOp)
	{
		ucGo = 0x01;

		for (iRet = 0; HAL_G600_MAX_DATA_PORT > iRet; iRet++)
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

	// Open first ports with default settings [0]0000000
	ucRet = hal_g600_port (p_pstHAL, 0x01, 0x00, 0x01);

	if (0x00 != ucRet)
	{
		hal_g600_log (p_pstHAL, HAL_G600_LOG_DATA_PORT_FAILURE_CODE, ucRet, __LINE__);
		goto CleanUp;
	}

	// Send AT command to power off
	ucRet = hal_g600_at_cmd (p_pstHAL, -1, "AT+MRST\r", "OK	down", "ERROR", 0, 0, 0, 0, 2000, p_pucProcToken, __LINE__, HAL_G600_NAME);

	if (0x00 != ucRet && 0x05 != ucRet)
	{
		// The module is not responding (0x05 = have echo back from module)
		hal_g600_power_off (p_pstHAL);
	}

	hal_g600_dtr (p_pstHAL, 0x00);	// DTR LOW

	hal_g600_log (p_pstHAL, HAL_G600_LOG_POWERED_OFF_CODE, 0x00, __LINE__);
	ucRet = 0x00;

CleanUp:
	// close all ports
	hal_g600_port (p_pstHAL, 0x00, 0x00, 0x00);

	return ucRet;
}

unsigned char hal_g600_get_ready (HAL_G600 *p_pstHAL)
{
	unsigned char	ucRet;

	// Open first ports with default settings [0]0000000
	ucRet = hal_g600_port (p_pstHAL, 0x01, 0x00, 0x01);

	if (0x00 != ucRet)
	{
		hal_g600_log (p_pstHAL, HAL_G600_LOG_DATA_PORT_FAILURE_CODE, ucRet, __LINE__);
		goto CleanUp;
	}


	// ================
	// == Enable DTR ==
	// ================

	hal_g600_dtr (p_pstHAL, 0x01);	// DTR ON (LOW)

	ucRet = hal_g600_at_cmd (p_pstHAL, -1, "AT&D1\r", "OK", "ERROR",
		0, 0, 0, 0, 2000, &p_pstHAL->stThread.ucProcToken, __LINE__, HAL_G600_NAME);

	if (0x00 != ucRet)
	{
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Enable DTR is not ready. RetCode [0x%02X]", __LINE__, HAL_G600_NAME, ucRet);

		ucRet = 0x02;
		goto CleanUp;
	}

CleanUp:
	// Close all ports
	hal_g600_port (p_pstHAL, 0x00, 0x00, 0x00);

	return ucRet;
}

void hal_g600_power_on (HAL_G600 *p_pstHAL)
{
	if (0 < p_pstHAL->stCtrlPort.hComPort)
	{
		#if defined (ARCH_L100) || defined (ARCH_L200) || defined (ARCH_L300) || defined (ARCH_L350) || defined (ARCH_N300)

			ioctl (p_pstHAL->stCtrlPort.hComPort, 1, 0);

		#endif
	}
}

void hal_g600_power_off (HAL_G600 *p_pstHAL)
{
	if (0 < p_pstHAL->stCtrlPort.hComPort)
	{
		#if defined (ARCH_L100) || defined (ARCH_L200) || defined (ARCH_L300) || defined (ARCH_L350) || defined (ARCH_N300)

			ioctl (p_pstHAL->stCtrlPort.hComPort, 2, 0);

		#endif
	}
}

unsigned char hal_g600_select_sim (HAL_G600 *p_pstHAL, unsigned char p_ucSim)
{
	#if defined (ARCH_L100) || defined (ARCH_L200) || defined (ARCH_L300) || defined (ARCH_L350) || defined (ARCH_N300)

		int		iSim	= 0;

		if (0 < p_pstHAL->stCtrlPort.hComPort)
		{
		    #if defined (ARCH_L300)

		        p_ucSim = 0x02; // L300 only allow to use SIM 1, because the SIM 1 for L300 is SIM 2 for L350

            #endif

			iSim = ioctl (p_pstHAL->stCtrlPort.hComPort, 5, 0);

			if (iSim == (int) p_ucSim)
			{
				return 0x00;
			}

			if (0x01 == p_ucSim)
			{
				ioctl (p_pstHAL->stCtrlPort.hComPort, 6, 1);	// Select SIM 1
				return 0x00;
			}

			#if defined (ARCH_L200) || defined (ARCH_L300) || defined (ARCH_L350) || defined (ARCH_N300)

				else if (0x02 == p_ucSim)
				{
					ioctl (p_pstHAL->stCtrlPort.hComPort, 6, 2);	// Select SIM 2
					return 0x00;
				}

			#endif
		}

	#endif

	return 0x01;
}

unsigned char hal_g600_ring (HAL_G600 *p_pstHAL)
{
	unsigned long	ulCurTime,
                ulTimeout;

	if (0 < p_pstHAL->stCtrlPort.hComPort && 0x01 == p_pstHAL->ucReady)
	{
    ulTimeout = (p_pstHAL->ucIsReg) ? 5000 : 15000;

		IGET_TICKCOUNT (ulCurTime);

		if (ulCurTime - p_pstHAL->ulLastReg >= ulTimeout && ulCurTime >= p_pstHAL->ulLastReg && 0 != p_pstHAL->ulLastReg)
		{
			#if defined (ARCH_L100) || defined (ARCH_L200)

				if (0 != ioctl (p_pstHAL->stCtrlPort.hComPort, 7, 0))
				{
					// Dont know how we need to double call this function to correctly get result
					if (0 != ioctl (p_pstHAL->stCtrlPort.hComPort, 7, 0))
					{
						return 0x00;	// RING
					}
				}

			#endif
		}
		else if (ulCurTime < p_pstHAL->ulLastReg || 0 == p_pstHAL->ulLastReg)
		{
			p_pstHAL->ulLastReg = ulCurTime;
		}
	}

	return 0x01;
}

// Return 0x01: sim door removed, else sim door closed
unsigned char hal_g600_sim_door (HAL_G600 *p_pstHAL, unsigned char p_ucSim)
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

void hal_g600_dtr (HAL_G600 *p_pstHAL, unsigned char p_ucFlag)
{
	if (0 < p_pstHAL->stCtrlPort.hComPort && 0x01 == p_pstHAL->ucReady)
	{
		if (0x01 == p_ucFlag)	// HIGH
		{
			hal_g600_log (p_pstHAL, HAL_G600_LOG_DTR_CODE, 0x01, __LINE__);

			#if defined (ARCH_L100) || defined (ARCH_L200)

				ioctl (p_pstHAL->stCtrlPort.hComPort, 8, 0);

			#elif defined (ARCH_L300) || defined (ARCH_L350) || defined (ARCH_N300)

				ioctl (p_pstHAL->stCtrlPort.hComPort, 19, 1);

			#endif
		}
		else	// LOW
		{
			hal_g600_log (p_pstHAL, HAL_G600_LOG_DTR_CODE, 0x00, __LINE__);

			#if defined (ARCH_L100) || defined (ARCH_L200)

				ioctl (p_pstHAL->stCtrlPort.hComPort, 9, 0);

			#elif defined (ARCH_L300) || defined (ARCH_L350) || defined (ARCH_N300)

				ioctl (p_pstHAL->stCtrlPort.hComPort, 19, 0);

			#endif
		}
	}
}

unsigned char hal_g600_sim (HAL_G600 *p_pstHAL, unsigned char p_ucSim)
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

unsigned char hal_g600_port (HAL_G600 *p_pstHAL, unsigned char p_ucPort, unsigned char p_ucSettings, unsigned char p_ucMode)
{
	unsigned char	ucRet				= 0x01;

	char			acDrvFile [64]		= { 0 };

	if (0x01 != p_pstHAL->ucInit)
	{
		ucRet = 0x01;
		goto CleanUp;
	}

	if (0x01 == p_ucPort || 0x00 == p_ucPort)
	{
		if (0x01 == p_ucMode)
		{
			ucRet = hal_g600_get_data_port_file (p_pstHAL, 0x01, acDrvFile, sizeof (acDrvFile));

			if (0x00 != ucRet || 0 == acDrvFile [0])
			{
				ucRet = 0x02;
				goto CleanUp;
			}

			if (0x80 & p_ucSettings)
			{
				comport_init_voice (&p_pstHAL->astDataPort [0].stPort, acDrvFile, HAL_G600_FAC_BAUDRATE, HAL_G600_FAC_DATA_BIT,
					HAL_G600_FAC_STOP_BIT, HAL_G600_FAC_PARITY, HAL_G600_FAC_FLOW_CTRL, HAL_G600_FAC_FRAG_SIZE, 0, 0, 0);
			}
			else
			{
				comport_init_voice (&p_pstHAL->astDataPort [0].stPort, acDrvFile, HAL_G600_DEF_BAUDRATE, HAL_G600_DEF_DATA_BIT,
					HAL_G600_DEF_STOP_BIT, HAL_G600_DEF_PARITY, HAL_G600_DEF_FLOW_CTRL, HAL_G600_DEF_FRAG_SIZE, 0, 0, 0);
			}

			ucRet = comport_open_voice (&p_pstHAL->astDataPort [0].stPort);
		}
		else
		{
			comport_close_voice (&p_pstHAL->astDataPort [0].stPort);
			ucRet = 0x00;
		}
	}

CleanUp:

	return ucRet;
}

unsigned char hal_g600_get_data_port_file (HAL_G600 *p_pstHAL, unsigned char p_ucPort, char *p_pcDrvFile, int p_iSize)
{
	unsigned char	ucRet		= 0x01;

	memset (p_pcDrvFile, 0x00, p_iSize);

	#if defined (ARCH_L100) || defined (ARCH_L200)

		if (0x01 == p_ucPort)
		{
			strncpy (p_pcDrvFile, "/dev/ttyS1", p_iSize - 1);
			ucRet = 0x00;
		}

	#elif defined (ARCH_L300) || defined (ARCH_L350) || defined (ARCH_N300)

		if (0x01 == p_ucPort)
		{
			strncpy (p_pcDrvFile, "/dev/ttyS2", p_iSize - 1);
			ucRet = 0x00;
		}

	#elif defined (WIN32)

		if (0x01 == p_ucPort)
		{
			strncpy (p_pcDrvFile, "COM1", p_iSize - 1);
			ucRet = 0x00;
		}

	#endif

	return ucRet;
}

void hal_g600_get_ctrl_port_file (HAL_G600 *p_pstHAL, char *p_pcDrvFile, int p_iSize)
{
	memset (p_pcDrvFile, 0x00, p_iSize);

	#if defined (ARCH_L100) || defined (ARCH_L200) || defined (ARCH_L300) || defined (ARCH_L350) || defined (ARCH_N300)

		strncpy (p_pcDrvFile, "/dev/gprs", p_iSize - 1);

	#endif
}

void hal_g600_log (HAL_G600 *p_pstHAL, int p_iLogID, unsigned char p_ucRet, ...)
{
	unsigned int	uiLine;

	int				iCurSim,
					iNewSim;

	char			*pcName,
					*pcBuf;

	va_list			args;

	va_start (args, p_ucRet);

	uiLine = va_arg (args, unsigned int);

	switch (p_iLogID)
	{
	case HAL_G600_LOG_DATA_PORT_FAILURE_CODE:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Data port failure. RetCode [0x%02X]", uiLine, HAL_G600_NAME, p_ucRet);
		break;

	case HAL_G600_LOG_CURRENT_SIM_CODE:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Module is using SIM %d", uiLine, HAL_G600_NAME, p_pstHAL->ucCurSIM);
		break;

	case HAL_G600_LOG_CHANGING_BAUDRATE_CODE:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Changing module baudrate setting", uiLine, HAL_G600_NAME);
		break;

	case HAL_G600_LOG_CHANGED_BAUDRATE_CODE:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Changed module baudrate setting", uiLine, HAL_G600_NAME);
		break;

	case HAL_G600_LOG_POWER_ON_FAILURE_CODE:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Module power on failure. RetCode [0x%02X]", uiLine, HAL_G600_NAME, p_ucRet);
		break;

	case HAL_G600_LOG_POWERING_ON_CODE:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Module powering on", uiLine, HAL_G600_NAME);
		break;

	case HAL_G600_LOG_POWERED_ON_CODE:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Module powered on", uiLine, HAL_G600_NAME);
		break;

	case HAL_G600_LOG_POWERING_OFF_CODE:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Module switching off", uiLine, HAL_G600_NAME);
		break;

	case HAL_G600_LOG_POWERED_OFF_CODE:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Module switched off", uiLine, HAL_G600_NAME);
		break;

	case HAL_G600_LOG_RESETTING_CODE:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Module resetting", uiLine, HAL_G600_NAME);
		break;

	case HAL_G600_LOG_RESET_CODE:
		logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Module reset", uiLine, HAL_G600_NAME);
		break;

	case HAL_G600_LOG_CHANGE_SIM_CODE:
		iCurSim = va_arg (args, int);
		iNewSim = va_arg (args, int);

		if (iCurSim != iNewSim)
		{
			logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Module change SIM%d -> SIM%d", uiLine, HAL_G600_NAME, iCurSim, iNewSim);
		}
		else
		{
			logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Module change SIM%d", uiLine, HAL_G600_NAME, iNewSim);
		}
		break;

	case HAL_G600_LOG_DTR_CODE:
		logger_log (&g_stVoiceLog, LOG_DETAIL, "@%04d %s: DTR is %s", uiLine, HAL_G600_NAME, (0x00 == p_ucRet) ? "LOW" : "HIGH");
		break;

	case HAL_G600_LOG_DATA_PORT_RAW_CODE:
		pcName = va_arg (args, char *);
		pcBuf = va_arg (args, char *);

		logger_log (&g_stVoiceLog, LOG_DETAIL, "@%04d %s: %s", uiLine, pcName, pcBuf);
		break;

	default:
		logger_log (&g_stVoiceLog, LOG_DETAIL, "@%04d %s: Event %d happen. RetCode [0x%02X]", uiLine, HAL_G600_NAME, p_iLogID, p_ucRet);
	}

	va_end (args);
}

void hal_g600_body (void *p_pvThread)
{
	unsigned char		ucRet,
						ucGo,
						ucWaitSIM			= 0x00;

	unsigned long		ulCurTime,
						ulLastSIM_1			= 0;

	int					iRet;

	HAL_G600			*pstHAL				= ((WRK_THREAD *) p_pvThread)->pvPtr;

	#if defined (ARCH_L200) || defined (ARCH_L350) || defined (ARCH_N300)

		unsigned long	ulLastSIM_2			= 0;

	#endif

	pstHAL->ucSIM_1_Door = hal_g600_sim_door (pstHAL, 0x01);
	pstHAL->ucGotSIM_1 = (0x00 == pstHAL->ucSIM_1_Door) ? 0x01 : 0x00;

	#if defined (ARCH_L200) || defined (ARCH_L350) || defined (ARCH_N300)

		pstHAL->ucSIM_2_Door = hal_g600_sim_door (pstHAL, 0x02);
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

	while (0x01 == pstHAL->stThread.ucProcToken)
	{
		if (0x01 != pstHAL->ucReady && (0x01 == pstHAL->ucGotSIM_1 || 0x01 == pstHAL->ucGotSIM_2))
		{
			critical_lock (&pstHAL->stLock, 0x01);

			ucGo = 0x01;

			// Make sure all holder are detached port
			for (iRet = 0; HAL_G600_MAX_DATA_PORT > iRet; iRet++)
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
				ucRet = hal_g600_switch_on (pstHAL, pstHAL->ucCurSIM, &pstHAL->stThread.ucProcToken);

				if (0x00 == ucRet)
				{
					if (0x00 == hal_g600_get_ready (pstHAL))
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
				else
				{
					pstHAL->ucStatus = SYS_STATUS_HARDWARE_FAILURE;
				}
			}
		}

		// ==================================
		// ** Detect SIM slot 1 door event **
		// ==================================

		ucRet = hal_g600_sim_door (pstHAL, 0x01);

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
						hal_g600_request (pstHAL, -1, HAL_G600_REQ_DETACH);
					}

					ulLastSIM_1 = 0;

					logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: SIM slot 1 is removed", __LINE__, HAL_G600_NAME);
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

					logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: SIM slot 1 is restored", __LINE__, HAL_G600_NAME);
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

			ucRet = hal_g600_sim_door (pstHAL, 0x02);

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
							hal_g600_request (pstHAL, -1, HAL_G600_REQ_DETACH);
						}

						ulLastSIM_2 = 0;

						logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: SIM slot 2 is removed", __LINE__, HAL_G600_NAME);
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

						logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: SIM slot 2 is restored", __LINE__, HAL_G600_NAME);
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

		if ((0x00 == pstHAL->ucGotSIM_1 && 0x00 == pstHAL->ucGotSIM_2) || (0x01 == pstHAL->ucSIM_1_Door && 0x01 == pstHAL->ucSIM_2_Door))
		{
			if (0x00 == ucWaitSIM)
			{
				ucWaitSIM = 0x01;

				hal_g600_request (pstHAL, -1, HAL_G600_REQ_DETACH);
				logger_log (&g_stVoiceLog, LOG_WARNING, "@%04d %s: Waiting for SIM insertion", __LINE__, HAL_G600_NAME);

				hal_g600_switch_off (pstHAL, &pstHAL->stThread.ucProcToken);
			}
		}
		else
		{
			if (0x01 == ucWaitSIM)
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

		ISLEEP (50);
	}

	ucRet = 0x01;
	hal_g600_switch_off (pstHAL, &ucRet);
}
