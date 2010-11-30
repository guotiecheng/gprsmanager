#include <voice.h>

ARCH                            g_stArch;

COREL                           g_stCoRel;
MOD_OPTM                        g_stOPTM;
CFG_CORE_ENGINE         	g_stCfg;
LOGGER_VOICE			g_stVoiceLog;

void * VOICE_Thread(void *arg)
{
	int iRet;
	unsigned char ucRet,
		   ucDebug		= 0x00;
 	char *acCfgFile = "/apps/etc/mobiled_voice_cfg.xml";


	logger_init_voice (&g_stVoiceLog, SYS_LOG_DEF_PATH, LOG_LEVEL, 256000, 0x01, ucDebug);

	ucRet = xml_config_init (acCfgFile, &g_stCfg);
	if (0x00 != ucRet)
	{
		logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s load configuration failure. RetCode [0x%02X]", __LINE__, "Mobiled_voice", ucRet);
		printf ("%s load configuration failure with retcode [0x%02X]. Please refer to %s\n", "Mobiled_voice", ucRet, SYS_LOG_DEF_PATH);
		iRet = 1;
		goto CleanUp;
	}

	mod_optm_init (&g_stOPTM);

	array_init (&g_stCoRel.stCoRels, SYS_COREL_MAX, ARRAY_ORDER_DIRECT, 0x00, 0);
	critical_init (&g_stCoRel.stLock);
	g_stCoRel.iPackSeqNo = 1;

	ucRet = mod_optm_start (&g_stOPTM);
	if (0x00 == ucRet)
	{
		logger_log (&g_stVoiceLog, LOG_DETAIL, "@%04d %s: Started optimus services", __LINE__, "Mobiled_Voice");
	}
	else if (0xFF != ucRet)
	{
		logger_log (&g_stVoiceLog, LOG_ERROR, "@%04d %s: Start optimus service failure. RetCode [0x%02X]", __LINE__, "Mobiled_Voice", ucRet);
		iRet = 2;
		goto CleanUp;
	}

	while(1)
	{
		sleep(1);
	}

CleanUp:

	critical_lock (&g_stCoRel.stLock, 0x01);
	array_term (&g_stCoRel.stCoRels);
	critical_unlock (&g_stCoRel.stLock);
	critical_term (&g_stCoRel.stLock);

	logger_term_voice (&g_stVoiceLog);
	
	mod_optm_term (&g_stOPTM);

	xml_config_term (&g_stCfg);

	return ((void *)0);
}
