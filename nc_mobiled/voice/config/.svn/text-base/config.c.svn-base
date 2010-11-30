#include "config.h"

unsigned char xml_config_init (char *p_pcPath, CFG_CORE_ENGINE *p_pstCfg)
{
	unsigned char			ucRet;

	stXMLParserMainData		stXML;

	memset (p_pstCfg, 0x00, sizeof (CFG_CORE_ENGINE));

	InitXmlParser (&stXML, p_pcPath);

	if (0x00 != ParserXml (&stXML))
	{
		logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Config file not found in path %s or the file is not in valid format", __LINE__, MOD_CFG_NAME, p_pcPath);
		ucRet = 0x01;
		goto CleanUp;
	}

	ucRet = xml_config_logger (&stXML, p_pstCfg);

	if (0x00 != ucRet)
	{
		goto CleanUp;
	}

	ucRet = xml_config_module (&stXML, p_pstCfg);

	if (0x00 != ucRet)
	{
		goto CleanUp;
	}
#if 0
	ucRet = xml_config_crypto (&stXML, p_pstCfg);

	if (0x00 != ucRet)
	{
		goto CleanUp;
	}
#endif
	ucRet = xml_config_channel (&stXML, p_pstCfg);

	if (0x00 != ucRet)
	{
		goto CleanUp;
	}

	ucRet = xml_config_pattern (&stXML, p_pstCfg);

	if (0x00 != ucRet)
	{
		goto CleanUp;
	}

	ucRet = xml_config_depend (&stXML, p_pstCfg);

	if (0x00 != ucRet)
	{
		goto CleanUp;
	}

	ucRet = xml_config_diagnose (&stXML, p_pstCfg);

	if (0x00 != ucRet)
	{
		goto CleanUp;
	}

CleanUp:
	TerminateXmlParser (&stXML);

	return ucRet;
}

unsigned char xml_config_term (CFG_CORE_ENGINE *p_pstCfg)
{
	unsigned char	ucRet;

	ARRAY_DATA		*pstTCPItem		= 0,
					*pstRouteItem	= 0,
					*pstPatternItem = 0,
					*pstRS232Item	= 0,
					*pstRS485Item	= 0,
					*pstPSTNItem	= 0;

	CFG_TCP			*pstTCP;

	CFG_RS232		*pstRS232;

	CFG_RS485		*pstRS485;

	CFG_PSTN		*pstPSTN;

	CFG_PATTERN		*pstPattern;

	CFG_ROUTE		*pstRoute;

	// == Terminate Modules ==
	array_term (&p_pstCfg->stModules.stRS232s);
	array_term (&p_pstCfg->stModules.stRS485s);
	array_term (&p_pstCfg->stModules.stPSTNs);

	// == Terminate Channel_TCPs ==
	ucRet = array_get_first (&p_pstCfg->stChannels.stTCPs, &pstTCPItem);

	while (0x00 == ucRet && 0 != pstTCPItem)
	{
		pstTCP = (CFG_TCP *) pstTCPItem->pvData;
		array_term (&(pstTCP->stTargets));

		ucRet = array_get_next (&p_pstCfg->stChannels.stTCPs, pstTCPItem, &pstTCPItem);
	}

	array_term (&p_pstCfg->stChannels.stTCPs);

	// == Terminate Channel_RS232s ==
	ucRet = array_get_first (&p_pstCfg->stChannels.stRS232s, &pstRS232Item);

	while (0x00 == ucRet && 0 != pstRS232Item)
	{
		pstRS232 = (CFG_RS232 *) pstRS232Item->pvData;
		array_term (&(pstRS232->stTargets));

		ucRet = array_get_next (&p_pstCfg->stChannels.stRS232s, pstRS232Item, &pstRS232Item);
	}

	array_term (&p_pstCfg->stChannels.stRS232s);

	// == Terminate Channel_RS485s ==
	ucRet = array_get_first (&p_pstCfg->stChannels.stRS485s, &pstRS485Item);

	while (0x00 == ucRet && 0 != pstRS485Item)
	{
		pstRS485 = (CFG_RS485 *) pstRS485Item->pvData;
		array_term (&(pstRS485->stTargets));

		ucRet = array_get_next (&p_pstCfg->stChannels.stRS485s, pstRS485Item, &pstRS485Item);
	}

	array_term (&p_pstCfg->stChannels.stRS485s);

	// == Terminate Channel_PSTNs ==
	ucRet = array_get_first (&p_pstCfg->stChannels.stPSTNs, &pstPSTNItem);

	while (0x00 == ucRet && 0 != pstPSTNItem)
	{
		pstPSTN = (CFG_PSTN *) pstPSTNItem->pvData;

		array_term (&(pstPSTN->stDials));
		array_term (&(pstPSTN->stTargets));

		ucRet = array_get_next (&p_pstCfg->stChannels.stPSTNs, pstPSTNItem, &pstPSTNItem);
	}

	array_term (&p_pstCfg->stChannels.stPSTNs);

	// == Terminate Patterns ==
	ucRet = array_get_first (&p_pstCfg->stPatterns, &pstPatternItem);

	// Loop each pattern
	while (0x00 == ucRet && 0 != pstPatternItem)
	{
		pstPattern = (CFG_PATTERN *) pstPatternItem->pvData;

		ucRet = array_get_first (&(pstPattern->stRoutes), &pstRouteItem);

		// Loop each route
		while (0x00 == ucRet && 0 != pstRouteItem)
		{
			pstRoute = (CFG_ROUTE *) pstRouteItem->pvData;
			array_term (&(pstRoute->stDests));

			ucRet = array_get_next (&(pstPattern->stRoutes), pstRouteItem, &pstRouteItem);
		}

		array_term (&(pstPattern->stRoutes));

		ucRet = array_get_next (&p_pstCfg->stPatterns, pstPatternItem, &pstPatternItem);
	}

	array_term (&p_pstCfg->stPatterns);
	array_term (&p_pstCfg->stDepends);
	array_term (&p_pstCfg->stCryptos);

	ucRet = 0x00;

	return ucRet;
}

unsigned char xml_config_logger (stXMLParserMainData *p_pstXML, CFG_CORE_ENGINE *p_pstCfg)
{
	unsigned char	ucRet;

	char			*pcValue	= 0;

	stElement		*pstElement,
					*pstLogger;

	pstElement = p_pstXML->pstMasElement;
	pstLogger = FindChildByName (pstElement, CFG_XML_LOGGER_VOICE_NAME, 0, 0);

	if (0 != pstLogger)
	{
		if (0x00 == FindAttributeValue (pstLogger->pstAttribute, CFG_XML_LOGGER_VOICE_DEST_NAME, &pcValue))
		{
			strncpy (p_pstCfg->stLogger.acFileName, pcValue, sizeof (p_pstCfg->stLogger.acFileName) - 1);
		}
		else
		{
			logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing attribute [%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_LOGGER_VOICE_NAME, CFG_XML_LOGGER_VOICE_DEST_NAME);
			ucRet = 0x01;
			goto CleanUp;
		}

		if (0x00 == FindAttributeValue (pstLogger->pstAttribute, CFG_XML_LOGGER_VOICE_LEVEL_NAME, &pcValue))
		{
			p_pstCfg->stLogger.ucLevel = atoi (pcValue);

			// Only allow to set log all in non-production mode
			#if defined (PRODUCTION_MODE)

				if (LOG_ALL == p_pstCfg->stLogger.ucLevel)
				{
					p_pstCfg->stLogger.ucLevel = LOG_DETAIL;
				}

			#endif
		}
		else
		{
			logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing attribute [%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_LOGGER_VOICE_NAME, CFG_XML_LOGGER_VOICE_LEVEL_NAME);
			ucRet = 0x01;
			goto CleanUp;
		}

		if (0x00 == FindAttributeValue (pstLogger->pstAttribute, CFG_XML_LOGGER_VOICE_FILE_SIZE_NAME, &pcValue))
		{
			p_pstCfg->stLogger.ulFileSize = atol (pcValue);
		}
		else
		{
			logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing attribute [%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_LOGGER_VOICE_NAME, CFG_XML_LOGGER_VOICE_FILE_SIZE_NAME);
			ucRet = 0x01;
			goto CleanUp;
		}

		p_pstCfg->stLogger.ucDuplicate = CFG_COMMON_NO;

		if (0x00 == FindAttributeValue (pstLogger->pstAttribute, CFG_XML_LOGGER_VOICE_DUPLICATE_NAME, &pcValue))
		{
			if (0 == strcmp (pcValue, "Y"))
			{
				p_pstCfg->stLogger.ucDuplicate = CFG_COMMON_YES;
			}
		}
	}
	else
	{
		logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Element [%s]", __LINE__, MOD_CFG_NAME, CFG_XML_LOGGER_VOICE_NAME);
		ucRet = 0x01;
		goto CleanUp;
	}

	ucRet = 0x00;

CleanUp:

	return ucRet;
}

