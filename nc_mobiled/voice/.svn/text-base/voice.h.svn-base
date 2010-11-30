#ifndef VOICE_H_
#define VOICE_H_

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>

#include "module/mod_optm.h"
#include "hal/arch.h"

#define DEBUG_VOICE

#ifdef DEBUG_VOICE
#define LOG_LEVEL LOG_NORMAL
#else
#define LOG_LEVEL LOG_DISABLE
#endif


extern LOGGER_VOICE		g_stVoiceLog;
extern MOD_OPTM			g_stOPTM;
extern CFG_CORE_ENGINE		g_stCfg;
//extern ARCH                     g_stArch;
extern COREL                    g_stCoRel;

void * VOICE_Thread(void *arg);

#endif
