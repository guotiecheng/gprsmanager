#include "arch.h"
#include "../voice.h"

ARCH			g_stArch;

unsigned char arch_init ()
{
	unsigned char	ucRet,
					ucGot_GTM900B	= 0x00,
					ucGot_G600	  = 0x00,
					ucGot_UC864E	= 0x00,
					ucNetworkMode = 0x00;

	ARCH			*pstArch		= &g_stArch;

	ARRAY_DATA		*pstItem;

//	CFG_MOD_PPP		*pstPPP;

	CFG_MOD_PSTN	*pstPSTNCfg;

	memset (pstArch, 0x00, sizeof (ARCH));

	hal_led_init (&pstArch->stHI.stLED);		// LED
	hal_buzzer_init (&pstArch->stHI.stBuzzer);	// Buzzer
	hal_eth_init (&pstArch->stHI.stEth);		// Ethernet
#if 0
  if (0x01 == g_stCfg.stDiagnose.ucEnable)
    arch_diagnose_init (&pstArch->stDiagnose, pstArch);
#endif
	g_stArch.ucDiagnoseAlert = ARCH_DIAGNOSE_IDLE; /* Move it to the initial section by WENJING*/
	
	// ==================
	// ** Mobile modem **
	// ==================

	ucGot_GTM900B = 0x01;

#if 0
		case CFG_MODEM_ID_G600_CODE:
			ucGot_G600= 0x01;
			break;

		case CFG_MODEM_ID_UC864E_CODE:
			ucGot_UC864E = 0x01;
      ucNetworkMode = HAL_UC864E_NETWORK_MODE_DEFAULT;

      if (pstPPP->stSIM1.acAPN [0])
        ucNetworkMode = (CFG_PPP_NETWORK_TYPE_GPRS == pstPPP->stSIM1.ucNetworkType) ? HAL_UC864E_NETWORK_MODE_GPRS : HAL_UC864E_NETWORK_MODE_HSPDA;
			break;
		}

		ucRet = array_get_next (&g_stCfg.stModules.stPPPs, pstItem, &pstItem);

#endif
	// HUAWEI GTM900B MODEM
	if (0x01 == ucGot_GTM900B)
	{
		hal_gtm900b_init (&pstArch->stMobile.stGTM900B);
	}

	// FIBOCOM G600 MODEM
	if (0x01 == ucGot_G600)
	{
		hal_g600_init (&pstArch->stMobile.stG600);
	}

	// TELIT UC864E MODEM
	if (0x01 == ucGot_UC864E)
	{
		hal_uc864e_init (&pstArch->stMobile.stUC864E, ucNetworkMode);
	}

	// PSTN modem
	ucRet = hal_cx930xx_init (&pstArch->stModem.stCX930XX, g_stCfg.stModules.stPSTNs.iCount);

	if (0x00 != ucRet)
	{
		ucRet = 0x01;
		goto CleanUp;
	}

	ucRet = array_get_first (&g_stCfg.stModules.stPSTNs, &pstItem);

	while (0x00 == ucRet && 0 != pstItem)
	{
		pstPSTNCfg = (CFG_MOD_PSTN *) pstItem->pvData;

		ucRet = hal_cx930xx_add_slot (&pstArch->stModem.stCX930XX, pstPSTNCfg->acID, pstPSTNCfg->acCtrlDrv, pstPSTNCfg->acDataDrv, pstPSTNCfg->acCountryReg);

		if (0x00 != ucRet)
		{
			g_stArch.ucDiagnoseAlert = ARCH_DIAGNOSE_HARDWARE_PSTN_FAIL;
			//ucRet = 0x02;
			//goto CleanUp;
		}

		ucRet = array_get_next (&g_stCfg.stModules.stPSTNs, pstItem, &pstItem);
	}

	thread_init (&pstArch->stThread, pstArch);

	g_stArch.ucInit = 0x01;
	ucRet = 0x00;

CleanUp:

	return ucRet;
}

void arch_term ()
{
	unsigned char			ucGot_GTM900B	= 0x00,
					ucGot_G600    = 0x00,
					ucGot_UC864E	= 0x00;

	ARCH			*pstArch		= &g_stArch;

//	ARRAY_DATA		*pstItem;

//	CFG_MOD_PPP		*pstPPP;

	arch_close (0x01);

	// ==================
	// ** Mobile modem **
	// ==================
#if 0
	ucRet = array_get_first (&g_stCfg.stModules.stPPPs, &pstItem);

	while (0x00 == ucRet && 0 != pstItem)
	{
		pstPPP = (CFG_MOD_PPP *) pstItem->pvData;

		switch (pstPPP->ucModemID)
		{
		case CFG_MODEM_ID_GTM900B_CODE:
			ucGot_GTM900B = 0x01;
			break;

		case CFG_MODEM_ID_G600_CODE:
			ucGot_G600 = 0x01;
			break;

		case CFG_MODEM_ID_UC864E_CODE:
			ucGot_UC864E = 0x01;
			break;
		}

		ucRet = array_get_next (&g_stCfg.stModules.stPPPs, pstItem, &pstItem);
	}
#endif
	ucGot_GTM900B = 0x01;

	// HUAWEI GTM900B MODEM
	if (0x01 == ucGot_GTM900B)
	{
		hal_gtm900b_term (&pstArch->stMobile.stGTM900B);
	}

	// FIBOCOM G600 MODEM
	if (0x01 == ucGot_G600)
	{
		hal_g600_term (&pstArch->stMobile.stG600);
	}

	// TELIT UC864E MODEM
	if (0x01 == ucGot_UC864E)
	{
		hal_uc864e_term (&pstArch->stMobile.stUC864E);
	}

	// PSTN modem
	if (0 < pstArch->stModem.stCX930XX.stSlots.iCount)
	{
		hal_cx930xx_term (&pstArch->stModem.stCX930XX);
	}

	hal_led_term (&pstArch->stHI.stLED);	// LED
	hal_buzzer_term (&pstArch->stHI.stBuzzer);	// Buzzer
	hal_eth_term (&pstArch->stHI.stEth);	// Ethernet
#if 0
  if (0x01 == g_stCfg.stDiagnose.ucEnable)
    arch_diagnose_term (&pstArch->stDiagnose);
#endif
	pstArch->ucInit = 0x00;
}