unsigned char xml_config_module (stXMLParserMainData *p_pstXML, CFG_CORE_ENGINE *p_pstCfg)
{
	unsigned char		ucRet				= 0x00,
						ucMobileModemID		= 0x00;

	char				*pcValue			= 0;

	int					iPPP				= 0,
						iPSTN				= 0,
						iRS232				= 0,
						iRS485				= 0,

						iPPP_Mobile_Base,
						iPPP_Mobile_Index,
						iPPP_PSTN_Base,
						iPPP_PSTN_Index;

	stElement			*pstElement,
						*pstModules,

						*pstGSM,
						*pstOPTM,

						*pstPPPs,
						*pstPPP,
						*pstPPP_PSTN,
						*pstSIM,
						*pstPing,

						*pstPSTNs,
						*pstPSTN,

						*pstRS232s,
						*pstRS232,

						*pstRS485s,
						*pstRS485;

	CFG_MOD_PPP			*pstCfgPPP			= 0,
						*pstCfgPPP_Ptr;

	CFG_MOD_PPP_PSTN	*pstCfgPPP_PSTN		= 0;

	CFG_SIM				*pstCfgSIM			= 0;

	CFG_MOD_PSTN		*pstCfgPSTN			= 0;

	CFG_MOD_RS232		*pstCfgRS232		= 0;

	CFG_MOD_RS485		*pstCfgRS485		= 0;

	ARRAY_DATA			*pstItem;

	pstElement	= p_pstXML->pstMasElement;
	pstModules	= FindChildByName (pstElement, CFG_XML_MODULE_NAME, 0, 0);
	pstPPPs		= FindChildByName (pstModules, CFG_XML_MODULE_PPPS_NAME, 0, 0);

	if (0 == pstModules)
	{
		p_pstCfg->stModules.ucBuzzer = CFG_COMMON_YES;
		ucRet = 0x00;
		goto CleanUp;
	}

	if (0x00 == FindAttributeValue (pstModules->pstAttribute, CFG_XML_MODULE_MOBILE_MODEM_ID_NAME, &pcValue))
	{
		if (0 == strcmp (CFG_MODEM_ID_GTM900B_NAME, pcValue))
		{
			ucMobileModemID = CFG_MODEM_ID_GTM900B_CODE;
		}
		else if (0 == strcmp (CFG_MODEM_ID_G600_NAME, pcValue))
		{
			ucMobileModemID = CFG_MODEM_ID_G600_CODE;
		}
		else if (0 == strcmp (CFG_MODEM_ID_UC864E_NAME, pcValue))
		{
			ucMobileModemID = CFG_MODEM_ID_UC864E_CODE;
		}
		else
		{
			logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Invalid Element [%s\\%s=%s]", __LINE__, MOD_CFG_NAME, CFG_XML_MODULE_NAME, CFG_XML_MODULE_MOBILE_MODEM_ID_NAME, pcValue);
			ucRet = 0x01;
			goto CleanUp;
		}
	}

	p_pstCfg->stModules.ucBuzzer = CFG_COMMON_YES;

	// Optional
	if (0x00 == FindAttributeValue (pstModules->pstAttribute, CFG_XML_MODULE_BUZZER_NAME, &pcValue))
	{
		if (0 == strcmp (pcValue, "N"))
		{
			p_pstCfg->stModules.ucBuzzer = CFG_COMMON_NO;
		}
	}

	// =================
	// ** GSM SECTION **
	// =================

	// Optional
	pstGSM = FindChildByName (pstModules, CFG_XML_MODULE_GSM_NAME, 0, 0);

	if (0 != pstGSM)
	{
		if (0x00 == FindAttributeValue (pstGSM->pstAttribute, CFG_XML_MODULE_GSM_ID_NAME, &pcValue))
		{
			strncpy (p_pstCfg->stModules.stGSM.acID, pcValue, sizeof (p_pstCfg->stModules.stGSM.acID) - 1);
		}
		else
		{
			logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Element [%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_MODULE_NAME, CFG_XML_MODULE_GSM_NAME, CFG_XML_MODULE_GSM_ID_NAME);
			ucRet = 0x01;
			goto CleanUp;
		}

		p_pstCfg->stModules.stGSM.ucModemID = ucMobileModemID;

		// Optional
		if (0x00 == FindAttributeValue (pstGSM->pstAttribute, CFG_XML_MODULE_GSM_MODEM_ID_NAME, &pcValue))
		{
			if (0 == strcmp (CFG_MODEM_ID_GTM900B_NAME, pcValue))
			{
				p_pstCfg->stModules.stGSM.ucModemID = CFG_MODEM_ID_GTM900B_CODE;
			}
			else if (0 == strcmp (CFG_MODEM_ID_G600_NAME, pcValue))
			{
				p_pstCfg->stModules.stGSM.ucModemID = CFG_MODEM_ID_G600_CODE;
			}
			else if (0 == strcmp (CFG_MODEM_ID_UC864E_NAME, pcValue))
			{
				p_pstCfg->stModules.stGSM.ucModemID = CFG_MODEM_ID_UC864E_CODE;
			}
			else
			{
				logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Invalid Element [%s\\%s\\%s=%s]", __LINE__, MOD_CFG_NAME,
					CFG_XML_MODULE_NAME, CFG_XML_MODULE_GSM_NAME, CFG_XML_MODULE_GSM_MODEM_ID_NAME, pcValue);
				ucRet = 0x01;
				goto CleanUp;
			}
		}
	}

	// =====================
	// ** OPTIMUS SECTION **
	// =====================

	// Optional
	pstOPTM = FindChildByName (pstModules, CFG_XML_MODULE_OPTM_NAME, 0, 0);

	if (0 != pstOPTM)
	{
		if (0x00 == FindAttributeValue (pstOPTM->pstAttribute, CFG_XML_MODULE_OPTM_ID_NAME, &pcValue))
		{
			strncpy (p_pstCfg->stModules.stOPTM.acID, pcValue, sizeof (p_pstCfg->stModules.stOPTM.acID) - 1);
		}
		else
		{
			logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Element [%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_MODULE_NAME, CFG_XML_MODULE_OPTM_NAME, CFG_XML_MODULE_OPTM_ID_NAME);
			ucRet = 0x01;
			goto CleanUp;
		}

		p_pstCfg->stModules.stOPTM.ucModemID = ucMobileModemID;

		// Optional
		if (0x00 == FindAttributeValue (pstOPTM->pstAttribute, CFG_XML_MODULE_OPTM_MODEM_ID_NAME, &pcValue))
		{
			if (0 == strcmp (CFG_MODEM_ID_GTM900B_NAME, pcValue))
			{
				p_pstCfg->stModules.stOPTM.ucModemID = CFG_MODEM_ID_GTM900B_CODE;
			}
			else if (0 == strcmp (CFG_MODEM_ID_G600_NAME, pcValue))
			{
				p_pstCfg->stModules.stOPTM.ucModemID = CFG_MODEM_ID_G600_CODE;
			}
			else if (0 == strcmp (CFG_MODEM_ID_UC864E_NAME, pcValue))
			{
				p_pstCfg->stModules.stOPTM.ucModemID = CFG_MODEM_ID_UC864E_CODE;
			}
			else
			{
				logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Invalid Element [%s\\%s\\%s=%s]", __LINE__, MOD_CFG_NAME,
					CFG_XML_MODULE_NAME, CFG_XML_MODULE_OPTM_NAME, CFG_XML_MODULE_OPTM_MODEM_ID_NAME, pcValue);
				ucRet = 0x01;
				goto CleanUp;
			}
		}

		p_pstCfg->stModules.stOPTM.ucVolume = CFG_OPTM_VOLUME_MIDDLE_CODE;

		// Optional
		if (0x00 == FindAttributeValue (pstOPTM->pstAttribute, CFG_XML_MODULE_OPTM_VOLUME_NAME, &pcValue))
		{
			if (0 == strcmp (CFG_OPTM_VOLUME_OFF_NAME, pcValue))
			{
				p_pstCfg->stModules.stOPTM.ucVolume = CFG_OPTM_VOLUME_OFF_CODE;
			}
			else if (0 == strcmp (CFG_OPTM_VOLUME_LOW_NAME, pcValue))
			{
				p_pstCfg->stModules.stOPTM.ucVolume = CFG_OPTM_VOLUME_LOW_CODE;
			}
			else if (0 == strcmp (CFG_OPTM_VOLUME_MIDDLE_NAME, pcValue))
			{
				p_pstCfg->stModules.stOPTM.ucVolume = CFG_OPTM_VOLUME_MIDDLE_CODE;
			}
			else if (0 == strcmp (CFG_OPTM_VOLUME_HIGH_NAME, pcValue))
			{
				p_pstCfg->stModules.stOPTM.ucVolume = CFG_OPTM_VOLUME_HIGH_CODE;
			}
		}
	}

	// =================
	// ** PPP SECTION **
	// =================

	iPPP = GetTotalChild (pstPPPs, CFG_XML_MODULE_PPPS_PPP_NAME);

	if (0 >= iPPP)
	{
		array_init (&p_pstCfg->stModules.stPPPs, 0, ARRAY_ORDER_DIRECT, 0x00, 0);
	}
	else
	{
		array_init (&p_pstCfg->stModules.stPPPs, iPPP, ARRAY_ORDER_DIRECT, 0x00, 0);

		pstPPP = GetNextChild (pstPPPs);

		while (0 != pstPPP)
		{
			if (0 == strcmp (pstPPP->pcName, CFG_XML_MODULE_PPPS_PPP_NAME))
			{
				MEM_ALLOC (pstCfgPPP, sizeof (CFG_MOD_PPP), (CFG_MOD_PPP *));

				if (0x00 == FindAttributeValue (pstPPP->pstAttribute, CFG_XML_MODULE_PPPS_PPP_ID, &pcValue))
				{
					strncpy (pstCfgPPP->acID, pcValue, sizeof (pstCfgPPP->acID) - 1);
				}
				else
				{
					logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME,
						CFG_XML_MODULE_NAME, CFG_XML_MODULE_PPPS_NAME, CFG_XML_MODULE_PPPS_PPP_NAME, CFG_XML_MODULE_PPPS_PPP_ID);
					ucRet = 0x01;
					goto CleanUp;
				}

				pstCfgPPP->ucModemID = ucMobileModemID;

				// Optional
				if (0x00 == FindAttributeValue (pstPPP->pstAttribute, CFG_XML_MODULE_PPPS_PPP_MODEM_ID, &pcValue))
				{
					if (0 == strcmp (CFG_MODEM_ID_GTM900B_NAME, pcValue))
					{
						pstCfgPPP->ucModemID = CFG_MODEM_ID_GTM900B_CODE;
					}
					else if (0 == strcmp (CFG_MODEM_ID_G600_NAME, pcValue))
					{
						pstCfgPPP->ucModemID = CFG_MODEM_ID_G600_CODE;
					}
					else if (0 == strcmp (CFG_MODEM_ID_UC864E_NAME, pcValue))
					{
						pstCfgPPP->ucModemID = CFG_MODEM_ID_UC864E_CODE;
					}
					else if (0 == strcmp (CFG_MODEM_ID_CX930XX_NAME, pcValue))
					{
						pstCfgPPP->ucModemID = CFG_MODEM_ID_CX930XX_CODE;
					}
					else
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Invalid Element [%s\\%s\\%s\\%s=%s]", __LINE__, MOD_CFG_NAME,
							CFG_XML_MODULE_NAME, CFG_XML_MODULE_PPPS_NAME, CFG_XML_MODULE_PPPS_PPP_NAME, CFG_XML_MODULE_PPPS_PPP_MODEM_ID, pcValue);
						ucRet = 0x01;
						goto CleanUp;
					}
				}

				pstCfgPPP->ucQuery = CFG_COMMON_YES;

				// Optional
				if (0x00 == FindAttributeValue (pstPPP->pstAttribute, CFG_XML_MODULE_PPPS_PPP_QUERY, &pcValue))
				{
					if (0 == strcmp (pcValue, "N"))
					{
						pstCfgPPP->ucQuery = CFG_COMMON_NO;
					}
				}


				// ========================
				// ** MOBILE SIM SECTION **
				// ========================

				pstSIM	= FindChildByName (pstPPP, CFG_XML_MODULE_PPPS_PPP_SIM_NAME, 0, 0);

				while (0 != pstSIM)
				{
					if (0 == strcmp (pstSIM->pcName, CFG_XML_MODULE_PPPS_PPP_SIM_NAME))
					{
						pstCfgPPP->ucType = CFG_PPP_TYPE_MOBILE;

						pstCfgSIM = (0 == pstCfgSIM) ? &pstCfgPPP->stSIM1 : &pstCfgPPP->stSIM2;

						if (0x00 == FindAttributeValue (pstSIM->pstAttribute, CFG_XML_MODULE_PPPS_PPP_SIM_APN, &pcValue))
						{
							strncpy (pstCfgSIM->acAPN, pcValue, sizeof (pstCfgSIM->acAPN) - 1);
						}
						else
						{
							logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Element [%s\\%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME,
								CFG_XML_MODULE_NAME, CFG_XML_MODULE_PPPS_NAME, CFG_XML_MODULE_PPPS_PPP_NAME, CFG_XML_MODULE_PPPS_PPP_SIM_NAME, CFG_XML_MODULE_PPPS_PPP_SIM_APN);
							ucRet = 0x01;
							goto CleanUp;
						}

						// Optional
						if (0x00 == FindAttributeValue (pstSIM->pstAttribute, CFG_XML_MODULE_PPPS_PPP_SIM_OPE, &pcValue))
						{
							strncpy (pstCfgSIM->acOPE, pcValue, sizeof (pstCfgSIM->acOPE) - 1);
						}

						// Optional
						if (0x00 == FindAttributeValue (pstSIM->pstAttribute, CFG_XML_MODULE_PPPS_PPP_SIM_UID, &pcValue))
						{
							strncpy (pstCfgSIM->acUID, pcValue, sizeof (pstCfgSIM->acUID) - 1);
						}

						// Optional
						if (0x00 == FindAttributeValue (pstSIM->pstAttribute, CFG_XML_MODULE_PPPS_PPP_SIM_PWD, &pcValue))
						{
							strncpy (pstCfgSIM->acPWD, pcValue, sizeof (pstCfgSIM->acPWD) - 1);
						}

						// Optional
						if (0x00 == FindAttributeValue (pstSIM->pstAttribute, CFG_XML_MODULE_PPPS_PPP_SIM_AUTH, &pcValue))
						{
							pstCfgSIM->ucAuth = (0 == strcmp (pcValue, "CHAP")) ? CFG_PPP_AUTH_TYPE_CHAP : CFG_PPP_AUTH_TYPE_PAP;
						}

						// Optional
						if (0x00 == FindAttributeValue (pstSIM->pstAttribute, CFG_XML_MODULE_PPPS_PPP_SIM_IP, &pcValue))
						{
							if (0 == strcmp (pcValue, "0.0.0.0"))
							{
								*pcValue = 0;
							}

							strncpy (pstCfgSIM->acIP, pcValue, sizeof (pstCfgSIM->acIP) - 1);
						}

						// Optional
						if (0x00 == FindAttributeValue (pstSIM->pstAttribute, CFG_XML_MODULE_PPPS_PPP_SIM_NETWORK_TYPE, &pcValue))
						{
							pstCfgSIM->ucNetworkType = (0 == strcmp (pcValue, "GPRS") || 0 == strcmp (pcValue, "EDGE")) ?
								CFG_PPP_NETWORK_TYPE_GPRS : CFG_PPP_NETWORK_TYPE_HSDPA;
						}
						else
						{
							// Default value for each ARCH board
							pstCfgSIM->ucNetworkType = (CFG_MODEM_ID_GTM900B_CODE == pstCfgPPP->ucModemID || CFG_MODEM_ID_G600_CODE == pstCfgPPP->ucModemID) ?
								CFG_PPP_NETWORK_TYPE_GPRS : CFG_PPP_NETWORK_TYPE_HSDPA;
						}

						pstCfgSIM->iFallbackThreshold = 3;

						// Optional
						if (0x00 == FindAttributeValue (pstSIM->pstAttribute, CFG_XML_MODULE_PPPS_PPP_SIM_FALLBACK_THRESHOLD, &pcValue))
						{
							pstCfgSIM->iFallbackThreshold = atoi (pcValue);
						}

						pstCfgSIM->ulIdleTimeout = (&pstCfgPPP->stSIM1 == pstCfgSIM) ? 0 : 86400000;	// 1 day

						// Optional (0 = permanent)
						if (0x00 == FindAttributeValue (pstSIM->pstAttribute, CFG_XML_MODULE_PPPS_PPP_SIM_IDLE_TIMEOUT, &pcValue))
						{
							if (0 <= atol (pcValue))
							{
								pstCfgSIM->ulIdleTimeout = atol (pcValue);
							}
						}

						// Optional
						pstCfgSIM->ulFallback = (&pstCfgPPP->stSIM2 == pstCfgSIM) ? 0 : 21600000;	// 6 hours

						if (0x00 == FindAttributeValue (pstSIM->pstAttribute, CFG_XML_MODULE_PPPS_PPP_SIM_FALLBACK, &pcValue))
						{
							if (0 <= atol (pcValue))
							{
								pstCfgSIM->ulFallback = atol (pcValue);
							}
						}

						// Optional
						pstPing = FindChildByName (pstSIM, CFG_XML_MODULE_PPPS_PPP_PING, 0, 0);

						if (0 != pstPing)
						{
							if (0x00 == FindAttributeValue (pstPing->pstAttribute, CFG_XML_MODULE_PPPS_PPP_PING_IP, &pcValue))
							{
								if (0 == strcmp (pcValue, "0.0.0.0"))
								{
									*pcValue = 0;
								}

								strncpy (pstCfgSIM->stPing.acIP, pcValue, sizeof (pstCfgSIM->stPing.acIP) - 1);
							}
							else
							{
								logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME,
									CFG_XML_MODULE_NAME, CFG_XML_MODULE_PPPS_NAME, CFG_XML_MODULE_PPPS_PPP_NAME, CFG_XML_MODULE_PPPS_PPP_SIM_NAME,
									CFG_XML_MODULE_PPPS_PPP_PING, CFG_XML_MODULE_PPPS_PPP_PING_IP);
								ucRet = 0x01;
								goto CleanUp;
							}

							if (0x00 == FindAttributeValue (pstPing->pstAttribute, CFG_XML_MODULE_PPPS_PPP_PING_INTERVAL, &pcValue))
							{
								pstCfgSIM->stPing.ulInterval = atol (pcValue);
							}
							else
							{
								logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME,
									CFG_XML_MODULE_NAME, CFG_XML_MODULE_PPPS_NAME, CFG_XML_MODULE_PPPS_PPP_NAME, CFG_XML_MODULE_PPPS_PPP_SIM_NAME,
									CFG_XML_MODULE_PPPS_PPP_PING, CFG_XML_MODULE_PPPS_PPP_PING_INTERVAL);
								ucRet = 0x01;
								goto CleanUp;
							}

							if (0x00 == FindAttributeValue (pstPing->pstAttribute, CFG_XML_MODULE_PPPS_PPP_PING_THRESHOLD, &pcValue))
							{
								pstCfgSIM->stPing.ucThreshold = atoi (pcValue);
							}
							else
							{
								logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME,
									CFG_XML_MODULE_NAME, CFG_XML_MODULE_PPPS_NAME, CFG_XML_MODULE_PPPS_PPP_NAME, CFG_XML_MODULE_PPPS_PPP_SIM_NAME,
									CFG_XML_MODULE_PPPS_PPP_PING, CFG_XML_MODULE_PPPS_PPP_PING_THRESHOLD);
								ucRet = 0x01;
								goto CleanUp;
							}
						}
					}

					pstSIM = GetNextPeer (pstSIM);
				}

				if (0 == pstCfgPPP->stSIM1.acAPN [0] && 0 == pstCfgPPP->stSIM2.acAPN [0])
				{
					// ==================
					// ** PSTN SECTION **
					// ==================

					pstPPP_PSTN	= FindChildByName (pstPPP, CFG_XML_MODULE_PPPS_PPP_PSTN_NAME, 0, 0);

					while (0 != pstPPP_PSTN)
					{
						if (0 == strcmp (pstPPP_PSTN->pcName, CFG_XML_MODULE_PPPS_PPP_PSTN_NAME))
						{
							pstCfgPPP->ucType = CFG_PPP_TYPE_DIALUP;

							pstCfgPPP_PSTN = (0 == pstCfgPPP_PSTN) ? &pstCfgPPP->stPSTN1 : &pstCfgPPP->stPSTN2;

							if (0x00 == FindAttributeValue (pstPPP_PSTN->pstAttribute, CFG_XML_MODULE_PPPS_PPP_PSTN_ID, &pcValue))
							{
								strncpy (pstCfgPPP_PSTN->acID, pcValue, sizeof (pstCfgPPP_PSTN->acID) - 1);
							}
							else
							{
								logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Element [%s\\%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME,
									CFG_XML_MODULE_NAME, CFG_XML_MODULE_PPPS_NAME, CFG_XML_MODULE_PPPS_PPP_NAME, CFG_XML_MODULE_PPPS_PPP_PSTN_NAME,
									CFG_XML_MODULE_PPPS_PPP_PSTN_ID);
								ucRet = 0x01;
								goto CleanUp;
							}

							if (0x00 == FindAttributeValue (pstPPP_PSTN->pstAttribute, CFG_XML_MODULE_PPPS_PPP_PSTN_DIAL, &pcValue))
							{
								strncpy (pstCfgPPP_PSTN->acDial, pcValue, sizeof (pstCfgPPP_PSTN->acDial) - 1);
							}
							else
							{
								logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Element [%s\\%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME,
									CFG_XML_MODULE_NAME, CFG_XML_MODULE_PPPS_NAME, CFG_XML_MODULE_PPPS_PPP_NAME, CFG_XML_MODULE_PPPS_PPP_PSTN_NAME,
									CFG_XML_MODULE_PPPS_PPP_PSTN_DIAL);
								ucRet = 0x01;
								goto CleanUp;
							}

							// Optional
							if (0x00 == FindAttributeValue (pstPPP_PSTN->pstAttribute, CFG_XML_MODULE_PPPS_PPP_PSTN_UID, &pcValue))
							{
								strncpy (pstCfgPPP_PSTN->acUID, pcValue, sizeof (pstCfgPPP_PSTN->acUID) - 1);
							}

							// Optional
							if (0x00 == FindAttributeValue (pstPPP_PSTN->pstAttribute, CFG_XML_MODULE_PPPS_PPP_PSTN_PWD, &pcValue))
							{
								strncpy (pstCfgPPP_PSTN->acPWD, pcValue, sizeof (pstCfgPPP_PSTN->acPWD) - 1);
							}

							// Optional
							if (0x00 == FindAttributeValue (pstPPP_PSTN->pstAttribute, CFG_XML_MODULE_PPPS_PPP_PSTN_AUTH, &pcValue))
							{
								pstCfgPPP_PSTN->ucAuth = (0 == strcmp (pcValue, "CHAP")) ? CFG_PPP_AUTH_TYPE_CHAP : CFG_PPP_AUTH_TYPE_PAP;
							}

							// Optional
							if (0x00 == FindAttributeValue (pstPPP_PSTN->pstAttribute, CFG_XML_MODULE_PPPS_PPP_PSTN_IP, &pcValue))
							{
								if (0 == strcmp (pcValue, "0.0.0.0"))
								{
									*pcValue = 0;
								}

								strncpy (pstCfgPPP_PSTN->acIP, pcValue, sizeof (pstCfgPPP_PSTN->acIP) - 1);
							}

							pstCfgPPP_PSTN->ucHandshake = CFG_PSTN_PROTOCOL_TYPE_V34;

							// Optional
							if (0x00 == FindAttributeValue (pstPPP_PSTN->pstAttribute, CFG_XML_MODULE_PPPS_PPP_PSTN_HANDSHAKE, &pcValue))
							{
								if (0 == strcmp ("V90", pcValue))
								{
									pstCfgPPP_PSTN->ucHandshake = CFG_PSTN_PROTOCOL_TYPE_V90;
								}
								else if (0 == strcmp ("V34", pcValue))
								{
									pstCfgPPP_PSTN->ucHandshake = CFG_PSTN_PROTOCOL_TYPE_V34;
								}
							}

							pstCfgPPP_PSTN->ulConnTimeout = 60000;

							// Optional
							if (0x00 == FindAttributeValue (pstPPP_PSTN->pstAttribute, CFG_XML_MODULE_PPPS_PPP_PSTN_CONN_TIMEOUT, &pcValue))
							{
								if (0 <= atol (pcValue))
								{
									pstCfgPPP_PSTN->ulConnTimeout = atol (pcValue);
								}
							}

							pstCfgPPP_PSTN->iFallbackThreshold = 3;

							// Optional
							if (0x00 == FindAttributeValue (pstPPP_PSTN->pstAttribute, CFG_XML_MODULE_PPPS_PPP_PSTN_FALLBACK_THRESHOLD, &pcValue))
							{
								pstCfgPPP_PSTN->iFallbackThreshold = atoi (pcValue);
							}

							// Optional (0 = permanent)
							if (0x00 == FindAttributeValue (pstPPP_PSTN->pstAttribute, CFG_XML_MODULE_PPPS_PPP_PSTN_IDLE_TIMEOUT, &pcValue))
							{
								if (0 <= atol (pcValue))
								{
									pstCfgPPP_PSTN->ulIdleTimeout = atol (pcValue);
								}
							}

							// Optional
							pstCfgPPP_PSTN->ulFallback = (&pstCfgPPP->stPSTN2 == pstCfgPPP_PSTN) ? 21600000 : 0;	// 6 hours

							if (0x00 == FindAttributeValue (pstPPP_PSTN->pstAttribute, CFG_XML_MODULE_PPPS_PPP_PSTN_FALLBACK, &pcValue))
							{
								if (0 <= atol (pcValue))
								{
									pstCfgPPP_PSTN->ulFallback = atol (pcValue);
								}
							}

							// Optional
							pstPing = FindChildByName (pstPPP_PSTN, CFG_XML_MODULE_PPPS_PPP_PING, 0, 0);

							if (0 != pstPing)
							{
								if (0x00 == FindAttributeValue (pstPing->pstAttribute, CFG_XML_MODULE_PPPS_PPP_PING_IP, &pcValue))
								{
									if (0 == strcmp (pcValue, "0.0.0.0"))
									{
										*pcValue = 0;
									}

									strncpy (pstCfgPPP_PSTN->stPing.acIP, pcValue, sizeof (pstCfgPPP_PSTN->stPing.acIP) - 1);
								}
								else
								{
									logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME,
										CFG_XML_MODULE_NAME, CFG_XML_MODULE_PPPS_NAME, CFG_XML_MODULE_PPPS_PPP_NAME, CFG_XML_MODULE_PPPS_PPP_SIM_NAME,
										CFG_XML_MODULE_PPPS_PPP_PING, CFG_XML_MODULE_PPPS_PPP_PING_IP);
									ucRet = 0x01;
									goto CleanUp;
								}

								if (0x00 == FindAttributeValue (pstPing->pstAttribute, CFG_XML_MODULE_PPPS_PPP_PING_INTERVAL, &pcValue))
								{
									pstCfgPPP_PSTN->stPing.ulInterval = atol (pcValue);
								}
								else
								{
									logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME,
										CFG_XML_MODULE_NAME, CFG_XML_MODULE_PPPS_NAME, CFG_XML_MODULE_PPPS_PPP_NAME, CFG_XML_MODULE_PPPS_PPP_SIM_NAME,
										CFG_XML_MODULE_PPPS_PPP_PING, CFG_XML_MODULE_PPPS_PPP_PING_INTERVAL);
									ucRet = 0x01;
									goto CleanUp;
								}

								if (0x00 == FindAttributeValue (pstPing->pstAttribute, CFG_XML_MODULE_PPPS_PPP_PING_THRESHOLD, &pcValue))
								{
									pstCfgPPP_PSTN->stPing.ucThreshold = atoi (pcValue);
								}
								else
								{
									logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME,
										CFG_XML_MODULE_NAME, CFG_XML_MODULE_PPPS_NAME, CFG_XML_MODULE_PPPS_PPP_NAME, CFG_XML_MODULE_PPPS_PPP_SIM_NAME,
										CFG_XML_MODULE_PPPS_PPP_PING, CFG_XML_MODULE_PPPS_PPP_PING_THRESHOLD);
									ucRet = 0x01;
									goto CleanUp;
								}
							}
						}

						pstPPP_PSTN = GetNextPeer (pstPPP_PSTN);
					}
				}

				if (CFG_PPP_TYPE_MOBILE != pstCfgPPP->ucType && CFG_PPP_TYPE_DIALUP != pstCfgPPP->ucType)
				{
					MEM_CLEAN (pstCfgPPP);	// Ignore this rubbish setting
				}
				else
				{
					ucRet = array_add (&p_pstCfg->stModules.stPPPs, pstCfgPPP, pstCfgPPP->acID, 0x01, 0);

					if (0x00 != ucRet)
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Add %s\\%s\\%s failure. RetCode [0x%02X]", __LINE__, MOD_CFG_NAME,
							CFG_XML_MODULE_NAME, CFG_XML_MODULE_PPPS_NAME, CFG_XML_MODULE_PPPS_PPP_NAME, ucRet);
						ucRet = 0x01;
						goto CleanUp;
					}

					pstCfgPPP = 0;
				}
			}

			pstPPP = GetNextPeer (pstPPP);
		}
	}

	// ================================
	// ** Update PPP interface index **
	// ================================

	iPPP_Mobile_Base = 10;
	iPPP_Mobile_Index = iPPP_Mobile_Base;
	iPPP_PSTN_Base = 20;
	iPPP_PSTN_Index = iPPP_PSTN_Base;

	ucRet = array_get_first (&p_pstCfg->stModules.stPPPs, &pstItem);

	while (0x00 == ucRet && 0 != pstItem)
	{
		pstCfgPPP_Ptr = (CFG_MOD_PPP *) pstItem->pvData;

		// This is mobile PPP
		if (0 != pstCfgPPP_Ptr->stSIM1.acAPN [0] || 0 != pstCfgPPP_Ptr->stSIM2.acAPN [0])
		{
			pstCfgPPP_Ptr->iIfIndex = iPPP_Mobile_Index++;
		}
		// This is PSTN PPP
		else if (0 != pstCfgPPP_Ptr->stPSTN1.acID [0] || 0 != pstCfgPPP_Ptr->stPSTN2.acID [0])
		{
			pstCfgPPP_Ptr->iIfIndex = iPPP_PSTN_Index++;
		}

		if (iPPP_Mobile_Index == iPPP_PSTN_Base)
		{
			logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Mobile PPP interface index overflow. Please reduce the number of mobile PPP less than %d", __LINE__, MOD_CFG_NAME,
				iPPP_PSTN_Base);
			ucRet = 0x01;
			goto CleanUp;
		}

		ucRet = array_get_next (&p_pstCfg->stModules.stPPPs, pstItem, &pstItem);
	}

	// Optional
	pstPSTNs = FindChildByName (pstModules, CFG_XML_MODULE_PSTNS_NAME, 0, 0);

	if (0 == pstPSTNs)
	{
		array_init (&p_pstCfg->stModules.stPSTNs, 0, ARRAY_ORDER_DIRECT, 0x00, 0);
	}
	else
	{
		iPSTN = GetTotalChild (pstPSTNs, CFG_XML_MODULE_PSTNS_PSTN_NAME);

		if (0 >= iPSTN)
		{
			array_init (&p_pstCfg->stModules.stPSTNs, 0, ARRAY_ORDER_DIRECT, 0x00, 0);
		}
		else
		{
			array_init (&p_pstCfg->stModules.stPSTNs, iPSTN, ARRAY_ORDER_DIRECT, 0x00, 0);

			pstPSTN = GetNextChild (pstPSTNs);

			while (0 != pstPSTN)
			{
				if (0 == strcmp (pstPSTN->pcName, CFG_XML_MODULE_PSTNS_PSTN_NAME))
				{
					MEM_ALLOC (pstCfgPSTN, sizeof (CFG_MOD_PSTN), (CFG_MOD_PSTN *));

					// Set default country setting to MALAYSIA
					strncpy (pstCfgPSTN->acCountryReg, "6C", sizeof (pstCfgPSTN->acCountryReg) - 1);

					if (0x00 == FindAttributeValue (pstPSTN->pstAttribute, CFG_XML_MODULE_PSTNS_PSTN_ID, &pcValue))
					{
						strncpy (pstCfgPSTN->acID, pcValue, sizeof (pstCfgPSTN->acID) - 1);
					}
					else
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_MODULE_NAME, CFG_XML_MODULE_PSTNS_NAME, CFG_XML_MODULE_PSTNS_PSTN_NAME, CFG_XML_MODULE_PSTNS_PSTN_ID);
						ucRet = 0x01;
						goto CleanUp;
					}

					if (0x00 == FindAttributeValue (pstPSTN->pstAttribute, CFG_XML_MODULE_PSTNS_PSTN_TYPE, &pcValue))
					{
						if (0 == strcmp (pcValue, "TTY"))
						{
							pstCfgPSTN->ucType = CFG_PORT_TYPE_TTY;
						}
						else if (0 == strcmp (pcValue, "SC16"))
						{
							pstCfgPSTN->ucType = CFG_PORT_TYPE_SC16;
						}
					}
					else
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_MODULE_NAME, CFG_XML_MODULE_PSTNS_NAME, CFG_XML_MODULE_PSTNS_PSTN_NAME, CFG_XML_MODULE_PSTNS_PSTN_TYPE);
						ucRet = 0x01;
						goto CleanUp;
					}

					if (0x00 == FindAttributeValue (pstPSTN->pstAttribute, CFG_XML_MODULE_PSTNS_PSTN_COUNTRY_REG, &pcValue))
					{
						strncpy (pstCfgPSTN->acCountryReg, pcValue, sizeof (pstCfgPSTN->acCountryReg) - 1);
					}

					if (0x00 == FindAttributeValue (pstPSTN->pstAttribute, CFG_XML_MODULE_PSTNS_PSTN_DATADRV, &pcValue))
					{
						strncpy (pstCfgPSTN->acDataDrv, pcValue, sizeof (pstCfgPSTN->acDataDrv) - 1);
					}
					else
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_MODULE_NAME, CFG_XML_MODULE_PSTNS_NAME, CFG_XML_MODULE_PSTNS_PSTN_NAME, CFG_XML_MODULE_PSTNS_PSTN_DATADRV);
						ucRet = 0x01;
						goto CleanUp;
					}

					if (0x00 == FindAttributeValue (pstPSTN->pstAttribute, CFG_XML_MODULE_PSTNS_PSTN_CTRLDRV, &pcValue))
					{
						strncpy (pstCfgPSTN->acCtrlDrv, pcValue, sizeof (pstCfgPSTN->acCtrlDrv) - 1);
					}
					else
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_MODULE_NAME, CFG_XML_MODULE_PSTNS_NAME, CFG_XML_MODULE_PSTNS_PSTN_NAME, CFG_XML_MODULE_PSTNS_PSTN_CTRLDRV);
						ucRet = 0x01;
						goto CleanUp;
					}

					ucRet= array_add (&p_pstCfg->stModules.stPSTNs, pstCfgPSTN, pstCfgPSTN->acID, 0x01, 0);

					if (0x00 != ucRet)
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Add %s\\%s\\%s failure. RetCode [0x%02X]", __LINE__, MOD_CFG_NAME, CFG_XML_MODULE_NAME, CFG_XML_MODULE_PSTNS_NAME, CFG_XML_MODULE_PSTNS_PSTN_NAME, ucRet);
						ucRet = 0x02;
						goto CleanUp;
					}
					else
					{
						pstCfgPSTN = 0;
					}
				}

				pstPSTN = GetNextPeer (pstPSTN);
			}
		}
	}

	// Optional
	pstRS232s = FindChildByName (pstModules, CFG_XML_MODULE_RS232S_NAME, 0, 0);

	if (0 == pstRS232s)
	{
		array_init (&p_pstCfg->stModules.stRS232s, 0, ARRAY_ORDER_DIRECT, 0x00, 0);
	}
	else
	{
		iRS232 = GetTotalChild (pstRS232s, CFG_XML_MODULE_RS232S_RS232_NAME);

		if (0 >= iRS232)
		{
			array_init (&p_pstCfg->stModules.stRS232s, 0, ARRAY_ORDER_DIRECT, 0x00, 0);
		}
		else
		{
			array_init (&p_pstCfg->stModules.stRS232s, iRS232, ARRAY_ORDER_DIRECT, 0x00, 0);

			pstRS232 = GetNextChild (pstRS232s);

			while (0 != pstRS232)
			{
				if (0 == strcmp (pstRS232->pcName, CFG_XML_MODULE_RS232S_RS232_NAME))
				{
					MEM_ALLOC (pstCfgRS232, sizeof (CFG_MOD_RS232), (CFG_MOD_RS232 *));

					if (0x00 == FindAttributeValue (pstRS232->pstAttribute, CFG_XML_MODULE_RS232S_RS232_ID, &pcValue))
					{
						strncpy (pstCfgRS232->acID, pcValue, sizeof (pstCfgRS232->acID) - 1);
					}
					else
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_MODULE_NAME, CFG_XML_MODULE_RS232S_NAME, CFG_XML_MODULE_RS232S_RS232_NAME, CFG_XML_MODULE_RS232S_RS232_ID);
						ucRet = 0x01;
						goto CleanUp;
					}

					if (0x00 == FindAttributeValue (pstRS232->pstAttribute, CFG_XML_MODULE_RS232S_RS232_TYPE, &pcValue))
					{
						if (0 == strcmp (pcValue, "TTY"))
						{
							pstCfgRS232->ucType = CFG_PORT_TYPE_TTY;
						}
						else if (0 == strcmp (pcValue, "SC16"))
						{
							pstCfgRS232->ucType = CFG_PORT_TYPE_SC16;
						}
					}
					else
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_MODULE_NAME, CFG_XML_MODULE_RS232S_NAME, CFG_XML_MODULE_RS232S_RS232_NAME, CFG_XML_MODULE_RS232S_RS232_TYPE);
						ucRet = 0x01;
						goto CleanUp;
					}

					if (0x00 == FindAttributeValue (pstRS232->pstAttribute, CFG_XML_MODULE_RS232S_RS232_DATADRV, &pcValue))
					{
						strncpy (pstCfgRS232->acDataDrv, pcValue, sizeof (pstCfgRS232->acDataDrv) - 1);
					}
					else
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_MODULE_NAME, CFG_XML_MODULE_RS232S_NAME, CFG_XML_MODULE_RS232S_RS232_NAME, CFG_XML_MODULE_RS232S_RS232_DATADRV);
						ucRet = 0x01;
						goto CleanUp;
					}

					if (0x00 == FindAttributeValue (pstRS232->pstAttribute, CFG_XML_MODULE_RS232S_RS232_BAUDRATE, &pcValue))
					{
						pstCfgRS232->ulBaudRate = atol (pcValue);
					}
					else
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_MODULE_NAME, CFG_XML_MODULE_RS232S_NAME, CFG_XML_MODULE_RS232S_RS232_NAME, CFG_XML_MODULE_RS232S_RS232_BAUDRATE);
						ucRet = 0x01;
						goto CleanUp;
					}

					if (0x00 == FindAttributeValue (pstRS232->pstAttribute, CFG_XML_MODULE_RS232S_RS232_STOPBIT, &pcValue))
					{
						pstCfgRS232->ucStopBit = atoi (pcValue);
					}
					else
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_MODULE_NAME, CFG_XML_MODULE_RS232S_NAME, CFG_XML_MODULE_RS232S_RS232_NAME, CFG_XML_MODULE_RS232S_RS232_STOPBIT);
						ucRet = 0x01;
						goto CleanUp;
					}

					if (0x00 == FindAttributeValue (pstRS232->pstAttribute, CFG_XML_MODULE_RS232S_RS232_DATABIT, &pcValue))
					{
						pstCfgRS232->ucDataBit = atoi (pcValue);
					}
					else
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_MODULE_NAME, CFG_XML_MODULE_RS232S_NAME, CFG_XML_MODULE_RS232S_RS232_NAME, CFG_XML_MODULE_RS232S_RS232_DATABIT);
						ucRet = 0x01;
						goto CleanUp;
					}

					if (0x00 == FindAttributeValue (pstRS232->pstAttribute, CFG_XML_MODULE_RS232S_RS232_PARITY, &pcValue))
					{
						pstCfgRS232->ucParity = atoi (pcValue);
					}
					else
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_MODULE_NAME, CFG_XML_MODULE_RS232S_NAME, CFG_XML_MODULE_RS232S_RS232_NAME, CFG_XML_MODULE_RS232S_RS232_PARITY);
						ucRet = 0x01;
						goto CleanUp;
					}

					if (0x00 == FindAttributeValue (pstRS232->pstAttribute, CFG_XML_MODULE_RS232S_RS232_FLOWCTRL, &pcValue))
					{
						pstCfgRS232->ucFlowCtrl = atoi (pcValue);
					}
					else
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_MODULE_NAME, CFG_XML_MODULE_RS232S_NAME, CFG_XML_MODULE_RS232S_RS232_NAME, CFG_XML_MODULE_RS232S_RS232_FLOWCTRL);
						ucRet = 0x01;
						goto CleanUp;
					}

					ucRet= array_add (&p_pstCfg->stModules.stRS232s, pstCfgRS232, pstCfgRS232->acID, 0x01, 0);

					if (0x00 != ucRet)
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Add %s\\%s\\%s failure. RetCode [0x%02X]", __LINE__, MOD_CFG_NAME, CFG_XML_MODULE_NAME, CFG_XML_MODULE_RS232S_NAME, CFG_XML_MODULE_RS232S_RS232_NAME, ucRet);
						ucRet = 0x02;
						goto CleanUp;
					}
					else
					{
						pstCfgRS232 = 0;
					}
				}

				pstRS232 = GetNextPeer (pstRS232);
			}
		}
	}

	// Optional
	pstRS485s = FindChildByName (pstModules, CFG_XML_MODULE_RS485S_NAME, 0,0);

	if (0 == pstRS485s)
	{
		array_init (&p_pstCfg->stModules.stRS485s, 0, ARRAY_ORDER_DIRECT, 0x00, 0);
	}
	else
	{
		iRS485 = GetTotalChild (pstRS485s, CFG_XML_MODULE_RS485S_RS485_NAME);

		if (0 >= iRS485)
		{
			array_init (&p_pstCfg->stModules.stRS485s, 0, ARRAY_ORDER_DIRECT, 0x00, 0);
		}
		else
		{
			array_init (&p_pstCfg->stModules.stRS485s, iRS485, ARRAY_ORDER_DIRECT, 0x00, 0);

			pstRS485 = GetNextChild (pstRS485s);

			while (0 != pstRS485)
			{
				if (0 == strcmp (pstRS485->pcName, CFG_XML_MODULE_RS485S_RS485_NAME))
				{
					MEM_ALLOC (pstCfgRS485, sizeof (CFG_MOD_RS485), (CFG_MOD_RS485 *));

					if (0x00 == FindAttributeValue (pstRS485->pstAttribute, CFG_XML_MODULE_RS485S_RS485_ID_NAME, &pcValue))
					{
						strncpy (pstCfgRS485->acID, pcValue, sizeof (pstCfgRS485->acID) - 1);
					}
					else
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_MODULE_NAME, CFG_XML_MODULE_RS485S_NAME, CFG_XML_MODULE_RS485S_RS485_NAME, CFG_XML_MODULE_RS485S_RS485_ID_NAME);
						ucRet = 0x01;
						goto CleanUp;
					}

					if (0x00 == FindAttributeValue (pstRS485->pstAttribute, CFG_XML_MODULE_RS485S_RS485_TYPE_NAME, &pcValue))
					{
						if( 0 == strcmp (pcValue, "TTY"))
						{
							pstCfgRS485->ucType = CFG_PORT_TYPE_TTY;
						}
						else if (0 == strcmp (pcValue, "SC16"))
						{
							pstCfgRS485->ucType = CFG_PORT_TYPE_SC16;
						}
					}
					else
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME,  CFG_XML_MODULE_NAME, CFG_XML_MODULE_RS485S_NAME, CFG_XML_MODULE_RS485S_RS485_NAME, CFG_XML_MODULE_RS485S_RS485_TYPE_NAME);
						ucRet = 0x01;
						goto CleanUp;
					}

					if (0x00 == FindAttributeValue (pstRS485->pstAttribute, CFG_XML_MODULE_RS485S_RS485_DATADRV_NAME, &pcValue))
					{
						strncpy (pstCfgRS485->acDataDrv, pcValue, sizeof (pstCfgRS485->acDataDrv) - 1);
					}
					else
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_MODULE_NAME, CFG_XML_MODULE_RS485S_NAME, CFG_XML_MODULE_RS485S_RS485_NAME, CFG_XML_MODULE_RS485S_RS485_DATADRV_NAME);
						ucRet = 0x01;
						goto CleanUp;
					}

					if (0x00 == FindAttributeValue (pstRS485->pstAttribute, CFG_XML_MODULE_RS485S_RS485_CTRLDRV_NAME, &pcValue))
					{
						strncpy (pstCfgRS485->acCtrlDrv, pcValue, sizeof (pstCfgRS485->acCtrlDrv) - 1);
					}
					else
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_MODULE_NAME, CFG_XML_MODULE_RS485S_NAME, CFG_XML_MODULE_RS485S_RS485_NAME, CFG_XML_MODULE_RS485S_RS485_CTRLDRV_NAME);
						ucRet = 0x01;
						goto CleanUp;
					}

					if (0x00 == FindAttributeValue (pstRS485->pstAttribute, CFG_XML_MODULE_RS485S_RS485_BAUDRATE_NAME, &pcValue))
					{
						pstCfgRS485->ulBaudRate = atol (pcValue);
					}
					else
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME,  CFG_XML_MODULE_NAME, CFG_XML_MODULE_RS485S_NAME, CFG_XML_MODULE_RS485S_RS485_NAME, CFG_XML_MODULE_RS485S_RS485_BAUDRATE_NAME);
						ucRet = 0x01;
						goto CleanUp;
					}

					if (0x00 == FindAttributeValue (pstRS485->pstAttribute, CFG_XML_MODULE_RS485S_RS485_STOPBIT_NAME, &pcValue))
					{
						pstCfgRS485->ucStopBit = atoi (pcValue);
					}
					else
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME,  CFG_XML_MODULE_NAME, CFG_XML_MODULE_RS485S_NAME, CFG_XML_MODULE_RS485S_RS485_NAME, CFG_XML_MODULE_RS485S_RS485_STOPBIT_NAME);
						ucRet = 0x01;
						goto CleanUp;
					}

					if (0x00 == FindAttributeValue (pstRS485->pstAttribute, CFG_XML_MODULE_RS485S_RS485_DATABIT_NAME, &pcValue))
					{
						pstCfgRS485->ucDataBit = atoi (pcValue);
					}
					else
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_MODULE_NAME, CFG_XML_MODULE_RS485S_NAME, CFG_XML_MODULE_RS485S_RS485_NAME, CFG_XML_MODULE_RS485S_RS485_DATABIT_NAME);
						ucRet = 0x01;
						goto CleanUp;
					}

					if (0x00 == FindAttributeValue (pstRS485->pstAttribute, CFG_XML_MODULE_RS485S_RS485_PARITY_NAME, &pcValue))
					{
						pstCfgRS485->ucParity = atoi (pcValue);
					}
					else
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_MODULE_NAME, CFG_XML_MODULE_RS485S_NAME, CFG_XML_MODULE_RS485S_RS485_NAME, CFG_XML_MODULE_RS485S_RS485_PARITY_NAME);
						ucRet = 0x01;
						goto CleanUp;
					}

					if (0x00 == FindAttributeValue (pstRS485->pstAttribute, CFG_XML_MODULE_RS485S_RS485_FLOWCTRL_NAME, &pcValue))
					{
						pstCfgRS485->ucFlowCtrl = atoi (pcValue);
					}
					else
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_MODULE_NAME, CFG_XML_MODULE_RS485S_NAME, CFG_XML_MODULE_RS485S_RS485_NAME, CFG_XML_MODULE_RS485S_RS485_FLOWCTRL_NAME);
						ucRet = 0x01;
						goto CleanUp;
					}

					ucRet= array_add (&p_pstCfg->stModules.stRS485s, pstCfgRS485, pstCfgRS485->acID, 0x01, 0);

					if (0x00 != ucRet)
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Add %s\\%s\\%s failure. RetCode [0x%02X]", __LINE__, MOD_CFG_NAME, CFG_XML_MODULE_NAME, CFG_XML_MODULE_RS485S_NAME, CFG_XML_MODULE_RS485S_RS485_NAME, ucRet);
						ucRet = 0x02;
						goto CleanUp;
					}
					else
					{
						pstCfgRS485 = 0;
					}
				}

				pstRS485 = GetNextPeer (pstRS485);
			}
		}
	}

	ucRet = 0x00;

