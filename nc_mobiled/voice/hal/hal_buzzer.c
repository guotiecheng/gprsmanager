#include "hal_buzzer.h"
#include "../voice.h"

unsigned char hal_buzzer_init (HAL_BUZZER *p_pstHAL)
{
	memset (p_pstHAL, 0x00, sizeof (HAL_BUZZER));

	comport_init_voice (&p_pstHAL->stCtrlPort, HAL_BUZZER_CTRL_DEV_FILE, 0, 0, 0, 0, 0, 0, 0, 0, 0);

	if (CFG_COMMON_YES == g_stCfg.stModules.ucBuzzer)
	{
		comport_open_voice (&p_pstHAL->stCtrlPort);

		hal_buzzer_off (p_pstHAL);
	}

	return 0x00;
}

void hal_buzzer_term (HAL_BUZZER *p_pstHAL)
{
	hal_buzzer_off (p_pstHAL);

	comport_term_voice (&p_pstHAL->stCtrlPort);
	memset (p_pstHAL, 0x00, sizeof (HAL_BUZZER));
}

void _hal_buzzer_on (HAL_BUZZER *p_pstHAL, unsigned char p_ucSetFlag)
{
	if (0 < p_pstHAL->stCtrlPort.hComPort)
	{
		if (0x01 == p_ucSetFlag)
		{
			memset (&p_pstHAL->stBuzzer [0], 0x00, sizeof (p_pstHAL->stBuzzer [0]));
			memset (&p_pstHAL->stBuzzer [1], 0x00, sizeof (p_pstHAL->stBuzzer [1]));

			p_pstHAL->stBuzzer [0].ucState = HAL_BUZZER_STATE_ON;
			p_pstHAL->stBuzzer [0].ucStatus = HAL_BUZZER_STATUS_ON;
		}

		#if defined (ARCH_L100) || defined (ARCH_L200)

			ioctl (p_pstHAL->stCtrlPort.hComPort, 1, 0);

		#elif defined (ARCH_L300) || defined (ARCH_L350) || defined (ARCH_N300) || defined (ARCH_L300B)

			ioctl (p_pstHAL->stCtrlPort.hComPort, 15, 0);

		#endif
	}
}

void _hal_buzzer_off (HAL_BUZZER *p_pstHAL, unsigned char p_ucSetFlag)
{
	if (0 < p_pstHAL->stCtrlPort.hComPort)
	{
		if (0x01 == p_ucSetFlag)
		{
			memset (&p_pstHAL->stBuzzer [0], 0x00, sizeof (p_pstHAL->stBuzzer [0]));
			memset (&p_pstHAL->stBuzzer [1], 0x00, sizeof (p_pstHAL->stBuzzer [1]));

			p_pstHAL->stBuzzer [0].ucState = HAL_BUZZER_STATE_OFF;
			p_pstHAL->stBuzzer [0].ucStatus = HAL_BUZZER_STATUS_OFF;
		}

		#if defined (ARCH_L100) || defined (ARCH_L200)

			ioctl (p_pstHAL->stCtrlPort.hComPort, 0, 0);

		#elif defined (ARCH_L300) || defined (ARCH_L350) || defined (ARCH_N300) || defined (ARCH_L300B)

			ioctl (p_pstHAL->stCtrlPort.hComPort, 16, 0);

		#endif
	}
}

void hal_buzzer_on (HAL_BUZZER *p_pstHAL)
{
	_hal_buzzer_on (p_pstHAL, 0x01);
}

void hal_buzzer_off (HAL_BUZZER *p_pstHAL)
{
	_hal_buzzer_off (p_pstHAL, 0x01);
}