unsigned char arch_start ()
{
	unsigned char	ucRet,
					ucGot_GTM900B	= 0x00,
					ucGot_G600  	= 0x00,
					ucGot_UC864E	= 0x00;

	ARCH			*pstArch		= &g_stArch;

//	ARRAY_DATA		*pstItem;

//	CFG_MOD_PPP		*pstPPP;

	if (0x01 != pstArch->ucInit)
	{
		ucRet = 0x01;
		goto CleanUp;
	}

	// ==================
	// ** Mobile modem **
	// ==================
#if 0
	ucRet = array_get_first (&g_stCfg.stModules.stPPPs, &pstItem);

	while (0x00 == ucRet && 0 != pstItem)
	{
		pstPPP = (CFG_MOD_PPP *) pstItem->pvData;

		switch (pstPPP->ucModemID)
		{
		case CFG_MODEM_ID_GTM900B_CODE:
			ucGot_GTM900B = 0x01;
			break;

		case CFG_MODEM_ID_G600_CODE:
			ucGot_G600 = 0x01;
			break;

		case CFG_MODEM_ID_UC864E_CODE:
			ucGot_UC864E = 0x01;
			break;
		}

		ucRet = array_get_next (&g_stCfg.stModules.stPPPs, pstItem, &pstItem);
	}
#endif
	ucGot_GTM900B = 0x01;

	// HUAWEI GTM900B MODEM
	if (0x01 == ucGot_GTM900B)
	{
		ucRet = hal_gtm900b_start (&pstArch->stMobile.stGTM900B);

		if (0x00 != ucRet)
		{
			ucRet = 0x02;
			goto CleanUp;
		}
	}

	// FIBOCOM G600 MODEM
	if (0x01 == ucGot_G600)
	{
		ucRet = hal_g600_start (&pstArch->stMobile.stG600);

		if (0x00 != ucRet)
		{
			ucRet = 0x02;
			goto CleanUp;
		}
	}

	// TELIT UC864E MODEM
	if (0x01 == ucGot_UC864E)
	{
		ucRet = hal_uc864e_start (&pstArch->stMobile.stUC864E);

		if (0x00 != ucRet)
		{
			ucRet = 0x03;
			goto CleanUp;
		}
	}

	// PSTN modem
	if (0 < pstArch->stModem.stCX930XX.stSlots.iCount)
	{
		ucRet = hal_cx930xx_start (&pstArch->stModem.stCX930XX);

		if (0x00 != ucRet)
		{
			ucRet = 0x04;
			goto CleanUp;
		}
	}
#if 0
  if (0x01 == g_stCfg.stDiagnose.ucEnable)
  {
    ucRet = arch_diagnose_start (&pstArch->stDiagnose);
  	if (0x00 != ucRet)
  	{
  		ucRet = 0x05;
  		goto CleanUp;
  	}
  }
#endif

//	ucRet = thread_start (&pstArch->stThread, arch_body);

CleanUp:

	return ucRet;
}

void arch_close (unsigned char p_ucWait)
{
	unsigned char			ucGot_GTM900B	= 0x00,
					ucGot_G600    = 0x00,
					ucGot_UC864E	= 0x00;

	ARCH			*pstArch		= &g_stArch;

//	ARRAY_DATA		*pstItem;

//	CFG_MOD_PPP		*pstPPP;

	if (0x01 != pstArch->ucInit)
	{
		return;
	}

	// ==================
	// ** Mobile modem **
	// ==================
#if 0
	ucRet = array_get_first (&g_stCfg.stModules.stPPPs, &pstItem);

	while (0x00 == ucRet && 0 != pstItem)
	{
		pstPPP = (CFG_MOD_PPP *) pstItem->pvData;

		switch (pstPPP->ucModemID)
		{
		case CFG_MODEM_ID_GTM900B_CODE:
			ucGot_GTM900B = 0x01;
			break;

		case CFG_MODEM_ID_G600_CODE:
			ucGot_G600= 0x01;
			break;

		case CFG_MODEM_ID_UC864E_CODE:
			ucGot_UC864E = 0x01;
			break;
		}

		ucRet = array_get_next (&g_stCfg.stModules.stPPPs, pstItem, &pstItem);
	}
#endif
	ucGot_GTM900B = 0x01;

	// HUAWEI GTM900B MODEM
	if (0x01 == ucGot_GTM900B)
	{
		hal_gtm900b_close (&pstArch->stMobile.stGTM900B);
	}

	// FIBOCOM G600 MODEM
	if (0x01 == ucGot_G600)
	{
		hal_g600_close (&pstArch->stMobile.stG600);
	}

	// TELIT UC864E MODEM
	if (0x01 == ucGot_UC864E)
	{
		hal_uc864e_close (&pstArch->stMobile.stUC864E);
	}

	// PSTN modem
	if (0 < pstArch->stModem.stCX930XX.stSlots.iCount)
	{
		hal_cx930xx_close (&pstArch->stModem.stCX930XX);
	}

	thread_close (&pstArch->stThread, SIGINT, 30000);
}