CleanUp:
	MEM_CLEAN (pstCfgPPP);
	MEM_CLEAN (pstCfgRS232);
	MEM_CLEAN (pstCfgRS485);

	return ucRet;
}

unsigned char xml_config_channel (stXMLParserMainData *p_pstXML, CFG_CORE_ENGINE *p_pstCfg)
{
	unsigned char	ucRet			= 0x00,
					ucFlags;

	char			*pcValue		= 0;

	int				iTarget			= 0,
					iTCP			= 0,
					iPSTN			= 0,
					iPSTNNode		= 0,
					iRS232			= 0,
					iRS485			= 0;

	stElement		*pstElement,
					*pstChannels,
					*pstTargets,
					*pstTarget,
					*pstCrypto,
					*pstTCPs,
					*pstTCP,
					*pstPSTNs,
					*pstPSTN,
					*pstPSTNNodes,
					*pstPSTNNode,
					*pstRS232s,
					*pstRS232,
					*pstRS485s,
					*pstRS485;

	CFG_TCP			*pstCfgTCP		= 0;

	CFG_PSTN		*pstCfgPSTN		= 0;

	CFG_PSTN_DIAL	*pstCfgPSTNDial	= 0;

	CFG_RS232		*pstCfgRS232	= 0;

	CFG_RS485		*pstCfgRS485	= 0;

	CFG_TARGET		*pstCfgTarget	= 0;

	ARRAY_DATA		*pstItem;

	pstElement  = p_pstXML->pstMasElement;
	pstChannels = FindChildByName (pstElement, CFG_XML_CHANNEL_NAME, 0, 0);
	pstTCPs		= FindChildByName (pstChannels, CFG_XML_CHANNEL_TCPS_NAME, 0, 0);

	if (0 == pstTCPs)
	{
		array_init (&p_pstCfg->stChannels.stTCPs, 0, ARRAY_ORDER_DIRECT, 0x00, 0);
	}
	else
	{
		iTCP = GetTotalChild (pstTCPs, CFG_XML_CHANNEL_TCPS_TCP_NAME);

		if (0 >= iTCP)
		{
			array_init (&p_pstCfg->stChannels.stTCPs, 0, ARRAY_ORDER_DIRECT, 0x00, 0);
		}
		else
		{
			array_init (&p_pstCfg->stChannels.stTCPs, iTCP, ARRAY_ORDER_DIRECT, 0x00, 0);

			pstTCP = GetNextChild (pstTCPs);

			while (0 != pstTCP)
			{
				if (0 == strcmp (pstTCP->pcName, CFG_XML_CHANNEL_TCPS_TCP_NAME))
				{
					MEM_ALLOC (pstCfgTCP, sizeof (CFG_TCP), (CFG_TCP *));

					// Default value
					pstCfgTCP->iBufSize = 2048;
					pstCfgTCP->ulConnTimeout = 20000;
					pstCfgTCP->iKeepAliveInterval = 600;
					pstCfgTCP->iKeepAliveRetry = 2;
					pstCfgTCP->ulPackTimeout = 45000;

					if (0x00 == FindAttributeValue (pstTCP->pstAttribute, CFG_XML_CHANNEL_TCPS_TCP_ID, &pcValue))
					{
						strncpy (pstCfgTCP->acID, pcValue, sizeof (pstCfgTCP->acID) - 1);
					}
					else
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_CHANNEL_NAME, CFG_XML_CHANNEL_TCPS_NAME, CFG_XML_CHANNEL_TCPS_TCP_NAME, CFG_XML_CHANNEL_TCPS_TCP_ID);
						ucRet = 0x01;
						goto CleanUp;
					}

					if (0x00 == FindAttributeValue (pstTCP->pstAttribute, CFG_XML_CHANNEL_TCPS_TCP_BUFSIZE, &pcValue))
					{
						pstCfgTCP->iBufSize = atoi(pcValue);
					}

					if (0x00 == FindAttributeValue (pstTCP->pstAttribute, CFG_XML_CHANNEL_TCPS_TCP_CONN_MODE_NAME, &pcValue))
					{
						if (0 == strcmp (pcValue, "C"))
						{
							pstCfgTCP->ucConnMode = CFG_CONN_MODE_CONNECT;
						}
						else if (0 == strcmp (pcValue, "L"))
						{
							pstCfgTCP->ucConnMode = CFG_CONN_MODE_LISTEN;
						}
						else
						{
							logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_CHANNEL_NAME, CFG_XML_CHANNEL_TCPS_NAME, CFG_XML_CHANNEL_TCPS_TCP_NAME, CFG_XML_CHANNEL_TCPS_TCP_CONN_MODE_NAME);
							ucRet = 0x01;
							goto CleanUp;
						}
					}
					else
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_CHANNEL_NAME, CFG_XML_CHANNEL_TCPS_NAME, CFG_XML_CHANNEL_TCPS_TCP_NAME, CFG_XML_CHANNEL_TCPS_TCP_CONN_MODE_NAME);
						ucRet = 0x01;
						goto CleanUp;
					}

					if (0x00 == FindAttributeValue (pstTCP->pstAttribute, CFG_XML_CHANNEL_TCPS_TCP_CONN_TYPE_NAME, &pcValue))
					{
						if (0 == strcmp (pcValue, "P"))
						{
							pstCfgTCP->ucConnType = CFG_CONN_TYPE_PERMANENT;
						}
						else if (0 == strcmp (pcValue, "O"))
						{
							pstCfgTCP->ucConnType = CFG_CONN_TYPE_ONDEMAND;
						}
						else if (0 == strcmp (pcValue, "O_RR"))
						{
							pstCfgTCP->ucConnType = CFG_CONN_TYPE_ONDEMAND_RR;
						}
						else
						{
							logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_CHANNEL_NAME, CFG_XML_CHANNEL_TCPS_NAME, CFG_XML_CHANNEL_TCPS_TCP_NAME, CFG_XML_CHANNEL_TCPS_TCP_CONN_TYPE_NAME);
							ucRet = 0x01;
							goto CleanUp;
						}
					}
					else
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_CHANNEL_NAME, CFG_XML_CHANNEL_TCPS_NAME, CFG_XML_CHANNEL_TCPS_TCP_NAME, CFG_XML_CHANNEL_TCPS_TCP_CONN_TYPE_NAME);
						ucRet = 0x01;
						goto CleanUp;
					}

					if (0x00 == FindAttributeValue (pstTCP->pstAttribute, CFG_XML_CHANNEL_TCPS_TCP_IP, &pcValue))
					{
						strncpy (pstCfgTCP->acIP, pcValue, sizeof (pstCfgTCP->acIP) - 1);
					}
					else
					{
						if (CFG_CONN_MODE_CONNECT == pstCfgTCP->ucConnMode)
						{
							logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_CHANNEL_NAME, CFG_XML_CHANNEL_TCPS_NAME, CFG_XML_CHANNEL_TCPS_TCP_NAME, CFG_XML_CHANNEL_TCPS_TCP_IP);
							ucRet = 0x01;
							goto CleanUp;
						}
					}

					if (0x00 == FindAttributeValue (pstTCP->pstAttribute, CFG_XML_CHANNEL_TCPS_TCP_LOCPORT, &pcValue))
					{
						pstCfgTCP->iLocPort = atoi (pcValue);
					}
					else
					{
						if (CFG_CONN_MODE_LISTEN == pstCfgTCP->ucConnMode)
						{
							logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_CHANNEL_NAME, CFG_XML_CHANNEL_TCPS_NAME, CFG_XML_CHANNEL_TCPS_TCP_NAME, CFG_XML_CHANNEL_TCPS_TCP_LOCPORT);
							ucRet = 0x01;
							goto CleanUp;
						}
					}

					if (0x00 == FindAttributeValue (pstTCP->pstAttribute, CFG_XML_CHANNEL_TCPS_TCP_REMPORT, &pcValue))
					{
						pstCfgTCP->iRemPort = atoi (pcValue);
					}
					else
					{
						if (CFG_CONN_MODE_CONNECT == pstCfgTCP->ucConnMode)
						{
							logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_CHANNEL_NAME, CFG_XML_CHANNEL_TCPS_NAME, CFG_XML_CHANNEL_TCPS_TCP_NAME, CFG_XML_CHANNEL_TCPS_TCP_REMPORT);
							ucRet = 0x01;
							goto CleanUp;
						}
					}

					if (0x00 == FindAttributeValue (pstTCP->pstAttribute, CFG_XML_CHANNEL_TCPS_TCP_CONN_TIMEOUT_NAME, &pcValue))
					{
						if (0 < atol (pcValue))
						{
							pstCfgTCP->ulConnTimeout = atol (pcValue);
						}
					}

					if (0x00 == FindAttributeValue (pstTCP->pstAttribute, CFG_XML_CHANNEL_TCPS_TCP_IDLE_TIMEOUT_NAME, &pcValue))
					{
						pstCfgTCP->ulIdleTimeout = atol (pcValue);
					}

					if (0x00 == FindAttributeValue (pstTCP->pstAttribute, CFG_XML_CHANNEL_TCPS_TCP_WRK_MIN_NAME, &pcValue))
					{
						pstCfgTCP->iMinWrk = atoi (pcValue);
					}
					else
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_CHANNEL_NAME, CFG_XML_CHANNEL_TCPS_NAME, CFG_XML_CHANNEL_TCPS_TCP_NAME, CFG_XML_CHANNEL_TCPS_TCP_WRK_MIN_NAME);
						ucRet = 0x01;
						goto CleanUp;
					}

					if (0x00 == FindAttributeValue (pstTCP->pstAttribute, CFG_XML_CHANNEL_TCPS_TCP_WRK_MAX_NAME, &pcValue))
					{
						pstCfgTCP->iMaxWrk = atoi (pcValue);
					}
					else
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_CHANNEL_NAME, CFG_XML_CHANNEL_TCPS_NAME, CFG_XML_CHANNEL_TCPS_TCP_NAME, CFG_XML_CHANNEL_TCPS_TCP_WRK_MAX_NAME);
						ucRet = 0x01;
						goto CleanUp;
					}

					if (0x00 == FindAttributeValue (pstTCP->pstAttribute, CFG_XML_CHANNEL_TCPS_TCP_WRK_KILL_IDLE_NAME, &pcValue))
					{
						pstCfgTCP->ulKillIdleWrk = atol (pcValue);
					}
					else
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_CHANNEL_NAME, CFG_XML_CHANNEL_TCPS_NAME, CFG_XML_CHANNEL_TCPS_TCP_NAME, CFG_XML_CHANNEL_TCPS_TCP_WRK_KILL_IDLE_NAME);
						goto CleanUp;
					}

					if (0x00 == FindAttributeValue (pstTCP->pstAttribute, CFG_XML_CHANNEL_TCPS_TCP_KEEP_ALIVE_INTERVAL_NAME, &pcValue))
					{
						pstCfgTCP->iKeepAliveInterval = atoi (pcValue);
					}

					if (0x00 == FindAttributeValue (pstTCP->pstAttribute, CFG_XML_CHANNEL_TCPS_TCP_KEEP_ALIVE_RETRY_NAME, &pcValue))
					{
						pstCfgTCP->iKeepAliveRetry = atoi (pcValue);
					}

					if (0x00 == FindAttributeValue (pstTCP->pstAttribute, CFG_XML_CHANNEL_TCPS_TCP_PCK_TIMEOUT_NAME, &pcValue))
					{
						pstCfgTCP->ulPackTimeout = atol (pcValue);
					}

					ucFlags = 0x00;

					if (0x00 == FindAttributeValue (pstTCP->pstAttribute, CFG_XML_CHANNEL_TCPS_TCP_DROP_LINE_FLAG_NAME, &pcValue))
					{
						if (0 == strcmp (pcValue, "Y"))
						{
							//10000000
							ucFlags = (ucFlags | CFG_CONN_FLAG_DROP_LINE);
						}
						else if (0 == strcmp (pcValue, "N"))
						{
							//01111111
							ucFlags = (ucFlags & !CFG_CONN_FLAG_DROP_LINE);
						}
						else
						{
							logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_CHANNEL_NAME, CFG_XML_CHANNEL_TCPS_NAME, CFG_XML_CHANNEL_TCPS_TCP_NAME, CFG_XML_CHANNEL_TCPS_TCP_DROP_LINE_FLAG_NAME);
							ucRet = 0x01;
							goto CleanUp;
						}
					}
					else
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_CHANNEL_NAME, CFG_XML_CHANNEL_TCPS_NAME, CFG_XML_CHANNEL_TCPS_TCP_NAME, CFG_XML_CHANNEL_TCPS_TCP_DROP_LINE_FLAG_NAME);
						ucRet = 0x01;
						goto CleanUp;
					}

					if (0x00 == FindAttributeValue (pstTCP->pstAttribute, CFG_XML_CHANNEL_TCPS_TCP_TUNNEL_FLAG_NAME, &pcValue))
					{
						if (0 == strcmp (pcValue, "Y"))
						{
							//01000000
							ucFlags = (ucFlags | CFG_CONN_FLAG_TUNNEL);
						}
						else if (0 == strcmp (pcValue, "N"))
						{
							//10111111
							ucFlags = (ucFlags & !CFG_CONN_FLAG_TUNNEL);
						}
						else
						{
							logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_CHANNEL_NAME, CFG_XML_CHANNEL_TCPS_NAME, CFG_XML_CHANNEL_TCPS_TCP_NAME, CFG_XML_CHANNEL_TCPS_TCP_TUNNEL_FLAG_NAME);
							ucRet = 0x01;
							goto CleanUp;
						}
					}
					else
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_CHANNEL_NAME, CFG_XML_CHANNEL_TCPS_NAME, CFG_XML_CHANNEL_TCPS_TCP_NAME, CFG_XML_CHANNEL_TCPS_TCP_TUNNEL_FLAG_NAME);
						ucRet = 0x01;
						goto CleanUp;
					}

					pstCfgTCP->ucFlags = ucFlags;

					pstCrypto = FindChildByName (pstTCP, CFG_XML_CHANNEL_TCPS_TCP_CRYPTO_NAME, 0, 0);

					if (0 != pstCrypto)
					{
						if (0x00 == FindAttributeValue (pstCrypto->pstAttribute, CFG_XML_CHANNEL_TCPS_TCP_CRYPTO_ID_NAME, &pcValue))
						{
							if (0 != *pcValue)
							{
								ucRet = array_get_by_id (&p_pstCfg->stCryptos, pcValue, &pstItem);

								if (0x00 != ucRet || 0 == pstItem)
								{
									logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME,
										CFG_XML_CHANNEL_NAME, CFG_XML_CHANNEL_TCPS_NAME, CFG_XML_CHANNEL_TCPS_TCP_NAME, CFG_XML_CHANNEL_TCPS_TCP_CRYPTO_NAME, CFG_XML_CHANNEL_TCPS_TCP_CRYPTO_ID_NAME);
									ucRet = 0x01;
									goto CleanUp;
								}
								else
								{
									pstCfgTCP->pstCrypto = (CFG_CRYPTO *) pstItem->pvData;
								}
							}
						}
					}

					pstTargets = FindChildByName(pstTCP, CFG_XML_CHANNEL_TCPS_TCP_TARGETS_NAME, 0, 0);
					pstTarget = GetNextChild (pstTargets);

					iTarget = GetTotalChild (pstTargets, CFG_XML_CHANNEL_TCPS_TCP_TARGETS_TARGET_NAME);

					if (0 < iTarget)
					{
						array_init (&pstCfgTCP->stTargets, iTarget, ARRAY_ORDER_DIRECT, 0x00, 0);

						while (0 != pstTarget)
						{
							if (0 == strcmp (pstTarget->pcName, CFG_XML_CHANNEL_TCPS_TCP_TARGETS_TARGET_NAME))
							{
								MEM_ALLOC (pstCfgTarget, sizeof (CFG_TARGET), (CFG_TARGET *));

								if (0x00 == FindAttributeValue (pstTarget->pstAttribute,CFG_XML_CHANNEL_TCPS_TCP_TARGETS_TARGET_PATTERN_NAME, &pcValue))
								{
									strncpy (pstCfgTarget->acPattern, pcValue, sizeof (pstCfgTarget->acPattern) - 1);
								}
								else
								{
									logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_CHANNEL_NAME, CFG_XML_CHANNEL_TCPS_NAME, CFG_XML_CHANNEL_TCPS_TCP_NAME, CFG_XML_CHANNEL_TCPS_TCP_TARGETS_NAME, CFG_XML_CHANNEL_TCPS_TCP_TARGETS_TARGET_NAME, CFG_XML_CHANNEL_TCPS_TCP_TARGETS_TARGET_PATTERN_NAME);
									ucRet = 0x01;
									goto CleanUp;
								}

								ucRet = array_add (&pstCfgTCP->stTargets, pstCfgTarget, 0, 0x01, 0);

								if (0x00 != ucRet)
								{
									logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Array List Attribute[%s\\%s\\%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_CHANNEL_NAME, CFG_XML_CHANNEL_TCPS_NAME, CFG_XML_CHANNEL_TCPS_TCP_NAME, CFG_XML_CHANNEL_TCPS_TCP_TARGETS_NAME, CFG_XML_CHANNEL_TCPS_TCP_TARGETS_TARGET_NAME, CFG_XML_CHANNEL_TCPS_TCP_TARGETS_TARGET_NAME);
									ucRet = 0x01;
									goto CleanUp;
								}
								else
								{
									pstCfgTarget = 0;
								}
							}

							pstTarget = GetNextPeer (pstTarget);
						}
					}
					else
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Element [%s\\%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_CHANNEL_NAME, CFG_XML_CHANNEL_TCPS_NAME, CFG_XML_CHANNEL_TCPS_TCP_NAME, CFG_XML_CHANNEL_TCPS_TCP_TARGETS_NAME, CFG_XML_CHANNEL_TCPS_TCP_TARGETS_TARGET_NAME);
						goto CleanUp;
					}

					ucRet = array_add (&p_pstCfg->stChannels.stTCPs, pstCfgTCP, pstCfgTCP->acID, 0x01, 0);

					if (0x00 != ucRet)
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s\\%s\\%s: Add %s failure. RetCode [0x%02X]", __LINE__, MOD_CFG_NAME, CFG_XML_CHANNEL_NAME, CFG_XML_CHANNEL_TCPS_NAME, CFG_XML_CHANNEL_TCPS_TCP_NAME, ucRet);
						ucRet = 0x02;
						goto CleanUp;
					}
					else
					{
						pstCfgTCP = 0;
					}
				}

				pstTCP = GetNextPeer (pstTCP);
			}
		}
	}

	pstPSTNs = FindChildByName (pstChannels, CFG_XML_CHANNEL_PSTNS_NAME, 0, 0);

	if (0 == pstPSTNs)
	{
		array_init (&p_pstCfg->stChannels.stPSTNs, 0, ARRAY_ORDER_DIRECT, 0x00, 0);
	}
	else
	{
		iPSTN = GetTotalChild (pstPSTNs, CFG_XML_CHANNEL_PSTNS_PSTN_NAME);

		if (0 >= iPSTN)
		{
			array_init (&p_pstCfg->stChannels.stPSTNs, 0, ARRAY_ORDER_DIRECT, 0x00, 0);
		}
		else
		{
			array_init (&p_pstCfg->stChannels.stPSTNs, iPSTN, ARRAY_ORDER_DIRECT, 0x00, 0);

			pstPSTN = GetNextChild (pstPSTNs);

			while (0 != pstPSTN)
			{
				if (0 == strcmp(pstPSTN->pcName, CFG_XML_CHANNEL_PSTNS_PSTN_NAME))
				{
					MEM_ALLOC (pstCfgPSTN, sizeof (CFG_PSTN), (CFG_PSTN *));

					// Default value
					pstCfgPSTN->iBufSize = 2048;
					pstCfgPSTN->ulPackTimeout = 45000;
					pstCfgPSTN->ulConnTimeout = 12000;
					pstCfgPSTN->ulIdleTimeout = 10000;

					if (0x00 == FindAttributeValue (pstPSTN->pstAttribute, CFG_XML_MODULE_PSTNS_PSTN_ID, &pcValue))
					{
						strncpy (pstCfgPSTN->acID, pcValue, sizeof (pstCfgPSTN->acID) - 1);
					}
					else
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_CHANNEL_NAME, CFG_XML_CHANNEL_PSTNS_NAME, CFG_XML_CHANNEL_PSTNS_PSTN_NAME, CFG_XML_CHANNEL_PSTNS_PSTN_ID);
						ucRet = 0x01;
						goto CleanUp;
					}

					if (0x00 == FindAttributeValue (pstPSTN->pstAttribute, CFG_XML_CHANNEL_PSTNS_PSTN_MODEM_NAME, &pcValue))
					{
						strncpy (pstCfgPSTN->acModem, pcValue, sizeof (pstCfgPSTN->acModem) - 1);
					}
					else
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_CHANNEL_NAME, CFG_XML_CHANNEL_PSTNS_NAME, CFG_XML_CHANNEL_PSTNS_PSTN_NAME, CFG_XML_CHANNEL_PSTNS_PSTN_MODEM_NAME);
						ucRet = 0x01;
						goto CleanUp;
					}

					if (0x00 == FindAttributeValue (pstPSTN->pstAttribute, CFG_XML_CHANNEL_PSTNS_PSTN_PROTOCOL_NAME, &pcValue))
					{
						if (0 == strcmp (pcValue, "FC_V22"))
						{
							pstCfgPSTN->ucProtocol = CFG_PSTN_PROTOCOL_TYPE_FC_V22;
						}
						else if (0 == strcmp (pcValue, "FC_V22B"))
						{
							pstCfgPSTN->ucProtocol = CFG_PSTN_PROTOCOL_TYPE_FC_V22B;
						}
						else if (0 == strcmp (pcValue, "FC_V22B_2400"))
						{
							pstCfgPSTN->ucProtocol = CFG_PSTN_PROTOCOL_TYPE_FC_V22B_2400;
						}
						else if (0 == strcmp (pcValue, "V34"))
						{
							pstCfgPSTN->ucProtocol = CFG_PSTN_PROTOCOL_TYPE_V34;
						}
						else if (0 == strcmp (pcValue, "V90"))
						{
							pstCfgPSTN->ucProtocol = CFG_PSTN_PROTOCOL_TYPE_V90;
						}
						else
						{
							logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_CHANNEL_NAME, CFG_XML_CHANNEL_PSTNS_NAME, CFG_XML_CHANNEL_PSTNS_PSTN_NAME, CFG_XML_CHANNEL_PSTNS_PSTN_PROTOCOL_NAME);
							ucRet = 0x01;
							goto CleanUp;
						}
					}
					else
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_CHANNEL_NAME, CFG_XML_CHANNEL_PSTNS_NAME, CFG_XML_CHANNEL_PSTNS_PSTN_NAME, CFG_XML_CHANNEL_PSTNS_PSTN_PROTOCOL_NAME);
						ucRet = 0x01;
						goto CleanUp;
					}

					if (0x00 == FindAttributeValue (pstPSTN->pstAttribute, CFG_XML_CHANNEL_PSTNS_PSTN_CONN_TYPE_NAME, &pcValue))
					{
						if (0 == strcmp (pcValue, "P"))
						{
							pstCfgPSTN->ucConnType = CFG_CONN_TYPE_PERMANENT;
						}
						else if (0 == strcmp (pcValue, "O"))
						{
							pstCfgPSTN->ucConnType = CFG_CONN_TYPE_ONDEMAND;
						}
						else if (0 == strcmp (pcValue, "O_RR"))
						{
							pstCfgPSTN->ucConnType = CFG_CONN_TYPE_ONDEMAND_RR;
						}
						else
						{
							logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_CHANNEL_NAME, CFG_XML_CHANNEL_PSTNS_NAME, CFG_XML_CHANNEL_PSTNS_PSTN_NAME, CFG_XML_CHANNEL_PSTNS_PSTN_CONN_TYPE_NAME);
							ucRet = 0x01;
							goto CleanUp;
						}
					}
					else
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_CHANNEL_NAME, CFG_XML_CHANNEL_PSTNS_NAME, CFG_XML_CHANNEL_PSTNS_PSTN_NAME, CFG_XML_CHANNEL_PSTNS_PSTN_CONN_TYPE_NAME);
						ucRet = 0x01;
						goto CleanUp;
					}

					if (0x00 == FindAttributeValue (pstPSTN->pstAttribute, CFG_XML_CHANNEL_PSTNS_PSTN_CONN_TIMEOUT_NAME, &pcValue))
					{
						pstCfgPSTN->ulConnTimeout = atol (pcValue);
					}

					if (0x00 == FindAttributeValue (pstPSTN->pstAttribute, CFG_XML_CHANNEL_PSTNS_PSTN_IDLE_TIMEOUT_NAME, &pcValue))
					{
						pstCfgPSTN->ulIdleTimeout = atol (pcValue);
					}

					if (0x00 == FindAttributeValue (pstPSTN->pstAttribute, CFG_XML_CHANNEL_PSTNS_PSTN_BUFSIZE_NAME, &pcValue))
					{
						pstCfgPSTN->iBufSize = atoi (pcValue);
					}

					if (0x00 == FindAttributeValue (pstPSTN->pstAttribute, CFG_XML_CHANNEL_PSTNS_PSTN_PCK_TIMEOUT_NAME, &pcValue))
					{
						pstCfgPSTN->ulPackTimeout = atol (pcValue);
					}

					if (0x00 == FindAttributeValue (pstPSTN->pstAttribute, CFG_XML_CHANNEL_PSTNS_PSTN_SNRM_TIMEOUT_NAME, &pcValue))
					{
						pstCfgPSTN->ulSNRM_Timeout = atol (pcValue);
					}

					if (0x00 == FindAttributeValue (pstPSTN->pstAttribute, CFG_XML_CHANNEL_PSTNS_PSTN_RR_TIMEOUT_NAME, &pcValue))
					{
						pstCfgPSTN->ulRR_Timeout = atol (pcValue);
					}

					if (0x00 == FindAttributeValue (pstPSTN->pstAttribute, CFG_XML_CHANNEL_PSTNS_PSTN_ATTENUATION_NAME, &pcValue))
					{
						pstCfgPSTN->iAttenuation = atoi (pcValue);
					}

					ucFlags = 0x00;

					if (0x00 == FindAttributeValue (pstPSTN->pstAttribute, CFG_XML_CHANNEL_PSTNS_PSTN_TUNNEL_FLAG_NAME, &pcValue))
					{
						if (0 == strcmp (pcValue, "Y"))
						{
							//01000000
							ucFlags = (ucFlags | CFG_CONN_FLAG_TUNNEL);
						}
						else if (0 == strcmp (pcValue, "N"))
						{
							//10111111
							ucFlags = (ucFlags & !CFG_CONN_FLAG_TUNNEL);
						}
						else
						{
							logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_CHANNEL_NAME, CFG_XML_CHANNEL_PSTNS_NAME, CFG_XML_CHANNEL_PSTNS_PSTN_NAME, CFG_XML_CHANNEL_PSTNS_PSTN_TUNNEL_FLAG_NAME);
							ucRet = 0x01;
							goto CleanUp;
						}
					}

					if (0x00 == FindAttributeValue (pstPSTN->pstAttribute, CFG_XML_CHANNEL_PSTNS_PSTN_DROP_LINE_FLAG_NAME, &pcValue))
					{
						if (0 == strcmp (pcValue, "Y"))
						{
							//10000000
							ucFlags = (ucFlags | CFG_CONN_FLAG_DROP_LINE);
						}
						else if (0 == strcmp (pcValue, "N"))
						{
							//01111111
							ucFlags = (ucFlags & !CFG_CONN_FLAG_DROP_LINE);
						}
						else
						{
							logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_CHANNEL_NAME, CFG_XML_CHANNEL_PSTNS_NAME, CFG_XML_CHANNEL_PSTNS_PSTN_NAME, CFG_XML_CHANNEL_PSTNS_PSTN_DROP_LINE_FLAG_NAME);
							ucRet = 0x01;
							goto CleanUp;
						}
					}
					else
					{
						//10000000
						ucFlags = (ucFlags | CFG_CONN_FLAG_DROP_LINE);
					}

					pstCfgPSTN->ucFlags = ucFlags;

					pstPSTNNodes = FindChildByName (pstPSTN, CFG_XML_CHANNEL_PSTNS_PSTN_NODES_NAME, 0, 0);

					if (0 != pstPSTNNodes)
					{
						iPSTNNode = GetTotalChild (pstPSTNNodes, CFG_XML_CHANNEL_PSTNS_PSTN_NODE_NAME);
					}
					else
					{
						iPSTNNode = 0;
					}

					if (0 >= iPSTNNode)
					{
						array_init (&pstCfgPSTN->stDials, 0, ARRAY_ORDER_DIRECT, 0x00, 0);
					}
					else
					{
						array_init (&pstCfgPSTN->stDials, iPSTNNode, ARRAY_ORDER_DIRECT, 0x00, 0);

						pstPSTNNode = GetNextChild (pstPSTNNodes);

						while (0 != pstPSTNNode)
						{
							if (0 == strcmp (pstPSTNNode->pcName, CFG_XML_CHANNEL_PSTNS_PSTN_NODE_NAME))
							{
								if (0x00 != FindAttributeValue (pstPSTNNode->pstAttribute, CFG_XML_CHANNEL_PSTNS_PSTN_NODE_DIAL_NAME, &pcValue))
								{
									logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME,
										CFG_XML_CHANNEL_NAME, CFG_XML_CHANNEL_PSTNS_NAME, CFG_XML_CHANNEL_PSTNS_PSTN_NAME, CFG_XML_CHANNEL_PSTNS_PSTN_NODES_NAME, CFG_XML_CHANNEL_PSTNS_PSTN_NODE_NAME, CFG_XML_CHANNEL_PSTNS_PSTN_NODE_DIAL_NAME);
									ucRet = 0x01;
									goto CleanUp;
								}

								if (0 == pcValue)
								{
									logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME,
										CFG_XML_CHANNEL_NAME, CFG_XML_CHANNEL_PSTNS_NAME, CFG_XML_CHANNEL_PSTNS_PSTN_NAME, CFG_XML_CHANNEL_PSTNS_PSTN_NODES_NAME, CFG_XML_CHANNEL_PSTNS_PSTN_NODE_NAME, CFG_XML_CHANNEL_PSTNS_PSTN_NODE_DIAL_NAME);
									ucRet = 0x01;
									goto CleanUp;
								}

								if (0 != *pcValue)
								{
									MEM_ALLOC (pstCfgPSTNDial, sizeof (CFG_PSTN_DIAL), (CFG_PSTN_DIAL *));

									strncpy (pstCfgPSTNDial->acDial, pcValue, sizeof (pstCfgPSTNDial->acDial) - 1);

									ucRet = array_add (&pstCfgPSTN->stDials, pstCfgPSTNDial, 0, 0x01, 0);

									if (0x00 != ucRet)
									{
										logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Array List Attribute[%s\\%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_CHANNEL_NAME, CFG_XML_CHANNEL_PSTNS_NAME, CFG_XML_CHANNEL_PSTNS_PSTN_NAME, CFG_XML_CHANNEL_PSTNS_PSTN_NODES_NAME, CFG_XML_CHANNEL_PSTNS_PSTN_NODE_NAME);
										ucRet = 0x01;
										goto CleanUp;
									}
									else
									{
										pstCfgPSTNDial = 0;
									}
								}
							}

							pstPSTNNode = GetNextPeer (pstPSTNNode);
						}
					}

					pstTargets = FindChildByName (pstPSTN, CFG_XML_CHANNEL_PSTNS_PSTN_TARGETS_NAME, 0, 0);
					pstTarget = GetNextChild (pstTargets);

					iTarget = GetTotalChild (pstTargets, CFG_XML_CHANNEL_PSTNS_PSTN_TARGETS_TARGET_NAME);

					if (0 < iTarget)
					{
						array_init (&pstCfgPSTN->stTargets, iTarget, ARRAY_ORDER_DIRECT, 0x00, 0);

						while (0 != pstTarget)
						{
							if (0 == strcmp (pstTarget->pcName, CFG_XML_CHANNEL_PSTNS_PSTN_TARGETS_TARGET_NAME))
							{
								MEM_ALLOC (pstCfgTarget, sizeof (CFG_TARGET), (CFG_TARGET *));

								if (0x00 == FindAttributeValue (pstTarget->pstAttribute, CFG_XML_CHANNEL_PSTNS_PSTN_TARGETS_TARGET_PATTERN_NAME, &pcValue))
								{
									strncpy (pstCfgTarget->acPattern, pcValue, sizeof (pstCfgTarget->acPattern) - 1);
								}
								else
								{
									logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_CHANNEL_NAME, CFG_XML_CHANNEL_PSTNS_NAME, CFG_XML_CHANNEL_PSTNS_PSTN_NAME, CFG_XML_CHANNEL_PSTNS_PSTN_TARGETS_NAME, CFG_XML_CHANNEL_PSTNS_PSTN_TARGETS_TARGET_NAME, CFG_XML_CHANNEL_PSTNS_PSTN_TARGETS_TARGET_PATTERN_NAME);
									ucRet = 0x01;
									goto CleanUp;
								}

								ucRet = array_add (&pstCfgPSTN->stTargets, pstCfgTarget, 0, 0x01, 0);

								if (0x00 != ucRet)
								{
									logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Array List Attribute[%s\\%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_CHANNEL_NAME,CFG_XML_CHANNEL_PSTNS_NAME, CFG_XML_CHANNEL_PSTNS_PSTN_NAME, CFG_XML_CHANNEL_PSTNS_PSTN_TARGETS_NAME, CFG_XML_CHANNEL_PSTNS_PSTN_TARGETS_TARGET_NAME);
									ucRet = 0x01;
									goto CleanUp;
								}
								else
								{
									pstCfgTarget = 0;
								}
							}

							pstTarget = GetNextPeer (pstTarget);
						}
					}
					else
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Element [%s\\%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_CHANNEL_NAME, CFG_XML_CHANNEL_PSTNS_NAME, CFG_XML_CHANNEL_PSTNS_PSTN_NAME, CFG_XML_CHANNEL_PSTNS_PSTN_TARGETS_NAME, CFG_XML_CHANNEL_PSTNS_PSTN_TARGETS_TARGET_NAME);
						goto CleanUp;
					}

					ucRet = array_add (&p_pstCfg->stChannels.stPSTNs, pstCfgPSTN, pstCfgPSTN->acID, 0x01, 0);

					if(0x00 != ucRet)
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Array List Attribute[%s\\%s]. RetCode [0x%02X]", __LINE__, MOD_CFG_NAME, CFG_XML_CHANNEL_NAME, CFG_XML_CHANNEL_PSTNS_NAME, ucRet);
						ucRet = 0x01;
						goto CleanUp;
					}
					else
					{
						pstCfgPSTN = 0;
					}
				}

				pstPSTN = GetNextPeer (pstPSTN);
			}
		}
	}

	pstRS232s = FindChildByName (pstChannels, CFG_XML_CHANNEL_RS232S_NAME, 0, 0);

	if (0 == pstRS232s)
	{
		array_init (&p_pstCfg->stChannels.stRS232s, 0, ARRAY_ORDER_DIRECT, 0x00, 0);
	}
	else
	{
		iRS232 = GetTotalChild (pstRS232s, CFG_XML_CHANNEL_RS232S_RS232_NAME);

		if (0 >= iRS232)
		{
			array_init (&p_pstCfg->stChannels.stRS232s, 0, ARRAY_ORDER_DIRECT, 0x00, 0);
		}
		else
		{
			array_init (&p_pstCfg->stChannels.stRS232s, iRS232, ARRAY_ORDER_DIRECT, 0x00, 0);

			pstRS232 = GetNextChild (pstRS232s);

			while (0 != pstRS232)
			{
				if ( 0 == strcmp(pstRS232->pcName, CFG_XML_CHANNEL_RS232S_RS232_NAME))
				{
					MEM_ALLOC (pstCfgRS232, sizeof (CFG_RS232), (CFG_RS232 *));

					if (0x00 == FindAttributeValue (pstRS232->pstAttribute, CFG_XML_MODULE_RS232S_RS232_ID, &pcValue))
					{
						strncpy (pstCfgRS232->acID, pcValue, sizeof (pstCfgRS232->acID) - 1);
					}
					else
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_CHANNEL_NAME, CFG_XML_CHANNEL_RS232S_NAME, CFG_XML_CHANNEL_RS232S_RS232_NAME, CFG_XML_CHANNEL_RS232S_RS232_ID);
						ucRet = 0x01;
						goto CleanUp;
					}

					// Default value
					pstCfgRS232->iBufSize = 2048;
					pstCfgRS232->ulPackTimeout = 45000;

					if (0x00 == FindAttributeValue (pstRS232->pstAttribute, CFG_XML_CHANNEL_RS232S_RS232_BUFSIZE, &pcValue))
					{
						pstCfgRS232->iBufSize = atoi (pcValue);
					}

					if (0x00 == FindAttributeValue (pstRS232->pstAttribute, CFG_XML_CHANNEL_RS232S_RS232_MODE_NAME, &pcValue))
					{
						if (0 == strcmp (pcValue, "RAW"))
						{
							pstCfgRS232->ucType = CFG_PORT_MODE_RAW;
						}
						else if (0 == strcmp (pcValue, "GSM"))
						{
							pstCfgRS232->ucType = CFG_PORT_MODE_GSM;
						}
						else
						{
							logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_CHANNEL_NAME, CFG_XML_CHANNEL_RS232S_NAME, CFG_XML_CHANNEL_RS232S_RS232_NAME, CFG_XML_CHANNEL_RS232S_RS232_MODE_NAME);
							ucRet = 0x01;
							goto CleanUp;
						}
					}
					else
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_CHANNEL_NAME, CFG_XML_CHANNEL_RS232S_NAME, CFG_XML_CHANNEL_RS232S_RS232_NAME, CFG_XML_CHANNEL_RS232S_RS232_MODE_NAME);
						ucRet = 0x01;
						goto CleanUp;
					}

					if (0x00 == FindAttributeValue (pstRS232->pstAttribute, CFG_XML_CHANNEL_RS232S_RS232_PCK_TIMEOUT_NAME, &pcValue))
					{
						pstCfgRS232->ulPackTimeout = atol (pcValue);
					}

					pstCfgRS232->ucFlags = 0x00;

					pstTargets = FindChildByName (pstRS232, CFG_XML_CHANNEL_RS232S_RS232_TARGETS_NAME, 0, 0);
					pstTarget = GetNextChild (pstTargets);

					iTarget = GetTotalChild (pstTargets, CFG_XML_CHANNEL_RS232S_RS232_TARGETS_TARGET_NAME);

					if (0 < iTarget)
					{
						array_init (&pstCfgRS232->stTargets, iTarget, ARRAY_ORDER_DIRECT, 0x00, 0);

						while (0 != pstTarget)
						{
							if (0 == strcmp (pstTarget->pcName, CFG_XML_CHANNEL_RS232S_RS232_TARGETS_TARGET_NAME))
							{
								MEM_ALLOC (pstCfgTarget, sizeof (CFG_TARGET), (CFG_TARGET *));

								if (0x00 == FindAttributeValue (pstTarget->pstAttribute, CFG_XML_CHANNEL_RS232S_RS232_TARGETS_TARGET_PATTERN_NAME, &pcValue))
								{
									strncpy (pstCfgTarget->acPattern, pcValue, sizeof (pstCfgTarget->acPattern) - 1);
								}
								else
								{
									logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME,  CFG_XML_CHANNEL_NAME, CFG_XML_CHANNEL_RS232S_NAME, CFG_XML_CHANNEL_RS232S_RS232_NAME, CFG_XML_CHANNEL_RS232S_RS232_TARGETS_NAME, CFG_XML_CHANNEL_RS232S_RS232_TARGETS_TARGET_NAME, CFG_XML_CHANNEL_RS232S_RS232_TARGETS_TARGET_PATTERN_NAME);
									ucRet = 0x01;
									goto CleanUp;
								}

								ucRet = array_add (&pstCfgRS232->stTargets, pstCfgTarget, 0, 0x01, 0);

								if (0x00 != ucRet)
								{
									logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Array List Attribute[%s\\%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_CHANNEL_NAME, CFG_XML_CHANNEL_RS232S_NAME, CFG_XML_CHANNEL_RS232S_RS232_NAME, CFG_XML_CHANNEL_RS232S_RS232_TARGETS_NAME, CFG_XML_CHANNEL_RS232S_RS232_TARGETS_TARGET_NAME);
									ucRet = 0x01;
									goto CleanUp;
								}
								else
								{
									pstCfgTarget = 0;
								}
							}

							pstTarget = GetNextPeer (pstTarget);
						}
					}
					else
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Element [%s\\%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_CHANNEL_NAME, CFG_XML_CHANNEL_RS232S_NAME, CFG_XML_CHANNEL_RS232S_RS232_NAME, CFG_XML_CHANNEL_RS232S_RS232_TARGETS_NAME, CFG_XML_CHANNEL_RS232S_RS232_TARGETS_TARGET_NAME);
						goto CleanUp;
					}

					ucRet = array_add (&p_pstCfg->stChannels.stRS232s, pstCfgRS232, pstCfgRS232->acID, 0x01, 0);

					if(0x00 != ucRet)
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Array List Attribute[%s\\%s]. RetCode [0x%02X]", __LINE__, MOD_CFG_NAME, CFG_XML_CHANNEL_NAME, CFG_XML_CHANNEL_RS232S_NAME, ucRet);
						ucRet = 0x01;
						goto CleanUp;
					}
					else
					{
						pstCfgRS232 = 0;
					}
				}

				pstRS232 = GetNextPeer (pstRS232);
			}
		}
	}

	pstRS485s = FindChildByName (pstChannels, CFG_XML_CHANNEL_RS485S_NAME, 0, 0);

	if (0 == pstRS485s)
	{
		array_init (&p_pstCfg->stChannels.stRS485s, 0, ARRAY_ORDER_DIRECT, 0x00, 0);
	}
	else
	{
		iRS485 = GetTotalChild(pstRS485s, CFG_XML_CHANNEL_RS485S_RS485_NAME);

		if (0 >= iRS485)
		{
			array_init (&p_pstCfg->stChannels.stRS485s, 0, ARRAY_ORDER_DIRECT, 0x00, 0);
		}
		else
		{
			array_init (&p_pstCfg->stChannels.stRS485s, iRS485, ARRAY_ORDER_DIRECT, 0x00, 0);

			pstRS485 = GetNextChild (pstRS485s);

			while (0 != pstRS485)
			{
				if (0 == strcmp (pstRS485->pcName, CFG_XML_CHANNEL_RS485S_RS485_NAME))
				{
					MEM_ALLOC (pstCfgRS485, sizeof (CFG_RS485), (CFG_RS485 *));

					if (0x00 == FindAttributeValue (pstRS485->pstAttribute, CFG_XML_CHANNEL_RS485S_RS485_ID, &pcValue))
					{
						strncpy (pstCfgRS485->acID, pcValue, sizeof (pstCfgRS485->acID) - 1);
					}
					else
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_CHANNEL_NAME, CFG_XML_CHANNEL_RS485S_NAME, CFG_XML_CHANNEL_RS485S_RS485_NAME, CFG_XML_CHANNEL_RS485S_RS485_ID);
						ucRet = 0x01;
						goto CleanUp;
					}

					// Default value
					pstCfgRS485->iBufSize = 2048;
					pstCfgRS485->ulPackTimeout = 45000;

					if (0x00 == FindAttributeValue (pstRS485->pstAttribute, CFG_XML_CHANNEL_RS485S_RS485_MODE_NAME, &pcValue))
					{
						if (0 == strcmp (pcValue, "ESLP_MASTER"))
						{
							pstCfgRS485->ucType = CFG_PORT_MODE_ESLP_MASTER;
						}
						else if (0 == strcmp (pcValue, "ESLP_SLAVE"))
						{
							pstCfgRS485->ucType = CFG_PORT_MODE_ESLP_BACKUP;
						}
						else if (0 == strcmp (pcValue, "ESLP_NODE"))
						{
							pstCfgRS485->ucType = CFG_PORT_MODE_ESLP_NODE;
						}
						else if (0 == strcmp (pcValue, "RAW"))
						{
							pstCfgRS485->ucType = CFG_PORT_MODE_RAW;
						}
						else
						{
							logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_CHANNEL_NAME, CFG_XML_CHANNEL_RS485S_NAME, CFG_XML_CHANNEL_RS485S_RS485_NAME, CFG_XML_CHANNEL_RS485S_RS485_MODE_NAME);
							ucRet = 0x01;
							goto CleanUp;
						}
					}
					else
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_CHANNEL_NAME, CFG_XML_CHANNEL_RS485S_NAME, CFG_XML_CHANNEL_RS485S_RS485_NAME, CFG_XML_CHANNEL_RS485S_RS485_MODE_NAME);
						ucRet = 0x01;
						goto CleanUp;
					}

					if (0x00 == FindAttributeValue (pstRS485->pstAttribute, CFG_XML_CHANNEL_RS485S_RS485_BUFSIZE, &pcValue))
					{
						pstCfgRS485->iBufSize = atoi (pcValue);
					}

					pstCfgRS485->ucRawLog = CFG_COMMON_NO;

					if (0x00 == FindAttributeValue (pstRS485->pstAttribute, CFG_XML_CHANNEL_RS485S_RS485_RAW_LOG, &pcValue))
					{
						if (0 == strcmp (pcValue, "Y"))
						{
							pstCfgRS485->ucRawLog = CFG_COMMON_YES;
						}
					}

					if (0x00 == FindAttributeValue (pstRS485->pstAttribute, CFG_XML_CHANNEL_RS485S_RS485_PCK_TIMEOUT_NAME, &pcValue))
					{
						pstCfgRS485->ulPackTimeout = atol(pcValue);
					}

					pstCfgRS485->ucFlags = 0x00;

					pstTargets = FindChildByName (pstRS485, CFG_XML_CHANNEL_RS485S_RS485_TARGETS_NAME, 0, 0);
					pstTarget = GetNextChild (pstTargets);

					iTarget = GetTotalChild (pstTargets, CFG_XML_CHANNEL_RS485S_RS485_TARGETS_TARGET_NAME);

					if (0 < iTarget)
					{
						array_init (&pstCfgRS485->stTargets, iTarget, ARRAY_ORDER_DIRECT, 0x00, 0);

						while(0 != pstTarget)
						{
							if (0 == strcmp (pstTarget->pcName, CFG_XML_CHANNEL_RS485S_RS485_TARGETS_TARGET_NAME))
							{
								MEM_ALLOC (pstCfgTarget, sizeof (CFG_TARGET), (CFG_TARGET *));

								if (0x00 == FindAttributeValue (pstTarget->pstAttribute, CFG_XML_CHANNEL_RS485S_RS485_TARGETS_TARGET_PATTERN_NAME, &pcValue))
								{
									strncpy (pstCfgTarget->acPattern, pcValue, sizeof (pstCfgTarget->acPattern) - 1);
								}
								else
								{
									logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_CHANNEL_NAME, CFG_XML_CHANNEL_RS485S_NAME, CFG_XML_CHANNEL_RS485S_RS485_NAME, CFG_XML_CHANNEL_RS485S_RS485_TARGETS_NAME, CFG_XML_CHANNEL_RS485S_RS485_TARGETS_TARGET_NAME, CFG_XML_CHANNEL_RS485S_RS485_TARGETS_TARGET_PATTERN_NAME);
									ucRet = 0x01;
									goto CleanUp;
								}

								ucRet = array_add (&pstCfgRS485->stTargets, pstCfgTarget, 0, 0x01, 0);

								if (0x00 != ucRet)
								{
									logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Array List Attribute[%s\\%s\\%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_CHANNEL_NAME, CFG_XML_CHANNEL_RS485S_NAME, CFG_XML_CHANNEL_RS485S_RS485_NAME, CFG_XML_CHANNEL_RS485S_RS485_TARGETS_NAME, CFG_XML_CHANNEL_RS485S_RS485_TARGETS_TARGET_NAME, CFG_XML_CHANNEL_RS485S_RS485_TARGETS_TARGET_PATTERN_NAME);
									ucRet = 0x01;
									goto CleanUp;
								}
								else
								{
									pstCfgTarget = 0;
								}
							}

							pstTarget = GetNextPeer (pstTarget);
						}

						ucRet = array_add (&p_pstCfg->stChannels.stRS485s, pstCfgRS485, pstCfgRS485->acID, 0x01, 0);

						if (0x00 != ucRet)
						{
							logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Array List Attribute[%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_CHANNEL_NAME, CFG_XML_CHANNEL_RS485S_NAME, CFG_XML_CHANNEL_RS485S_RS485_NAME);
							ucRet = 0x01;
							goto CleanUp;
						}
						else
						{
							pstCfgRS485 = 0;
						}
					}
					else
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Element [%s\\%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME,  CFG_XML_CHANNEL_NAME, CFG_XML_CHANNEL_RS485S_NAME, CFG_XML_CHANNEL_RS485S_RS485_NAME, CFG_XML_CHANNEL_RS485S_RS485_TARGETS_NAME, CFG_XML_CHANNEL_RS485S_RS485_TARGETS_TARGET_NAME);
						goto CleanUp;
					}
				}

				pstRS485 = GetNextPeer (pstRS485);
			}
		}
	}

	ucRet = 0x00;