void hal_buzzer (HAL_BUZZER *p_pstHAL, unsigned long p_ulPause, unsigned long p_ulGap, unsigned long p_ulRest, unsigned char p_ucCount,
				 unsigned char p_ucRepeat, unsigned char p_ucState, unsigned long p_ulTrgPoint)
{
	if (0x00 == p_pstHAL->stBuzzer [1].ucIsUsed || p_ulTrgPoint != p_pstHAL->stBuzzer [1].ulTrgPoint)
	{
		p_pstHAL->stBuzzer [1].ulPause = p_ulPause;
		p_pstHAL->stBuzzer [1].ulGap = p_ulGap;
		p_pstHAL->stBuzzer [1].ulRest = p_ulRest;
		p_pstHAL->stBuzzer [1].ucCount = p_ucCount;
		p_pstHAL->stBuzzer [1].ucRepeat = p_ucRepeat;

		p_pstHAL->stBuzzer [1].ucState = p_ucState;
		p_pstHAL->stBuzzer [1].ulLastTime = 0;
		p_pstHAL->stBuzzer [1].ulExpiry = 0;

		if (p_ulTrgPoint != p_pstHAL->stBuzzer [1].ulTrgPoint)
		{
			p_pstHAL->stBuzzer [1].ulTrgPoint = p_ulTrgPoint;
			p_pstHAL->stBuzzer [1].ucInterrupt = 0x01;
		}
		else
		{
			p_pstHAL->stBuzzer [1].ucInterrupt = 0x00;
		}

		p_pstHAL->stBuzzer [1].ucIsUsed = 0x01;
	}
}

