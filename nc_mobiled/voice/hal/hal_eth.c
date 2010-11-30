#include "hal_eth.h"
#include "../voice.h"

unsigned char hal_eth_init (HAL_ETH *p_pstHAL)
{
	unsigned char	ucRet;

	memset (p_pstHAL, 0x00, sizeof (HAL_ETH));

  p_pstHAL->ucDepFailed_eth0 = 0xFF;
  p_pstHAL->ucDepFailed_eth1 = 0xFF;

	comport_init_voice (&p_pstHAL->stCtrlPort, HAL_ETH_CTRL_DEV_FILE, 0, 0, 0, 0, 0, 0, 0, 0, 0);

	ucRet = comport_open_voice (&p_pstHAL->stCtrlPort);

	return ucRet;
}

void hal_eth_term (HAL_ETH *p_pstHAL)
{
	comport_term_voice (&p_pstHAL->stCtrlPort);
	memset (p_pstHAL, 0x00, sizeof (HAL_ETH));
}

unsigned char hal_eth_link (HAL_ETH *p_pstHAL, unsigned char p_ucSlot)
{
	unsigned char	ucRet	= 0x00;

	#if defined (ARCH_L100) || defined (ARCH_L200) || defined (ARCH_N300)

		if (0 < p_pstHAL->stCtrlPort.hComPort)
		{
			if (1 != ioctl (p_pstHAL->stCtrlPort.hComPort, 1, 0))
			{
				ucRet = 0x01;
			}
		}

	#elif defined (ARCH_L300) || defined (ARCH_L350)

		int		aiState [32]		= { 0 },
				iState				= 0;

		if (0 < p_pstHAL->stCtrlPort.hComPort)
		{
			if (0 == ioctl (p_pstHAL->stCtrlPort.hComPort, 24, &aiState))
			{
				switch (p_ucSlot)
				{
				case 0x00:	iState = aiState [1]; break;
				case 0x01:	iState = aiState [2]; break;
				}

				if (1 != iState)
				{
					ucRet = 0x01;	// No link
				}
			}
		}

	#endif

	return ucRet;
}