CleanUp:
	MEM_CLEAN (pstCfgTCP);
	MEM_CLEAN (pstCfgPSTN);
	MEM_CLEAN (pstCfgPSTNDial);
	MEM_CLEAN (pstCfgRS232);
	MEM_CLEAN (pstCfgRS485);
	MEM_CLEAN (pstCfgTarget);

	return ucRet;
}

#if 0
unsigned char xml_config_crypto (stXMLParserMainData *p_pstXML, CFG_CORE_ENGINE  *p_pstCfg)
{
	unsigned char	ucRet				= 0x00;

	int				iCrypto;

	char			acKey [128]			= { 0 },
					*pcValue;

	stElement		*pstElement,
					*pstCryptos,
					*pstCrypto;

	CFG_CRYPTO		*pstCfgCrypto		= 0;

	pstElement = p_pstXML->pstMasElement;
	pstCryptos = FindChildByName (pstElement, CFG_XML_CRYPTOS_NAME, 0, 0);

	if (0 == pstCryptos)
	{
		array_init (&p_pstCfg->stCryptos, 0, ARRAY_ORDER_DIRECT, 0x00, 0);
	}
	else
	{
		iCrypto = GetTotalChild (pstCryptos, CFG_XML_CRYPTO_NAME);

		if (0 >= iCrypto)
		{
			array_init (&p_pstCfg->stCryptos, 0, ARRAY_ORDER_DIRECT, 0x00, 0);
		}
		else
		{
			array_init (&p_pstCfg->stCryptos, iCrypto, ARRAY_ORDER_DIRECT, 0x00, 0);

			pstCrypto = GetNextChild (pstCryptos);

			while (0 != pstCrypto)
			{
				if (0 == strcmp (pstCrypto->pcName, CFG_XML_CRYPTO_NAME))
				{
					MEM_ALLOC (pstCfgCrypto, sizeof (CFG_CRYPTO), (CFG_CRYPTO *));

					if (0x00 == FindAttributeValue (pstCrypto->pstAttribute, CFG_XML_CRYPTO_ID_NAME, &pcValue))
					{
						strncpy (pstCfgCrypto->acID, pcValue, sizeof (pstCfgCrypto->acID) - 1);
					}
					else
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute[%s\\%s\\%s]", __LINE__, MOD_CFG_NAME,
							CFG_XML_CRYPTOS_NAME, CFG_XML_CRYPTO_NAME, CFG_XML_CRYPTO_ID_NAME);
						ucRet = 0x01;
						goto CleanUp;
					}

					pstCfgCrypto->ucType = CFG_CRYPTO_TYPE_SSL;

					if (0x00 == FindAttributeValue (pstCrypto->pstAttribute, CFG_XML_CRYPTO_TYPE_NAME, &pcValue))
					{
						if (0 == strcmp (pcValue, "ENC"))
						{
							pstCfgCrypto->ucType = CFG_CRYPTO_TYPE_ENC;
						}
						else if (0 != strcmp (pcValue, "SSL"))
						{
							logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Invalid Attribute[%s\\%s\\%s = %s]", __LINE__, MOD_CFG_NAME,
								CFG_XML_CRYPTOS_NAME, CFG_XML_CRYPTO_NAME, CFG_XML_CRYPTO_TYPE_NAME, pcValue);
							ucRet = 0x01;
							goto CleanUp;
						}
					}

					if (0x00 == FindAttributeValue (pstCrypto->pstAttribute, CFG_XML_CRYPTO_INDEX_NAME, &pcValue))
					{
						pstCfgCrypto->ucIndex = atoi (pcValue);
					}

					if (0x00 == FindAttributeValue (pstCrypto->pstAttribute, CFG_XML_CRYPTO_SESSION_TIMEOUT_NAME, &pcValue))
					{
						pstCfgCrypto->ulSessionTimeout = atol (pcValue);
					}

					pstCfgCrypto->ucSrc = CFG_CRYPTO_SRC_SAM;

					if (0x00 == FindAttributeValue (pstCrypto->pstAttribute, CFG_XML_CRYPTO_SRC_NAME, &pcValue))
					{
						if (0 == strcmp (pcValue, "FILE"))
						{
							pstCfgCrypto->ucSrc = CFG_CRYPTO_SRC_FILE;
						}
						else if (0 == strcmp (pcValue, "RAW"))
						{
							pstCfgCrypto->ucSrc = CFG_CRYPTO_SRC_RAW;
						}
						else if (0 != strcmp (pcValue, "SAM"))
						{
							logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Invalid Attribute[%s\\%s\\%s = %s]", __LINE__, MOD_CFG_NAME,
								CFG_XML_CRYPTOS_NAME, CFG_XML_CRYPTO_NAME, CFG_XML_CRYPTO_SRC_NAME, pcValue);
							ucRet = 0x01;
							goto CleanUp;
						}
					}

					if (0x00 == FindAttributeValue (pstCrypto->pstAttribute, CFG_XML_CRYPTO_PKCS11_OPS_IP_NAME, &pcValue))
					{
						strncpy (pstCfgCrypto->acOPSIP, pcValue, sizeof (pstCfgCrypto->acOPSIP) - 1);
					}

					if (0x00 == FindAttributeValue (pstCrypto->pstAttribute, CFG_XML_CRYPTO_PKCS11_OPS_PORT_NAME, &pcValue))
					{
						pstCfgCrypto->iOPSPort = atoi (pcValue);
					}

					if (0x00 == FindAttributeValue (pstCrypto->pstAttribute, CFG_XML_CRYPTO_PKCS11_OPS_TIMEOUT_NAME, &pcValue))
					{
						pstCfgCrypto->ulOPSTimeout = atol (pcValue);
					}

					memset (pstCfgCrypto->acCipher, 0x00, sizeof (pstCfgCrypto->acCipher));

					if (CFG_CRYPTO_TYPE_SSL == pstCfgCrypto->ucType)
					{
						strcpy (pstCfgCrypto->acCipher, "AES256-SHA");
					}

					if (0x00 == FindAttributeValue (pstCrypto->pstAttribute, CFG_XML_CRYPTO_CIPHER_NAME, &pcValue))
					{
						strncpy (pstCfgCrypto->acCipher, pcValue, sizeof (pstCfgCrypto->acCipher) - 1);
					}

					if (0x00 == FindAttributeValue (pstCrypto->pstAttribute, CFG_XML_CRYPTO_CERT_NAME, &pcValue))
					{
						strncpy (pstCfgCrypto->acCert, pcValue, sizeof (pstCfgCrypto->acCert) - 1);
					}

					if (0x00 == FindAttributeValue (pstCrypto->pstAttribute, CFG_XML_CRYPTO_PRVT_KEY_NAME, &pcValue))
					{
						if (CFG_CRYPTO_TYPE_ENC == pstCfgCrypto->ucType)
						{
							// Decrypt and get the actual key
							ucRet = cor_getkey (pcValue, acKey);

							if (0x00 != ucRet)
							{
								logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Invalid Attribute[%s\\%s\\%s = %s]", __LINE__, MOD_CFG_NAME,
									CFG_XML_CRYPTOS_NAME, CFG_XML_CRYPTO_NAME, CFG_XML_CRYPTO_PRVT_KEY_NAME, pcValue);

								ISLEEP (5000);	// Purposely delay a bit before exit, this to prevent burst attack to retrieve key

								ucRet = 0x01;
								goto CleanUp;
							}

							pcValue = acKey;
						}

						strncpy (pstCfgCrypto->acPrvtKey, pcValue, sizeof (pstCfgCrypto->acPrvtKey) - 1);
					}

					if (0x00 == FindAttributeValue (pstCrypto->pstAttribute, CFG_XML_CRYPTO_CA_NAME, &pcValue))
					{
						strncpy (pstCfgCrypto->acCA, pcValue, sizeof (pstCfgCrypto->acCA) - 1);
					}

					if (0x00 == FindAttributeValue (pstCrypto->pstAttribute, CFG_XML_CRYPTO_CRL_NAME, &pcValue))
					{
						strncpy (pstCfgCrypto->acCRL, pcValue, sizeof (pstCfgCrypto->acCRL) - 1);
					}

					if (0x00 == FindAttributeValue (pstCrypto->pstAttribute, CFG_XML_CRYPTO_PASSPHASE_NAME, &pcValue))
					{
						strncpy (pstCfgCrypto->acPassPhase, pcValue, sizeof (pstCfgCrypto->acPassPhase) - 1);
					}

					if (0x00 == FindAttributeValue (pstCrypto->pstAttribute, CFG_XML_CRYPTO_CN_NAME, &pcValue))
					{
						strncpy (pstCfgCrypto->acCN, pcValue, sizeof (pstCfgCrypto->acCN) - 1);
					}

					pstCfgCrypto->ucPadding = CFG_CRYPTO_PADDING_TYPE_PKCS;

					if (0x00 == FindAttributeValue (pstCrypto->pstAttribute, CFG_XML_CRYPTO_PADDING_NAME, &pcValue))
					{
						if (0 == strcmp (pcValue, "NULL_NUM_BYTES"))
						{
							pstCfgCrypto->ucPadding = CFG_CRYPTO_PADDING_TYPE_NULL_NUM_BYTES;
						}
						else if (0 != strcmp (pcValue, "PKCS"))
						{
							logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Invalid Attribute[%s\\%s\\%s = %s]", __LINE__, MOD_CFG_NAME,
								CFG_XML_CRYPTOS_NAME, CFG_XML_CRYPTO_NAME, CFG_XML_CRYPTO_PADDING_NAME, pcValue);
							ucRet = 0x01;
							goto CleanUp;
						}
					}

					ucRet = array_add (&p_pstCfg->stCryptos, pstCfgCrypto, pstCfgCrypto->acID, 0x01, 0);

					if (0x00 != ucRet)
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Array List Attribute[%s\\%s]", __LINE__, MOD_CFG_NAME,  CFG_XML_CRYPTOS_NAME, CFG_XML_CRYPTO_NAME);
						goto CleanUp;
					}
					else
					{
						pstCfgCrypto = 0;
					}
				}

				pstCrypto = GetNextPeer (pstCrypto);
			}
		}
	}

	ucRet = 0x00;