unsigned char hal_buzzer_main (HAL_BUZZER *p_pstHAL)
{
	unsigned char	ucCache;

	unsigned long	ulCurTime;

	// Interrupt
	if (0x01 == p_pstHAL->stBuzzer [1].ucIsUsed)
	{
		if (0x01 == p_pstHAL->stBuzzer [1].ucInterrupt || HAL_BUZZER_STATE_ON == p_pstHAL->stBuzzer [1].ucState || HAL_BUZZER_STATE_OFF == p_pstHAL->stBuzzer [1].ucState)
		{
			ucCache = p_pstHAL->stBuzzer [0].ucStatus;

			memcpy (&p_pstHAL->stBuzzer [0], &p_pstHAL->stBuzzer [1], sizeof (HAL_BUZZER_SLOT));

			p_pstHAL->stBuzzer [0].ucStatus = ucCache;

			p_pstHAL->stBuzzer [0].ucIsUsed = 0x01;
			p_pstHAL->stBuzzer [1].ucIsUsed = 0x00;
			p_pstHAL->stBuzzer [0].ucInterrupt = 0x00;

			if (0x00 != p_pstHAL->stBuzzer [0].ucRepeat && 0x00 != p_pstHAL->stBuzzer [0].ucCount)
			{
				p_pstHAL->stBuzzer [0].ucRepeat = p_pstHAL->stBuzzer [0].ucCount;
			}
		}
	}

	IGET_TICKCOUNT (ulCurTime);

	if (0x01 == p_pstHAL->stBuzzer [0].ucIsUsed)
	{
		p_pstHAL->stBuzzer [0].ucIsUsed = 0x00;

		p_pstHAL->stBuzzer [0].ulLastTime = ulCurTime;
		p_pstHAL->stBuzzer [0].ulExpiry = ulCurTime;

		if (HAL_BUZZER_STATE_IDLE == p_pstHAL->stBuzzer [0].ucState)
		{
			if (0 != p_pstHAL->stBuzzer [0].ulPause)
			{
				if (HAL_BUZZER_STATUS_OFF == p_pstHAL->stBuzzer [0].ucStatus)
				{
					p_pstHAL->stBuzzer [0].ucStatus = HAL_BUZZER_STATUS_ON;
					_hal_buzzer_on (p_pstHAL, 0x00);
				}

				p_pstHAL->stBuzzer [0].ulExpiry += p_pstHAL->stBuzzer [0].ulPause;
			}
			else
			{
				p_pstHAL->stBuzzer [0].ucStatus = HAL_BUZZER_STATUS_ON;
				_hal_buzzer_on (p_pstHAL, 0x00);
			}

			if (0 != p_pstHAL->stBuzzer [0].ucCount)
			{
				(p_pstHAL->stBuzzer [0].ucCount)--;
			}

			p_pstHAL->stBuzzer [0].ucState = HAL_BUZZER_STATE_PAUSE;
		}
		else if (HAL_BUZZER_STATE_PAUSE == p_pstHAL->stBuzzer [0].ucState)
		{
			if (0 != p_pstHAL->stBuzzer [0].ulGap)
			{
				if (HAL_BUZZER_STATUS_ON == p_pstHAL->stBuzzer [0].ucStatus)
				{
					p_pstHAL->stBuzzer [0].ucStatus = HAL_BUZZER_STATUS_OFF;
					_hal_buzzer_off (p_pstHAL, 0x00);
				}

				p_pstHAL->stBuzzer [0].ulExpiry += p_pstHAL->stBuzzer [0].ulGap;
			}

			p_pstHAL->stBuzzer [0].ucState = HAL_BUZZER_STATE_GAP;
		}
		else if (HAL_BUZZER_STATE_GAP == p_pstHAL->stBuzzer [0].ucState)
		{
			if (0 != p_pstHAL->stBuzzer [0].ulRest && 0x00 == p_pstHAL->stBuzzer [0].ucCount)
			{
				p_pstHAL->stBuzzer [0].ulExpiry += p_pstHAL->stBuzzer [0].ulRest;
			}

			p_pstHAL->stBuzzer [0].ucState = HAL_BUZZER_STATE_REST;
		}
		else
		{
			if (HAL_BUZZER_STATE_ON == p_pstHAL->stBuzzer [0].ucState)
			{
				if (HAL_BUZZER_STATUS_OFF == p_pstHAL->stBuzzer [0].ucStatus)
				{
					p_pstHAL->stBuzzer [0].ucStatus = HAL_BUZZER_STATUS_ON;
					_hal_buzzer_on (p_pstHAL, 0x00);
				}
			}
			else if (HAL_BUZZER_STATE_OFF == p_pstHAL->stBuzzer [0].ucState)
			{
				if (HAL_BUZZER_STATUS_ON == p_pstHAL->stBuzzer [0].ucStatus)
				{
					p_pstHAL->stBuzzer [0].ucStatus = HAL_BUZZER_STATUS_OFF;
					_hal_buzzer_off (p_pstHAL, 0x00);
				}
			}

			if (0x00 == p_pstHAL->stBuzzer [0].ucCount)
			{
				// Cache the next state
				if (0x01 == p_pstHAL->stBuzzer [1].ucIsUsed)
				{
					ucCache = p_pstHAL->stBuzzer [0].ucStatus;

					memcpy (&p_pstHAL->stBuzzer [0], &p_pstHAL->stBuzzer [1], sizeof (HAL_BUZZER_SLOT));
					p_pstHAL->stBuzzer [1].ucIsUsed = 0x00;

					p_pstHAL->stBuzzer [0].ucStatus = ucCache;
					p_pstHAL->stBuzzer [0].ucInterrupt = 0x00;

					if (0x00 != p_pstHAL->stBuzzer [0].ucRepeat && 0x00 != p_pstHAL->stBuzzer [0].ucCount)
					{
						p_pstHAL->stBuzzer [0].ucRepeat = p_pstHAL->stBuzzer [0].ucCount;
					}
				}
				else if (0x00 != p_pstHAL->stBuzzer [0].ucRepeat)
				{
					p_pstHAL->stBuzzer [0].ucState = HAL_BUZZER_STATE_IDLE;
					p_pstHAL->stBuzzer [0].ucCount = p_pstHAL->stBuzzer [0].ucRepeat;
				}
			}
			else
			{
				if (0 != p_pstHAL->stBuzzer [0].ucCount)
				{
					(p_pstHAL->stBuzzer [0].ucCount)--;
				}

				p_pstHAL->stBuzzer [0].ucState = HAL_BUZZER_STATE_IDLE;
			}
		}
	}

	if (ulCurTime >= p_pstHAL->stBuzzer [0].ulExpiry || ulCurTime < p_pstHAL->stBuzzer [0].ulLastTime ||
		0 == p_pstHAL->stBuzzer [0].ulExpiry || 0 == p_pstHAL->stBuzzer [0].ulLastTime)
	{
		p_pstHAL->stBuzzer [0].ucIsUsed = 0x01;
	}

	return 0x00;
}
