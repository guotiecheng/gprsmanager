#ifndef HAL_ETH_H
#define HAL_ETH_H

#include <stdio.h>

#if defined (WIN32)

	#include <windows.h>

#elif defined (LINUX) || defined (UCLINUX)

	#include <unistd.h>
	#include <stdlib.h>
	#include <fcntl.h>
	#include <signal.h>
	#include <sys/types.h>
	#include <sys/wait.h>
	#include <errno.h>

#endif

#include "../local.h"

#define HAL_ETH_NAME										"ETHERNET  "


#if defined (ARCH_L100) || defined (ARCH_L200) || defined (ARCH_N300)

	#define HAL_ETH_CTRL_DEV_FILE							"/dev/eth"

#elif defined (ARCH_L300) || defined (ARCH_L350)

	#define HAL_ETH_CTRL_DEV_FILE							"/dev/detect"

#else

	#define HAL_ETH_CTRL_DEV_FILE							""

#endif

typedef struct HAL_ETH_T
{
  unsigned char ucDepFailed_eth0, ucDepFailed_eth1;
	COM_PORT_VOICE		stCtrlPort;
}
HAL_ETH;

unsigned char hal_eth_init (HAL_ETH *p_pstHAL);
void hal_eth_term (HAL_ETH *p_pstHAL);

unsigned char hal_eth_link (HAL_ETH *p_pstHAL, unsigned char p_ucSlot);

#endif