CleanUp:
	MEM_CLEAN (pstCfgCrypto);

	return ucRet;
}
#endif
unsigned char xml_config_pattern (stXMLParserMainData *p_pstXML, CFG_CORE_ENGINE  *p_pstCfg)
{
	unsigned char	ucRet				= 0x00,
					ucFlags				= 0x00;

	char			acLen[3]			= { 0 },
					*pcValue			= 0,
					*pcLen				= 0,
					*pcPtr				= 0;

	int				iPattern,
					iDest,
					iRoute,
					iLen				= 0;

	stElement		*pstElement,
					*pstPatterns,
					*pstPattern,
					*pstParser,
					*pstRoute,
					*pstDest;

	CFG_PATTERN		*pstCfgPattern		= 0;

	CFG_DEST		*pstCfgDest			= 0;

	CFG_ROUTE		*pstCfgRoute		= 0;

	pstElement = p_pstXML->pstMasElement;
	pstPatterns = FindChildByName (pstElement, CFG_XML_PATTERNS_NAME, 0, 0);
	pstPattern = FindChildByName (pstPatterns, CFG_XML_PATTERNS_PATTERN_NAME, 0, 0);

	if (0 == pstPatterns)
	{
		array_init (&p_pstCfg->stPatterns, 0, ARRAY_ORDER_DIRECT, 0x00, 0);
	}
	else
	{
		iPattern = GetTotalChild (pstPatterns, CFG_XML_PATTERNS_PATTERN_NAME);

		if (0 >= iPattern)
		{
			array_init (&p_pstCfg->stPatterns, 0, ARRAY_ORDER_DIRECT, 0x00, 0);
		}
		else
		{
			array_init (&p_pstCfg->stPatterns, iPattern, ARRAY_ORDER_DIRECT, 0x00, 0);

			pstPattern = GetNextChild (pstPatterns);

			while (0 != pstPattern)
			{
				if (0 == strcmp (pstPattern->pcName, CFG_XML_PATTERNS_PATTERN_NAME))
				{
					MEM_ALLOC (pstCfgPattern, sizeof (CFG_PATTERN), (CFG_PATTERN *));

					if (0x00 == FindAttributeValue (pstPattern->pstAttribute, CFG_XML_PATTERNS_PATTERN_ID_NAME, &pcValue))
					{
						strncpy (pstCfgPattern->acID, pcValue, sizeof (pstCfgPattern->acID) - 1);
					}
					else
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute[%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_PATTERNS_NAME, CFG_XML_PATTERNS_PATTERN_NAME, CFG_XML_PATTERNS_PATTERN_ID_NAME);
						ucRet = 0x01;
						goto CleanUp;
					}

					if (0x00 == FindAttributeValue (pstPattern->pstAttribute, CFG_XML_PATTERNS_PATTERN_FORMAT_NAME, &pcValue))
					{
						ucRet = ucHexToByte (pcValue, pstCfgPattern->aucPtn, &iLen) ;

						if (0x00 == ucRet)
						{
							pstCfgPattern->iPtnLen = iLen;
						}
						else
						{
							logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute[%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_PATTERNS_NAME, CFG_XML_PATTERNS_PATTERN_NAME, CFG_XML_PATTERNS_PATTERN_FORMAT_NAME);
							ucRet = 0x01;
							goto CleanUp;
						}
					}
					else
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute[%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_PATTERNS_NAME, CFG_XML_PATTERNS_PATTERN_NAME, CFG_XML_PATTERNS_PATTERN_FORMAT_NAME);
						ucRet = 0x01;
						goto CleanUp;
					}

					// Optional
					pstParser = FindChildByName (pstPattern, CFG_XML_PATTERNS_PATTERN_PARSER_NAME, 0, 0);

					if (0 == pstParser)
					{
						strncpy (pstCfgPattern->stParser.acID, "NONE", sizeof (pstCfgPattern->stParser.acID) - 1);
						pstCfgPattern->stParser.ucType = CFG_PARSER_TYPE_NONE;
					}
					else
					{
						if (0x00 == FindAttributeValue (pstParser->pstAttribute, CFG_XML_PATTERNS_PATTERN_PARSER_ID, &pcValue))
						{
							if (0 == *pcValue)
							{
								strncpy (pstCfgPattern->stParser.acID, "NONE", sizeof (pstCfgPattern->stParser.acID) - 1);
								pstCfgPattern->stParser.ucType = CFG_PARSER_TYPE_NONE;
							}
							else
							{
								strncpy (pstCfgPattern->stParser.acID, pcValue, sizeof (pstCfgPattern->stParser.acID) - 1);

								if (0 == strcmp (pstCfgPattern->stParser.acID, "NONE"))
								{
									pstCfgPattern->stParser.ucType = CFG_PARSER_TYPE_NONE;
								}
								else if (0 == strcmp (pstCfgPattern->stParser.acID, "TPDU"))
								{
									pstCfgPattern->stParser.ucType = CFG_PARSER_TYPE_TPDU;
								}
								else if (0 == strcmp (pstCfgPattern->stParser.acID, "ISO8583"))
								{
									pstCfgPattern->stParser.ucType = CFG_PARSER_TYPE_ISO8583;
								}
								else if (0 == strcmp (pstCfgPattern->stParser.acID, "APACS"))
								{
									pstCfgPattern->stParser.ucType = CFG_PARSER_TYPE_APACS;
								}
								else
								{
									logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Invalid Attribute[%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME,  CFG_XML_PATTERNS_NAME, CFG_XML_PATTERNS_PATTERN_NAME, CFG_XML_PATTERNS_PATTERN_PARSER_NAME, CFG_XML_PATTERNS_PATTERN_PARSER_ID);
									ucRet = 0x01;
									goto CleanUp;
								}
							}
						}
						else
						{
							logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute[%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_PATTERNS_NAME, CFG_XML_PATTERNS_PATTERN_NAME, CFG_XML_PATTERNS_PATTERN_PARSER_NAME, CFG_XML_PATTERNS_PATTERN_PARSER_ID);
							ucRet = 0x01;
							goto CleanUp;
						}

						ucFlags = 0x00;

						if (0x00 == FindAttributeValue (pstParser->pstAttribute, CFG_XML_PATTERNS_PATTERN_PARSER_STAMPTPDU, &pcValue))
						{
							if (0 == strcmp (pcValue, "Y"))
							{
								//10000000
								ucFlags = (ucFlags | 0x80);
							}
							else if (0 == strcmp (pcValue, "N"))
							{
								//01111111
								ucFlags = (ucFlags & 0x7F);
							}

							pstCfgPattern->stParser.ucFlags = ucFlags;
						}
						else
						{
							logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute[%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_PATTERNS_NAME, CFG_XML_PATTERNS_PATTERN_NAME, CFG_XML_PATTERNS_PATTERN_PARSER_NAME, CFG_XML_PATTERNS_PATTERN_PARSER_STAMPTPDU);
							ucRet = 0x01;
							goto CleanUp;
						}

						if (0x00 == FindAttributeValue (pstParser->pstAttribute, CFG_XML_PATTERNS_PATTERN_PARSER_PROTOCOLFMT_NAME, &pcValue))
						{
							iLen = strlen (pcValue) - 1;

							if ('{' == *pcValue && '}' == *(pcValue + iLen))
							{
								//find last deliminator
								pcLen = strchr(pcValue, ':');

								if (0 != pcLen)
								{
									//{HL_BCD:19}
									pcPtr = strchr (pcValue, '}');

									if (0 != pcPtr)
									{
										iLen = pcPtr - pcLen - 1;

										if (iLen < sizeof (acLen))
										{
											memset (acLen, 0x00, sizeof (acLen));
											strncpy (acLen, pcLen + 1, iLen);

											pstCfgPattern->stParser.ucProtocolLen = atoi (acLen) ;
										}
									}
								}

								if (0 == strncmp (pcValue, "{HL_HEX:", 8))
								{
									pstCfgPattern->stParser.ucProtocolFmt = CFG_PATTERN_PROTOCOL_FMT_HL_HEX;
								}
								else if (0 == strncmp (pcValue, "{HL_BCD:", 8))
								{
									pstCfgPattern->stParser.ucProtocolFmt = CFG_PATTERN_PROTOCOL_FMT_HL_BCD;
								}
								else if (0 == strncmp (pcValue, "{IHL_HEX:", 9))
								{
									pstCfgPattern->stParser.ucProtocolFmt = CFG_PATTERN_PROTOCOL_FMT_IHL_HEX;
								}
								else if (0 == strncmp (pcValue, "{IHL_BCD:", 9))
								{
									pstCfgPattern->stParser.ucProtocolFmt = CFG_PATTERN_PROTOCOL_FMT_IHL_BCD;
								}
								else if (0 == strncmp (pcValue, "{HLHL_HEX:", 10))
								{
									pstCfgPattern->stParser.ucProtocolFmt = CFG_PATTERN_PROTOCOL_FMT_HLHL_HEX;
								}
								else if (0 == strncmp (pcValue, "{HLHL_BCD:", 10))
								{
									pstCfgPattern->stParser.ucProtocolFmt = CFG_PATTERN_PROTOCOL_FMT_HLHL_BCD;
								}
								else if (0 == strncmp (pcValue, "{LRC_HEX:", 9))
								{
									pstCfgPattern->stParser.ucProtocolFmt = CFG_PATTERN_PROTOCOL_FMT_LRC_HEX;
								}
								else if (0 == strncmp (pcValue, "{LRC_BCD:", 9))
								{
									pstCfgPattern->stParser.ucProtocolFmt = CFG_PATTERN_PROTOCOL_FMT_LRC_BCD;
								}
								else if (0 == strcmp (pcValue, "{NONE}"))
								{
									pstCfgPattern->stParser.ucProtocolFmt = CFG_PATTERN_PROTOCOL_FMT_NONE;
								}
								else
								{
									logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute[%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_PATTERNS_NAME, CFG_XML_PATTERNS_PATTERN_NAME, CFG_XML_PATTERNS_PATTERN_PARSER_NAME, CFG_XML_PATTERNS_PATTERN_PARSER_PROTOCOLFMT_NAME);
									ucRet = 0x01;
									goto CleanUp;
								}
							}
							else
							{
								logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute[%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_PATTERNS_NAME, CFG_XML_PATTERNS_PATTERN_NAME, CFG_XML_PATTERNS_PATTERN_PARSER_NAME, CFG_XML_PATTERNS_PATTERN_PARSER_PROTOCOLFMT_NAME);
								ucRet = 0x01;
								goto CleanUp;
							}
						}
						else
						{
							logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute[%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_PATTERNS_NAME, CFG_XML_PATTERNS_PATTERN_NAME, CFG_XML_PATTERNS_PATTERN_PARSER_NAME, CFG_XML_PATTERNS_PATTERN_PARSER_PROTOCOLFMT_NAME);
							ucRet = 0x01;
							goto CleanUp;
						}

						if (0x00 == FindAttributeValue (pstParser->pstAttribute, CFG_XML_PATTERNS_PATTERN_PARSER_DEF_FILE_NAME, &pcValue))
						{
							strncpy (pstCfgPattern->stParser.acDefFile, pcValue, sizeof (pstCfgPattern->stParser.acDefFile) - 1);
						}
						else
						{
							pstCfgPattern->stParser.acDefFile [0] = 0;
						}

						// Load ISO8583 definition file if any
						if (CFG_PARSER_TYPE_ISO8583 == pstCfgPattern->stParser.ucType)
						{
							ucRet = iso8583_init (&pstCfgPattern->stParser.stISO8583, pstCfgPattern->stParser.acDefFile);

							if (0x00 != ucRet)
							{
								if (0 != pstCfgPattern->stParser.acDefFile [0])
								{
									logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: ISO8583 initialize failure with file %s. RetCode [0x%02X]", __LINE__, MOD_CFG_NAME, pstCfgPattern->stParser.acDefFile, ucRet);
								}
								else
								{
									logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: ISO8583 initialize failure. RetCode [0x%02X]", __LINE__, MOD_CFG_NAME, ucRet);
								}

								ucRet = 0x01;
								goto CleanUp;
							}
						}
					}

					pstRoute = FindChildByName (pstPattern, CFG_XML_PATTERNS_PATTERN_ROUTE_NAME, 0, 0);

					if (0 == pstRoute)
					{
						array_init (&pstCfgPattern->stRoutes, 0, ARRAY_ORDER_DIRECT, 0x00, 0);
					}
					else
					{
						iRoute = GetTotalSamePeer (pstRoute);

						if (0 >= iRoute)
						{
							array_init (&pstCfgPattern->stRoutes, 0, ARRAY_ORDER_DIRECT, 0x00, 0);
						}
						else
						{
							array_init (&pstCfgPattern->stRoutes, iRoute, ARRAY_ORDER_DIRECT, 0x00, 0);

							while (0 != pstRoute)
							{
								if (0 == strcmp(pstRoute->pcName, CFG_XML_PATTERNS_PATTERN_ROUTE_NAME))
								{
									MEM_ALLOC (pstCfgRoute, sizeof (CFG_ROUTE), (CFG_ROUTE *));

									if (0x00 == FindAttributeValue (pstRoute->pstAttribute, CFG_XML_PATTERNS_PATTERN_ROUTE_KEY_NAME, &pcValue))
									{
										strncpy (pstCfgRoute->acKey, pcValue, sizeof (pstCfgRoute->acKey) - 1);
									}
									else
									{
										logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute[%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_PATTERNS_NAME, CFG_XML_PATTERNS_PATTERN_NAME, CFG_XML_PATTERNS_PATTERN_ROUTE_NAME, CFG_XML_PATTERNS_PATTERN_ROUTE_KEY_NAME);
										ucRet = 0x01;
										goto CleanUp;
									}

									if (0x00 == FindAttributeValue (pstRoute->pstAttribute, CFG_XML_PATTERNS_PATTERN_ROUTE_VAL_NAME, &pcValue))
									{
										strncpy (pstCfgRoute->acVal, pcValue, sizeof (pstCfgRoute->acVal) - 1);
									}
									else
									{
										logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute[%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_PATTERNS_NAME, CFG_XML_PATTERNS_PATTERN_NAME, CFG_XML_PATTERNS_PATTERN_ROUTE_NAME, CFG_XML_PATTERNS_PATTERN_ROUTE_VAL_NAME);
										ucRet = 0x01;
										goto CleanUp;
									}

									if (0x00 == FindAttributeValue (pstRoute->pstAttribute, CFG_XML_PATTERNS_PATTERN_ROUTE_COREL_NAME, &pcValue))
									{
										strncpy (pstCfgRoute->acCoRel, pcValue, sizeof (pstCfgRoute->acCoRel) - 1);
									}
									else
									{
										logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute[%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_PATTERNS_NAME, CFG_XML_PATTERNS_PATTERN_NAME, CFG_XML_PATTERNS_PATTERN_ROUTE_NAME, CFG_XML_PATTERNS_PATTERN_ROUTE_COREL_NAME);
										ucRet = 0x01;
										goto CleanUp;
									}

									if (0x00 == FindAttributeValue (pstRoute->pstAttribute, CFG_XML_PATTERNS_PATTERN_ROUTE_GENCOREL_NAME, &pcValue))
									{
										strncpy (pstCfgRoute->acGenCoRel, pcValue, sizeof (pstCfgRoute->acGenCoRel) - 1);
									}
									else
									{
										logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute[%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_PATTERNS_NAME, CFG_XML_PATTERNS_PATTERN_NAME, CFG_XML_PATTERNS_PATTERN_ROUTE_NAME, CFG_XML_PATTERNS_PATTERN_ROUTE_GENCOREL_NAME);
										ucRet = 0x01;
										goto CleanUp;
									}

									if (0x00 == FindAttributeValue (pstRoute->pstAttribute, CFG_XML_PATTERNS_PATTERN_ROUTE_SELFROUTE_NAME, &pcValue))
									{
										if (0 == strcmp (pcValue, "Y"))
										{
											//10000000
											ucFlags = (ucFlags | 0x80);
										}
										else if (0 == strcmp (pcValue, "N"))
										{
											//01111111
											ucFlags = (ucFlags & 0x7F);
										}

										pstCfgRoute->ucFlags = ucFlags;
									}
									else
									{
										logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute[%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_PATTERNS_NAME, CFG_XML_PATTERNS_PATTERN_NAME, CFG_XML_PATTERNS_PATTERN_ROUTE_NAME, CFG_XML_PATTERNS_PATTERN_ROUTE_SELFROUTE_NAME);
										ucRet = 0x01;
										goto CleanUp;
									}

									pstDest = FindChildByName (pstRoute, CFG_XML_PATTERNS_PATTERN_DEST_NAME, 0, 0);

									if (0 == pstDest)
									{
										array_init (&pstCfgRoute->stDests, 0, ARRAY_ORDER_DIRECT, 0x00, 0);
									}
									else
									{
										iDest = GetTotalChild (pstRoute, CFG_XML_PATTERNS_PATTERN_DEST_NAME);

										if (0 >= iDest)
										{
											array_init (&pstCfgRoute->stDests, 0, ARRAY_ORDER_DIRECT, 0x00, 0);
										}
										else
										{
											array_init (&pstCfgRoute->stDests, iDest, ARRAY_ORDER_DIRECT, 0x00, 0);

											while (0 != pstDest)
											{
												if (0 == strcmp (pstDest->pcName, CFG_XML_PATTERNS_PATTERN_DEST_NAME))
												{
													MEM_ALLOC (pstCfgDest, sizeof (CFG_DEST), (CFG_DEST *));

													if (0x00 == FindAttributeValue (pstDest->pstAttribute, CFG_XML_PATTERNS_PATTERN_DEST_ID_NAME, &pcValue))
													{
														strncpy (pstCfgDest->acID, pcValue, sizeof (pstCfgDest->acID) - 1);
													}
													else
													{
														logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute[%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_PATTERNS_NAME, CFG_XML_PATTERNS_PATTERN_NAME, CFG_XML_PATTERNS_PATTERN_DEST_NAME, CFG_XML_PATTERNS_PATTERN_DEST_ID_NAME);
														ucRet = 0x01;
														goto CleanUp;
													}

													if (0x00 == FindAttributeValue (pstDest->pstAttribute, CFG_XML_PATTERNS_PATTERN_DEST_TYPE_NAME, &pcValue))
													{
														if (0 == strcmp (pcValue, "P"))
														{
															pstCfgDest->ucType = CFG_PATT_DEST_PRIMARY;
														}
														else if(0==strcmp(pcValue,"S"))
														{
															pstCfgDest->ucType = CFG_PATT_DEST_SECONDARY;
														}
													}
													else
													{
														logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute[%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_PATTERNS_NAME, CFG_XML_PATTERNS_PATTERN_NAME, CFG_XML_PATTERNS_PATTERN_DEST_NAME, CFG_XML_PATTERNS_PATTERN_DEST_TYPE_NAME);
														ucRet = 0x01;
														goto CleanUp;
													}

													if (0x00 == FindAttributeValue (pstDest->pstAttribute, CFG_XML_PATTERNS_PATTERN_DEST_WEIGHT_NAME, &pcValue))
													{
														pstCfgDest->ucWeight = atoi (pcValue);

													}
													else
													{
														logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute[%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_PATTERNS_NAME, CFG_XML_PATTERNS_PATTERN_NAME, CFG_XML_PATTERNS_PATTERN_DEST_NAME, CFG_XML_PATTERNS_PATTERN_DEST_WEIGHT_NAME);
														ucRet = 0x01;
														goto CleanUp;
													}

													if (0x00 == FindAttributeValue (pstDest->pstAttribute, CFG_XML_PATTERNS_PATTERN_DEST_CONGESTED_NAME, &pcValue))
													{
														pstCfgDest->iCongested = atoi(pcValue);
													}
													else
													{
														logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute[%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_PATTERNS_NAME, CFG_XML_PATTERNS_PATTERN_NAME, CFG_XML_PATTERNS_PATTERN_DEST_NAME, CFG_XML_PATTERNS_PATTERN_DEST_CONGESTED_NAME);
														ucRet = 0x01;
														goto CleanUp;
													}

													if (0x00 == FindAttributeValue (pstDest->pstAttribute, CFG_XML_PATTERNS_PATTERN_DEST_THRESHOLD_NAME, &pcValue))
													{
														pstCfgDest->iThreshold = atoi(pcValue);
													}
													else
													{
														logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute[%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_PATTERNS_NAME, CFG_XML_PATTERNS_PATTERN_NAME, CFG_XML_PATTERNS_PATTERN_DEST_NAME, CFG_XML_PATTERNS_PATTERN_DEST_THRESHOLD_NAME);
														ucRet = 0x01;
														goto CleanUp;
													}

													if (0x00 == FindAttributeValue (pstDest->pstAttribute, CFG_XML_PATTERNS_PATTERN_DEST_PROTOCOLFMT_NAME, &pcValue))
													{
														iLen = strlen (pcValue) - 1;

														if ('{' == *pcValue && '}' == *(pcValue + iLen))
														{
															//find last deliminator
															pcLen = strchr (pcValue, ':');

															if (pcLen)
															{
																//{HL_BCD:19}
																pcPtr = strchr (pcValue, '}');

																iLen = pcPtr - pcLen - 1;
																strncpy (acLen, pcLen + 1, iLen);
																acLen [iLen] = 0;

																pstCfgDest->ucProtocolLen = atoi (acLen) ;
															}

															if (0 == strncmp (pcValue, "{HL_HEX:", 8))
															{
																pstCfgDest->ucProtocolFmt = CFG_PATTERN_PROTOCOL_FMT_HL_HEX;
															}
															else if (0 == strncmp (pcValue, "{HL_BCD:", 8))
															{
																pstCfgDest->ucProtocolFmt = CFG_PATTERN_PROTOCOL_FMT_HL_BCD;
															}
															else if (0 == strncmp (pcValue, "{IHL_HEX:", 9))
															{
																pstCfgDest->ucProtocolFmt = CFG_PATTERN_PROTOCOL_FMT_IHL_HEX;
															}
															else if (0 == strncmp (pcValue, "{IHL_BCD:", 9))
															{
																pstCfgDest->ucProtocolFmt = CFG_PATTERN_PROTOCOL_FMT_IHL_BCD;
															}
															else if (0 == strncmp (pcValue, "{HLHL_HEX:", 10))
															{
																pstCfgDest->ucProtocolFmt = CFG_PATTERN_PROTOCOL_FMT_HLHL_HEX;
															}
															else if (0 == strncmp (pcValue, "{HLHL_BCD:", 10))
															{
																pstCfgDest->ucProtocolFmt = CFG_PATTERN_PROTOCOL_FMT_HLHL_BCD;
															}
															else if (0 == strncmp (pcValue, "{LRC_HEX:", 9))
															{
																pstCfgDest->ucProtocolFmt = CFG_PATTERN_PROTOCOL_FMT_LRC_HEX;
															}
															else if (0 == strncmp (pcValue, "{LRC_BCD:", 9))
															{
																pstCfgDest->ucProtocolFmt = CFG_PATTERN_PROTOCOL_FMT_LRC_BCD;
															}
															else if (0 == strcmp (pcValue, "{NONE}"))
															{
																pstCfgDest->ucProtocolFmt = CFG_PATTERN_PROTOCOL_FMT_NONE;
															}
															else
															{
																logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute[%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_PATTERNS_NAME, CFG_XML_PATTERNS_PATTERN_NAME, CFG_XML_PATTERNS_PATTERN_DEST_NAME, CFG_XML_PATTERNS_PATTERN_DEST_PROTOCOLFMT_NAME);
																ucRet = 0x01;
																goto CleanUp;
															}
														}
														else
														{
															logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute[%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_PATTERNS_NAME, CFG_XML_PATTERNS_PATTERN_NAME, CFG_XML_PATTERNS_PATTERN_DEST_NAME, CFG_XML_PATTERNS_PATTERN_DEST_PROTOCOLFMT_NAME);
															ucRet = 0x01;
															goto CleanUp;
														}
													}
													else
													{
														logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute[%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_PATTERNS_NAME, CFG_XML_PATTERNS_PATTERN_NAME, CFG_XML_PATTERNS_PATTERN_DEST_NAME, CFG_XML_PATTERNS_PATTERN_DEST_PROTOCOLFMT_NAME);
														ucRet = 0x01;
														goto CleanUp;
													}

													if (0x00 == FindAttributeValue (pstDest->pstAttribute, CFG_XML_PATTERNS_PATTERN_DEST_COOLDOWN_NAME, &pcValue))
													{
														pstCfgDest->ulCooldown = atol (pcValue);
													}
													else
													{
														// This is because have some typo in previous version, so we need to make it backward compatible
														if (0x00 == FindAttributeValue (pstDest->pstAttribute, CFG_XML_PATTERNS_PATTERN_DEST_COLDOWN_NAME, &pcValue))
														{
															pstCfgDest->ulCooldown = atol (pcValue);
														}
														else
														{
															logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute[%s\\%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_PATTERNS_NAME, CFG_XML_PATTERNS_PATTERN_NAME, CFG_XML_PATTERNS_PATTERN_DEST_NAME, CFG_XML_PATTERNS_PATTERN_DEST_COOLDOWN_NAME);
															ucRet = 0x01;
															goto CleanUp;
														}
													}

													// == Find channel pointer ==
													xml_get_channel (p_pstCfg, pstCfgDest->acID, &pstCfgDest->pstTCP, &pstCfgDest->pstPSTN, &pstCfgDest->pstRS232, &pstCfgDest->pstRS485);

													ucRet = array_add (&pstCfgRoute->stDests, pstCfgDest, pstCfgDest->acID, 0x01, 0);

													if(0x00 !=ucRet)
													{
														logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Array List Attribute[%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_PATTERNS_NAME, CFG_XML_PATTERNS_PATTERN_NAME, CFG_XML_PATTERNS_PATTERN_DEST_NAME);
														ucRet = 0x01;
														goto CleanUp;
													}
													else
													{
														pstCfgDest = 0;
													}
												}

												pstDest = GetNextPeer (pstDest);
											}
										}
									}

									ucRet = array_add (&pstCfgPattern->stRoutes, pstCfgRoute, 0, 0x01, 0);

									if (0x00 != ucRet)
									{
										logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Array List Attribute[%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_PATTERNS_NAME, CFG_XML_PATTERNS_PATTERN_NAME, CFG_XML_PATTERNS_PATTERN_ROUTE_NAME);
										goto CleanUp;
									}
									else
									{
										pstCfgRoute = 0;
									}
								}

								pstRoute = GetNextPeer (pstRoute);
							}
						}

						ucRet = array_add (&p_pstCfg->stPatterns, pstCfgPattern, pstCfgPattern->acID, 0x01, 0);

						if (0x00 != ucRet)
						{
							logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Array List Attribute[%s\\%s]", __LINE__, MOD_CFG_NAME,  CFG_XML_PATTERNS_NAME, CFG_XML_PATTERNS_PATTERN_NAME);
							goto CleanUp;
						}
						else
						{
							pstCfgPattern = 0;
						}
					}
				}

				pstPattern = GetNextPeer (pstPattern);
			}
		}
	}

	ucRet = 0x00;

CleanUp:
	MEM_CLEAN (pstCfgPattern);
	MEM_CLEAN (pstCfgRoute);
	MEM_CLEAN (pstCfgDest);

	return ucRet;
}

unsigned char xml_config_diagnose (stXMLParserMainData *p_pstXML, CFG_CORE_ENGINE *p_pstCfg)
{
	char			*pcValue					= 0;

	stElement		*pstDiagnose;

	pstDiagnose	= FindChildByName (p_pstXML->pstMasElement, CFG_XML_DIAGNOSE_NAME, 0, 0);

  if (0 == pstDiagnose) {
    goto CleanUp;
  }

  p_pstCfg->stDiagnose.ucEnable = 0x01;

	strcpy (p_pstCfg->stDiagnose.acWanPingIP, "4.2.2.2");

	if (0x00 == FindAttributeValue (pstDiagnose->pstAttribute, CFG_XML_DIAGNOSE_WAN_PING_IP_NAME, &pcValue))
		strncpy (p_pstCfg->stDiagnose.acWanPingIP, pcValue, sizeof (p_pstCfg->stDiagnose.acWanPingIP) - 1);

	p_pstCfg->stDiagnose.iRetryThreshold = 3;
	if (0x00 == FindAttributeValue (pstDiagnose->pstAttribute, CFG_XML_DIAGNOSE_RETRY_THRESHOLD_NAME, &pcValue))
  {
  	if (0 <= atoi (pcValue))
  	{
  		p_pstCfg->stDiagnose.iRetryThreshold = atoi (pcValue);
  	}
	}

	p_pstCfg->stDiagnose.ulRetryInterval = 120000;
	if (0x00 == FindAttributeValue (pstDiagnose->pstAttribute, CFG_XML_DIAGNOSE_RETRY_INTERVAL_NAME, &pcValue))
  {
  	if (0 <= atol (pcValue))
  	{
  		p_pstCfg->stDiagnose.ulRetryInterval = atol (pcValue);
  	}
	}


CleanUp:
  return 0x00;
}

unsigned char xml_config_depend (stXMLParserMainData *p_pstXML, CFG_CORE_ENGINE *p_pstCfg)
{
	unsigned char	ucRet;

	char			*pcValue					= 0,
					*pcStart,
					*pcEnd;

	int				iDepend						= 0;

	stElement		*pstElement,
					*pstDepends,
					*pstDepend;

	CFG_DEPEND		*pstCfgDepend				= 0;

	pstElement	= p_pstXML->pstMasElement;
	pstDepends	= FindChildByName (pstElement, CFG_XML_DEPENDENCIES_NAME, 0, 0);
	pstDepend	= FindChildByName (pstDepends, CFG_XML_DEPENDENCIES_DEPENDENCY_NAME, 0, 0);

	if (0 == pstDepends)
	{
		array_init (&p_pstCfg->stDepends, 0, ARRAY_ORDER_DIRECT, 0x00, 0);
	}
	else
	{
		iDepend = GetTotalChild (pstDepends, CFG_XML_DEPENDENCIES_DEPENDENCY_NAME);

		if (0 >= iDepend)
		{
			array_init (&p_pstCfg->stDepends, 0, ARRAY_ORDER_DIRECT, 0x00, 0);
		}
		else
		{
			array_init (&p_pstCfg->stDepends, iDepend, ARRAY_ORDER_DIRECT, 0x00, 0);

			pstDepend = GetNextChild (pstDepends);

			while (0 != pstDepend)
			{
				if (0 == strcmp (pstDepend->pcName, CFG_XML_DEPENDENCIES_DEPENDENCY_NAME))
				{
					MEM_ALLOC (pstCfgDepend, sizeof (CFG_DEPEND), (CFG_DEPEND *));

					if (0x00 == FindAttributeValue (pstDepend->pstAttribute, CFG_XML_DEPENDENCIES_DEPENDENCY_ID_NAME, &pcValue))
					{
						strncpy (pstCfgDepend->acID, pcValue, sizeof (pstCfgDepend->acID) - 1);
					}
					else
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_DEPENDENCIES_NAME, CFG_XML_DEPENDENCIES_DEPENDENCY_NAME, CFG_XML_DEPENDENCIES_DEPENDENCY_ID_NAME);
						ucRet = 0x01;
						goto CleanUp;
					}

					if (0x00 != FindAttributeValue (pstDepend->pstAttribute, CFG_XML_DEPENDENCIES_DEPENDENCY_DEPEND_NAME, &pcValue))
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s]", __LINE__, MOD_CFG_NAME, CFG_XML_DEPENDENCIES_NAME, CFG_XML_DEPENDENCIES_DEPENDENCY_NAME, CFG_XML_DEPENDENCIES_DEPENDENCY_DEPEND_NAME);
						ucRet = 0x01;
						goto CleanUp;
					}
					else
					{
						if (0 != strstr (pcValue, "{IF:"))
						{
							pstCfgDepend->ucType = CFG_DEPEND_TYPE_IF;
						}
						else if (0 != strstr (pcValue, "{HW:"))
						{
							pstCfgDepend->ucType = CFG_DEPEND_TYPE_HW;
						}
						else
						{
							pstCfgDepend->ucType = CFG_DEPEND_TYPE_NORMAL;
						}

						if (CFG_DEPEND_TYPE_IF == pstCfgDepend->ucType || CFG_DEPEND_TYPE_HW == pstCfgDepend->ucType)
						{
							pcStart = strchr (pcValue, ':');

							if (0 != pcStart)
							{
								pcStart++;
								pcEnd = strchr (pcStart, '}');

								if (0 != pcEnd && pcEnd > pcStart)
								{
									strncpy (pstCfgDepend->acDepend, pcStart, pcEnd - pcStart);
								}
								else
								{
									strncpy (pstCfgDepend->acDepend, pcValue, sizeof (pstCfgDepend->acDepend) - 1);
								}
							}
							else
							{
								strncpy (pstCfgDepend->acDepend, pcValue, sizeof (pstCfgDepend->acDepend) - 1);
							}
						}
						else
						{
							strncpy (pstCfgDepend->acDepend, pcValue, sizeof (pstCfgDepend->acDepend) - 1);
						}
					}

					if (0x00 == FindAttributeValue (pstDepend->pstAttribute, CFG_XML_DEPENDENCIES_DEPENDENCY_CMP_NAME, &pcValue))
					{
						if (0 == strcmp (pcValue, "AND"))
						{
							pstCfgDepend->ucCmp = CFG_DEPEND_CMP_AND;
						}
						else if (0 == strcmp (pcValue, "OR"))
						{
							pstCfgDepend->ucCmp = CFG_DEPEND_CMP_OR;
						}
						else
						{
							pstCfgDepend->ucCmp = CFG_DEPEND_CMP_AND;
						}
					}
					else
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Missing Attribute [%s\\%s\\%s]", __LINE__, MOD_CFG_NAME,  CFG_XML_DEPENDENCIES_NAME, CFG_XML_DEPENDENCIES_DEPENDENCY_NAME, CFG_XML_DEPENDENCIES_DEPENDENCY_CMP_NAME);
						ucRet = 0x01;
						goto CleanUp;
					}

					ucRet= array_add (&p_pstCfg->stDepends, pstCfgDepend, pstCfgDepend->acID, 0x01, 0);

					if (0x00 != ucRet)
					{
						logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Array List Attribute[%s\\%s\\%s]", __LINE__, MOD_CFG_NAME,  CFG_XML_DEPENDENCIES_NAME, CFG_XML_DEPENDENCIES_DEPENDENCY_NAME, CFG_XML_DEPENDENCIES_DEPENDENCY_NAME);
						goto CleanUp;
					}
					else
					{
						pstCfgDepend = 0;
					}
				}

				pstDepend = GetNextPeer (pstDepend);
			}
		}
	}

	ucRet = 0x00;

CleanUp:
	MEM_CLEAN (pstCfgDepend);

	return ucRet;
}

void xml_get_channel (CFG_CORE_ENGINE *p_pstCfg, char *p_pcID, CFG_TCP **p_ppstTCP, CFG_PSTN **p_ppstPSTN, CFG_RS232 **p_ppstRS232, CFG_RS485 **p_ppstRS485)
{
	unsigned char	ucRet;

	ARRAY_DATA		*pstItem;

	*p_ppstTCP = 0;
	*p_ppstPSTN = 0;
	*p_ppstRS232 = 0;
	*p_ppstRS485 = 0;

	ucRet = array_get_by_id (&p_pstCfg->stChannels.stTCPs, p_pcID, &pstItem);

	if (0x00 == ucRet && 0 != pstItem)
	{
		*p_ppstTCP = (CFG_TCP *) pstItem->pvData;
		return;
	}

	ucRet = array_get_by_id (&p_pstCfg->stChannels.stRS232s, p_pcID, &pstItem);

	if (0x00 == ucRet && 0 != pstItem)
	{
		*p_ppstRS232 = (CFG_RS232 *) pstItem->pvData;
		return;
	}

	ucRet = array_get_by_id (&p_pstCfg->stChannels.stRS485s, p_pcID, &pstItem);

	if (0x00 == ucRet && 0 != pstItem)
	{
		*p_ppstRS485 = (CFG_RS485 *) pstItem->pvData;
	}

	ucRet = array_get_by_id (&p_pstCfg->stChannels.stPSTNs, p_pcID, &pstItem);

	if (0x00 == ucRet && 0 != pstItem)
	{
		*p_ppstPSTN = (CFG_PSTN *) pstItem->pvData;
	}
}
