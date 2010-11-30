#include "hal_led.h"

unsigned char hal_led_init (HAL_LED *p_pstHAL)
{
	unsigned char	ucIndex;

	memset (p_pstHAL, 0x00, sizeof (HAL_LED));

	for (ucIndex = 0; HAL_LED_MAX > ucIndex; ucIndex++)
	{
		p_pstHAL->astLED [ucIndex][0].ucIndex = ucIndex;
		p_pstHAL->astLED [ucIndex][1].ucIndex = ucIndex;
	}

	comport_init_voice (&p_pstHAL->stCtrlPort, HAL_LED_CTRL_DEV_FILE, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	comport_open_voice (&p_pstHAL->stCtrlPort);

	for (ucIndex = 0x00; HAL_LED_MAX > ucIndex; ucIndex++)
	{
		hal_led_off (p_pstHAL, ucIndex);
	}

	return 0x00;
}

void hal_led_term (HAL_LED *p_pstHAL)
{
	unsigned char	ucIndex;

	for (ucIndex = 0x00; HAL_LED_MAX > ucIndex; ucIndex++)
	{
		hal_led_off (p_pstHAL, ucIndex);
	}

	comport_term_voice (&p_pstHAL->stCtrlPort);
	memset (p_pstHAL, 0x00, sizeof (HAL_LED));
}

void _hal_led_on (HAL_LED *p_pstHAL, unsigned char p_ucSlot, unsigned char p_ucSetFlag)
{
	if (0 < p_pstHAL->stCtrlPort.hComPort)
	{
		#if defined (ARCH_L100) || defined (ARCH_L200)

			switch (p_ucSlot)
			{
			case 0x00:
				if (0x01 == p_ucSetFlag)
				{
					memset (&p_pstHAL->astLED [p_ucSlot][0], 0x00, sizeof (p_pstHAL->astLED [p_ucSlot][0]));
					memset (&p_pstHAL->astLED [p_ucSlot][1], 0x00, sizeof (p_pstHAL->astLED [p_ucSlot][1]));

					p_pstHAL->astLED [p_ucSlot][0].ucState = HAL_LED_STATE_ON;
					p_pstHAL->astLED [p_ucSlot][0].ucStatus = HAL_LED_STATUS_ON;
				}

				ioctl (p_pstHAL->stCtrlPort.hComPort, 6, 0);
				break;

			case 0xFF:
				if (0x01 == p_ucSetFlag)
				{
					memset (&p_pstHAL->astLED [0][0], 0x00, sizeof (p_pstHAL->astLED [0][0]));
					memset (&p_pstHAL->astLED [0][1], 0x00, sizeof (p_pstHAL->astLED [0][1]));

					p_pstHAL->astLED [0][0].ucState = HAL_LED_STATE_ON;
					p_pstHAL->astLED [0][0].ucStatus = HAL_LED_STATUS_ON;
				}

				ioctl (p_pstHAL->stCtrlPort.hComPort, 5, 0);	// Run LED blink
				break;

			case 0x01:
			case 0x02:
			case 0x03:
			case 0x04:
			case 0x05:
				if (0x01 == p_ucSetFlag)
				{
					memset (&p_pstHAL->astLED [p_ucSlot][0], 0x00, sizeof (p_pstHAL->astLED [p_ucSlot][0]));
					memset (&p_pstHAL->astLED [p_ucSlot][1], 0x00, sizeof (p_pstHAL->astLED [p_ucSlot][1]));

					p_pstHAL->astLED [p_ucSlot][0].ucState = HAL_LED_STATE_ON;
					p_pstHAL->astLED [p_ucSlot][0].ucStatus = HAL_LED_STATUS_ON;
				}

				ioctl (p_pstHAL->stCtrlPort.hComPort, 1, p_ucSlot - 0x01);
				break;
			}

		#elif defined (ARCH_L300) || defined (ARCH_L350)

			switch (p_ucSlot)
			{
			case 0x00:
			case 0x01:
			case 0x02:
			case 0x03:
			case 0x04:
			case 0x05:
			case 0x06:
			case 0x07:
				if (0x01 == p_ucSetFlag)
				{
					memset (&p_pstHAL->astLED [p_ucSlot][0], 0x00, sizeof (p_pstHAL->astLED [p_ucSlot][0]));
					memset (&p_pstHAL->astLED [p_ucSlot][1], 0x00, sizeof (p_pstHAL->astLED [p_ucSlot][1]));

					p_pstHAL->astLED [p_ucSlot][0].ucState = HAL_LED_STATE_ON;
					p_pstHAL->astLED [p_ucSlot][0].ucStatus = HAL_LED_STATUS_ON;
				}

				ioctl (p_pstHAL->stCtrlPort.hComPort, p_ucSlot, 0);
				break;
			}

		#elif defined (ARCH_L300B)

			switch (p_ucSlot)
			{
			case 0x00:
			case 0x01:
			case 0x02:
			case 0x03:
				if (0x01 == p_ucSetFlag)
				{
					memset (&p_pstHAL->astLED [p_ucSlot][0], 0x00, sizeof (p_pstHAL->astLED [p_ucSlot][0]));
					memset (&p_pstHAL->astLED [p_ucSlot][1], 0x00, sizeof (p_pstHAL->astLED [p_ucSlot][1]));

					p_pstHAL->astLED [p_ucSlot][0].ucState = HAL_LED_STATE_ON;
					p_pstHAL->astLED [p_ucSlot][0].ucStatus = HAL_LED_STATUS_ON;
				}

				ioctl (p_pstHAL->stCtrlPort.hComPort, 1, p_ucSlot);
				break;
			}

		#elif defined (ARCH_N300)

			switch (p_ucSlot)
			{
			/* Here 18 Leds at all in AllInOne card */
			case 0x00:
			case 0x01:
			case 0x02:
			case 0x03:
			case 0x04:
			case 0x05:
			case 0x06:
			case 0x07:
			case 0x08:
			case 0x09:
			case 0x0a:
			case 0x0b:
			case 0x0c:
			case 0x0d:
			case 0x0e:
			case 0x0f:
			case 0x10:
			case 0x11:			
				if (0x01 == (p_ucSetFlag & 0x01))
				{
					memset (&p_pstHAL->astLED [p_ucSlot][0], 0x00, sizeof (p_pstHAL->astLED [p_ucSlot][0]));
					memset (&p_pstHAL->astLED [p_ucSlot][1], 0x00, sizeof (p_pstHAL->astLED [p_ucSlot][1]));

					p_pstHAL->astLED [p_ucSlot][0].ucState = HAL_LED_STATE_ON;
					p_pstHAL->astLED [p_ucSlot][0].ucStatus = HAL_LED_STATUS_ON;
				}

				if ((0x08 == (p_ucSetFlag & 0x08)))
                {
                    ioctl (p_pstHAL->stCtrlPort.hComPort, CMD_BLINK, p_ucSlot);
                    //logger_log (&g_stLog, LOG_ALL, "@%04d %s: LED_0x%02x_Blink", __LINE__, HAL_LED_NAME, p_ucSlot);

                    memset (&p_pstHAL->astLED [p_ucSlot][0], 0x00, sizeof (p_pstHAL->astLED [p_ucSlot][0]));
                    memset (&p_pstHAL->astLED [p_ucSlot][1], 0x00, sizeof (p_pstHAL->astLED [p_ucSlot][1]));

                    p_pstHAL->astLED [p_ucSlot][0].ucState = HAL_LED_STATE_OFF;
                    p_pstHAL->astLED [p_ucSlot][0].ucStatus = HAL_LED_STATUS_OFF;
                }
                else if ((0x06 == (p_ucSetFlag & 0x06)))
                {
                    ioctl (p_pstHAL->stCtrlPort.hComPort, CMD_RED, p_ucSlot);
                    //logger_log (&g_stLog, LOG_ALL, "@%04d %s: LED_0x%02x_Red_ON", __LINE__, HAL_LED_NAME, p_ucSlot);
                }
                else if ((0x04 == (p_ucSetFlag & 0x04)))
                {
                    ioctl (p_pstHAL->stCtrlPort.hComPort, CMD_GREEN, p_ucSlot);
                    //logger_log (&g_stLog, LOG_ALL, "@%04d %s: LED_0x%02x_Green_ON", __LINE__, HAL_LED_NAME, p_ucSlot);
                }
                else if((0x02 == (p_ucSetFlag & 0x02)))
                {
                    ioctl (p_pstHAL->stCtrlPort.hComPort, CMD_YELLOW, p_ucSlot);
                    //logger_log (&g_stLog, LOG_ALL, "@%04d %s: LED_0x%02x_Yellow_ON", __LINE__, HAL_LED_NAME, p_ucSlot);
                }
                else
                {
                    ioctl (p_pstHAL->stCtrlPort.hComPort, CMD_ON, p_ucSlot);
                    //if(0x0a != p_ucSlot)logger_log (&g_stLog, LOG_ALL, "@%04d %s: LED_0x%02x_ON", __LINE__, HAL_LED_NAME, p_ucSlot);
                }
                    
				
				break;
			}
		#endif
	}
}

void _hal_led_off (HAL_LED *p_pstHAL, unsigned char p_ucSlot, unsigned char p_ucSetFlag)
{
	if (0 < p_pstHAL->stCtrlPort.hComPort)
	{
		#if defined (ARCH_L100) || defined (ARCH_L200)

			switch (p_ucSlot)
			{
			case 0x00:
				if (0x01 == p_ucSetFlag)
				{
					memset (&p_pstHAL->astLED [p_ucSlot][0], 0x00, sizeof (p_pstHAL->astLED [p_ucSlot][0]));
					memset (&p_pstHAL->astLED [p_ucSlot][1], 0x00, sizeof (p_pstHAL->astLED [p_ucSlot][1]));

					p_pstHAL->astLED [p_ucSlot][0].ucState = HAL_LED_STATE_OFF;
					p_pstHAL->astLED [p_ucSlot][0].ucStatus = HAL_LED_STATUS_OFF;
				}

				ioctl (p_pstHAL->stCtrlPort.hComPort, 4, 0);
				break;

			case 0xFF:
				if (0x01 == p_ucSetFlag)
				{
					memset (&p_pstHAL->astLED [0][0], 0x00, sizeof (p_pstHAL->astLED [0][0]));
					memset (&p_pstHAL->astLED [0][1], 0x00, sizeof (p_pstHAL->astLED [0][1]));

					p_pstHAL->astLED [0][0].ucState = HAL_LED_STATE_OFF;
					p_pstHAL->astLED [0][0].ucStatus = HAL_LED_STATUS_OFF;
				}

				ioctl (p_pstHAL->stCtrlPort.hComPort, 4, 0);
				break;

			case 0x01:
			case 0x02:
			case 0x03:
			case 0x04:
			case 0x05:
				if (0x01 == p_ucSetFlag)
				{
					memset (&p_pstHAL->astLED [p_ucSlot][0], 0x00, sizeof (p_pstHAL->astLED [p_ucSlot][0]));
					memset (&p_pstHAL->astLED [p_ucSlot][1], 0x00, sizeof (p_pstHAL->astLED [p_ucSlot][1]));

					p_pstHAL->astLED [p_ucSlot][0].ucState = HAL_LED_STATE_OFF;
					p_pstHAL->astLED [p_ucSlot][0].ucStatus = HAL_LED_STATUS_OFF;
				}

				ioctl (p_pstHAL->stCtrlPort.hComPort, 0, p_ucSlot - 0x01);
			}

		#elif defined (ARCH_L300) || defined (ARCH_L350)

			switch (p_ucSlot)
			{
			case 0x00:
			case 0x01:
			case 0x02:
			case 0x03:
			case 0x04:
			case 0x05:
			case 0x06:
			case 0x07:
				if (0x01 == p_ucSetFlag)
				{
					memset (&p_pstHAL->astLED [p_ucSlot][0], 0x00, sizeof (p_pstHAL->astLED [p_ucSlot][0]));
					memset (&p_pstHAL->astLED [p_ucSlot][1], 0x00, sizeof (p_pstHAL->astLED [p_ucSlot][1]));

					p_pstHAL->astLED [p_ucSlot][0].ucState = HAL_LED_STATE_OFF;
					p_pstHAL->astLED [p_ucSlot][0].ucStatus = HAL_LED_STATUS_OFF;
				}

				ioctl (p_pstHAL->stCtrlPort.hComPort, p_ucSlot, 1);
				break;
			}

		#elif defined (ARCH_L300B)

			switch (p_ucSlot)
			{
			case 0x00:
			case 0x01:
			case 0x02:
			case 0x03:
				if (0x01 == p_ucSetFlag)
				{
					memset (&p_pstHAL->astLED [p_ucSlot][0], 0x00, sizeof (p_pstHAL->astLED [p_ucSlot][0]));
					memset (&p_pstHAL->astLED [p_ucSlot][1], 0x00, sizeof (p_pstHAL->astLED [p_ucSlot][1]));

					p_pstHAL->astLED [p_ucSlot][0].ucState = HAL_LED_STATE_OFF;
					p_pstHAL->astLED [p_ucSlot][0].ucStatus = HAL_LED_STATUS_OFF;
				}

				ioctl (p_pstHAL->stCtrlPort.hComPort, 0, p_ucSlot);
				break;
			}

		#elif defined (ARCH_N300)

			switch (p_ucSlot)
			{
			/* Here 18 Leds at all in AllInOne card */
			case 0x00:
			case 0x01:
			case 0x02:
			case 0x03:
			case 0x04:
			case 0x05:
			case 0x06:
			case 0x07:
			case 0x08:
			case 0x09:
			case 0x0a:
			case 0x0b:
			case 0x0c:
			case 0x0d:
			case 0x0e:
			case 0x0f:
			case 0x10:
			case 0x11:
				if (0x01 == p_ucSetFlag)
				{
					memset (&p_pstHAL->astLED [p_ucSlot][0], 0x00, sizeof (p_pstHAL->astLED [p_ucSlot][0]));
					memset (&p_pstHAL->astLED [p_ucSlot][1], 0x00, sizeof (p_pstHAL->astLED [p_ucSlot][1]));

					p_pstHAL->astLED [p_ucSlot][0].ucState = HAL_LED_STATE_OFF;
					p_pstHAL->astLED [p_ucSlot][0].ucStatus = HAL_LED_STATUS_OFF;
				}

				ioctl (p_pstHAL->stCtrlPort.hComPort, 0, p_ucSlot);
				//if(0x0a != p_ucSlot)logger_log (&g_stLog, LOG_ALL, "@%04d %s: LED_0x%02x_OFF", __LINE__, HAL_LED_NAME, p_ucSlot);
				break;
			}

		#endif
	}
}

void hal_led_on (HAL_LED *p_pstHAL, unsigned char p_ucSlot)
{
	_hal_led_on (p_pstHAL, p_ucSlot, 0x01);
}

void hal_led_off (HAL_LED *p_pstHAL, unsigned char p_ucSlot)
{
	_hal_led_off (p_pstHAL, p_ucSlot, 0x01);
}

unsigned char hal_led_slot (HAL_LED *p_pstHAL, char *p_pcIf)
{
	#if defined (ARCH_L100) || defined (ARCH_L200)

		if (0 != strstr (p_pcIf, "ttyS0"))
		{
			return 0x01;
		}
		else if (0 != strstr (p_pcIf, "ttyS2"))
		{
			return 0x02;
		}
		else if (0 != strstr (p_pcIf, "ttyS3"))
		{
			return 0x03;
		}

	#elif defined (ARCH_L300) || defined (ARCH_L350)

		if (0 != strstr (p_pcIf, "ttyF0"))
		{
			return 0x01;
		}
		else if (0 != strstr (p_pcIf, "ttyS3"))
		{
			return 0x02;
		}
		else if (0 != strstr (p_pcIf, "ttyS5"))
		{
			return 0x03;
		}
		else if (0 != strstr (p_pcIf, "ttyS1"))
		{
			return 0x04;
		}

	#endif

	return 0xFF;
}

void hal_led (HAL_LED *p_pstHAL, unsigned char p_ucSlot, unsigned long p_ulPause, unsigned long p_ulGap, unsigned long p_ulRest, unsigned char p_ucCount,
			  unsigned char p_ucRepeat, unsigned char p_ucState, unsigned long p_ulTrgPoint)
{
//if (p_ucSlot >0 && p_ucSlot < 4)
//  printf ("Slot %d LED p_ulPause=%lu, p_ulGap=%lu, p_ulRest=%lu, p_ucCount=%d, p_ucRepeat=%d, p_ucState=%d, p_ulTrgPoint=%lu\n",
//  p_ucSlot, p_ulPause, p_ulGap,p_ulRest,p_ucCount,p_ucRepeat,p_ucState, p_ulTrgPoint);

	#if defined (ARCH_L100) || defined (ARCH_L200)

		if (0xFF == p_ucSlot && (HAL_LED_STATE_ON == p_ucState || HAL_LED_STATE_OFF == p_ucState))
		{
			if (HAL_LED_STATE_ON == p_ucState)
			{
				_hal_led_on (p_pstHAL, 0xFF, 0x01);	// Auto blink blue LED
			}
			else
			{
				_hal_led_off (p_pstHAL, 0x00, 0x01);	// Off blue LED
			}

			return;
		}

	#elif defined (ARCH_N300)

	    if (HAL_LED_STATE_BLINK == p_ucState)
        {
            _hal_led_on (p_pstHAL, p_ucSlot, 0x08);
            return;
        }
        
    #endif

	if (HAL_LED_MAX <= p_ucSlot)
	{
		return;
	}

	if (0x00 == p_pstHAL->astLED [p_ucSlot][1].ucIsUsed || p_ulTrgPoint != p_pstHAL->astLED [p_ucSlot][1].ulTrgPoint)
	{
//if (p_ucSlot == 4)
//printf ("b(%d) SET Count : %d..old=%lu.new=%lu\n", p_ucSlot, p_pstHAL->astLED [p_ucSlot][0].ucCount, p_pstHAL->astLED [p_ucSlot][1].ulTrgPoint, p_ulTrgPoint);


		p_pstHAL->astLED [p_ucSlot][1].ulPause = p_ulPause;
		p_pstHAL->astLED [p_ucSlot][1].ulGap = p_ulGap;
		p_pstHAL->astLED [p_ucSlot][1].ulRest = p_ulRest;
		p_pstHAL->astLED [p_ucSlot][1].ucCount = p_ucCount;
		p_pstHAL->astLED [p_ucSlot][1].ucRepeat = p_ucRepeat;

		p_pstHAL->astLED [p_ucSlot][1].ucState = p_ucState;
		p_pstHAL->astLED [p_ucSlot][1].ulLastTime = 0;
		p_pstHAL->astLED [p_ucSlot][1].ulExpiry = 0;

		if (p_ulTrgPoint != p_pstHAL->astLED [p_ucSlot][1].ulTrgPoint)
		{
			p_pstHAL->astLED [p_ucSlot][1].ulTrgPoint = p_ulTrgPoint;
			p_pstHAL->astLED [p_ucSlot][1].ucInterrupt = 0x01;
		}
		else
		{
			p_pstHAL->astLED [p_ucSlot][1].ucInterrupt = 0x00;
		}


		p_pstHAL->astLED [p_ucSlot][1].ucIsUsed = 0x01;
	}
}

unsigned char hal_led_main (HAL_LED *p_pstHAL)
{
	unsigned char	ucIndex,
					ucCache;

	unsigned long	ulCurTime;

	for (ucIndex = 0x00; HAL_LED_MAX > ucIndex; ucIndex++)
	{
		// Interrupt
		if (0x01 == p_pstHAL->astLED [ucIndex][1].ucIsUsed)
		{
			if (0x01 == p_pstHAL->astLED [ucIndex][1].ucInterrupt || HAL_LED_STATE_ON == p_pstHAL->astLED [ucIndex][1].ucState || HAL_LED_STATE_OFF == p_pstHAL->astLED [ucIndex][1].ucState)
			{
				ucCache = p_pstHAL->astLED [ucIndex][0].ucStatus;

				memcpy (&p_pstHAL->astLED [ucIndex][0], &p_pstHAL->astLED [ucIndex][1], sizeof (HAL_LED_SLOT));

				p_pstHAL->astLED [ucIndex][0].ucStatus = ucCache;

				p_pstHAL->astLED [ucIndex][0].ucIsUsed = 0x01;
				p_pstHAL->astLED [ucIndex][1].ucIsUsed = 0x00;
				p_pstHAL->astLED [ucIndex][0].ucInterrupt = 0x00;

				if (0x00 != p_pstHAL->astLED [ucIndex][0].ucRepeat && 0x00 != p_pstHAL->astLED [ucIndex][0].ucCount)
				{
					p_pstHAL->astLED [ucIndex][0].ucRepeat = p_pstHAL->astLED [ucIndex][0].ucCount;
				}
			}
		}
	}

	IGET_TICKCOUNT (ulCurTime);

	for (ucIndex = 0x00; HAL_LED_MAX > ucIndex; ucIndex++)
	{
		if (0x01 == p_pstHAL->astLED [ucIndex][0].ucIsUsed)
		{
			p_pstHAL->astLED [ucIndex][0].ucIsUsed = 0x00;

			p_pstHAL->astLED [ucIndex][0].ulLastTime = ulCurTime;
			p_pstHAL->astLED [ucIndex][0].ulExpiry = ulCurTime;

			//if (HAL_LED_STATE_IDLE == p_pstHAL->astLED [ucIndex][0].ucState)
			if (HAL_LED_STATE_IDLE == (p_pstHAL->astLED [ucIndex][0].ucState & HAL_LED_STATE_MAX))
			{
				if (0 != p_pstHAL->astLED [ucIndex][0].ulPause)
				{
					if ((HAL_LED_STATUS_OFF == p_pstHAL->astLED [ucIndex][0].ucStatus) 
					        && ((0 != p_pstHAL->astLED [ucIndex][0].ucCount && 0 != p_pstHAL->astLED [ucIndex][0].ucRepeat) 
					                || (0 == p_pstHAL->astLED [ucIndex][0].ucRepeat)))
					{
						p_pstHAL->astLED [ucIndex][0].ucStatus = HAL_LED_STATUS_ON;

						if (0x03 == (p_pstHAL->astLED [ucIndex][0].ucState & 0x03)) // red
    					{
    					    _hal_led_on (p_pstHAL, ucIndex, 0x06);
    					}
    					else if (0x02 == (p_pstHAL->astLED [ucIndex][0].ucState & 0x02)) // green
    					{
    					    _hal_led_on (p_pstHAL, ucIndex, 0x04);
    					}
    					else if (0x01 == (p_pstHAL->astLED [ucIndex][0].ucState & 0x01)) // yellow
    					{
    					    _hal_led_on (p_pstHAL, ucIndex, 0x02);
    					}
    					else
    					{
    					    _hal_led_on (p_pstHAL, ucIndex, 0x00);
    					}
                        //if (ucIndex == 3)
                        //printf ("a (%d) current count : %d\n", ucIndex, p_pstHAL->astLED [ucIndex][0].ucCount);
					}

					p_pstHAL->astLED [ucIndex][0].ulExpiry += p_pstHAL->astLED [ucIndex][0].ulPause;
				}
				else
				{
					p_pstHAL->astLED [ucIndex][0].ucStatus = HAL_LED_STATUS_ON;
					if (0x03 == (p_pstHAL->astLED [ucIndex][0].ucState & 0x03)) // red
					{
					    _hal_led_on (p_pstHAL, ucIndex, 0x06);
					}
					else if (0x02 == (p_pstHAL->astLED [ucIndex][0].ucState & 0x02)) // green
					{
					    _hal_led_on (p_pstHAL, ucIndex, 0x04);
					}
					else if (0x01 == (p_pstHAL->astLED [ucIndex][0].ucState & 0x01)) // yellow
					{
					    _hal_led_on (p_pstHAL, ucIndex, 0x02);
					}
					else
					{
					    _hal_led_on (p_pstHAL, ucIndex, 0x00);
					}
				}
                /*
				if (0 != p_pstHAL->astLED [ucIndex][0].ucCount)
				{
					(p_pstHAL->astLED [ucIndex][0].ucCount)--;
				}
                */
				p_pstHAL->astLED [ucIndex][0].ucState = HAL_LED_STATE_PAUSE;
			}
			else if (HAL_LED_STATE_PAUSE == p_pstHAL->astLED [ucIndex][0].ucState)
			{
				if (0 != p_pstHAL->astLED [ucIndex][0].ulGap)
				{
					if (HAL_LED_STATUS_ON == p_pstHAL->astLED [ucIndex][0].ucStatus)
					{
						p_pstHAL->astLED [ucIndex][0].ucStatus = HAL_LED_STATUS_OFF;
						_hal_led_off (p_pstHAL, ucIndex, 0x00);
					}

					p_pstHAL->astLED [ucIndex][0].ulExpiry += p_pstHAL->astLED [ucIndex][0].ulGap;
				}

				p_pstHAL->astLED [ucIndex][0].ucState = HAL_LED_STATE_GAP;
			}
			else if (HAL_LED_STATE_GAP == p_pstHAL->astLED [ucIndex][0].ucState)
			{
//if (ucIndex==4)
//			  printf("state:GAP, %lu, %d\n", p_pstHAL->astLED [ucIndex][0].ulRest, p_pstHAL->astLED [ucIndex][0].ucCount);
				if (0 != p_pstHAL->astLED [ucIndex][0].ulRest && 0x00 == p_pstHAL->astLED [ucIndex][0].ucCount)
				{
					p_pstHAL->astLED [ucIndex][0].ulExpiry += p_pstHAL->astLED [ucIndex][0].ulRest;
				}

				p_pstHAL->astLED [ucIndex][0].ucState = HAL_LED_STATE_REST;
			}
			else
			{
			    if (HAL_LED_STATE_ON == (p_pstHAL->astLED [ucIndex][0].ucState & HAL_LED_STATE_MAX))
				{
				    if (HAL_LED_STATUS_OFF == p_pstHAL->astLED [ucIndex][0].ucStatus)
					{
					    p_pstHAL->astLED [ucIndex][0].ucStatus = HAL_LED_STATUS_ON;

					    if (0x03 == (p_pstHAL->astLED [ucIndex][0].ucState & 0x03)) // red
						{
						    _hal_led_on (p_pstHAL, ucIndex, 0x06);
						}
						else if (0x02 == (p_pstHAL->astLED [ucIndex][0].ucState & 0x02)) // green
						{
						    _hal_led_on (p_pstHAL, ucIndex, 0x04);
						}
						else if (0x01 == (p_pstHAL->astLED [ucIndex][0].ucState & 0x01)) // yellow
						{
						    _hal_led_on (p_pstHAL, ucIndex, 0x02);
						}
						else
						{
						    _hal_led_on (p_pstHAL, ucIndex, 0x00);
						}
					}
				}
				else if (HAL_LED_STATE_OFF == p_pstHAL->astLED [ucIndex][0].ucState)
				{
				    if (HAL_LED_STATUS_ON == p_pstHAL->astLED [ucIndex][0].ucStatus)
					{
					    p_pstHAL->astLED [ucIndex][0].ucStatus = HAL_LED_STATUS_OFF;
						_hal_led_off (p_pstHAL, ucIndex, 0x00);
					}
				}
/*
			  if (0 != p_pstHAL->astLED [ucIndex][0].ucCount)
        {
  				if (HAL_LED_STATE_ON == p_pstHAL->astLED [ucIndex][0].ucState)
  				{
  					if (HAL_LED_STATUS_OFF == p_pstHAL->astLED [ucIndex][0].ucStatus)
  					{
  						p_pstHAL->astLED [ucIndex][0].ucStatus = HAL_LED_STATUS_ON;
  						_hal_led_on (p_pstHAL, ucIndex, 0x00);
  if (ucIndex == 3)
  printf ("c (%d) current count : %d\n", ucIndex, p_pstHAL->astLED [ucIndex][0].ucCount);
  					}
  				}
  				else if (HAL_LED_STATE_OFF == p_pstHAL->astLED [ucIndex][0].ucState)
  				{
  					if (HAL_LED_STATUS_ON == p_pstHAL->astLED [ucIndex][0].ucStatus)
  					{
  						p_pstHAL->astLED [ucIndex][0].ucStatus = HAL_LED_STATUS_OFF;
  						_hal_led_off (p_pstHAL, ucIndex, 0x00);
  					}
  				}
			  }
*/
				if (0x00 == p_pstHAL->astLED [ucIndex][0].ucCount)
				{
					// Cache the next state
					if (0x01 == p_pstHAL->astLED [ucIndex][1].ucIsUsed)
					{
						ucCache = p_pstHAL->astLED [ucIndex][0].ucStatus;

						memcpy (&p_pstHAL->astLED [ucIndex][0], &p_pstHAL->astLED [ucIndex][1], sizeof (HAL_LED_SLOT));
						p_pstHAL->astLED [ucIndex][1].ucIsUsed = 0x00;

//if (ucIndex == 4)
//printf ("b(%d) Reset inused : %d\n", ucIndex, p_pstHAL->astLED [ucIndex][0].ucCount);

						p_pstHAL->astLED [ucIndex][0].ucStatus = ucCache;
						p_pstHAL->astLED [ucIndex][0].ucInterrupt = 0x00;
					}
					else if (0x00 != p_pstHAL->astLED [ucIndex][0].ucRepeat)
					{
					    p_pstHAL->astLED [ucIndex][0].ucState = p_pstHAL->astLED [ucIndex][1].ucState;//HAL_LED_STATE_IDLE;
						p_pstHAL->astLED [ucIndex][0].ucCount = p_pstHAL->astLED [ucIndex][0].ucRepeat;
//if (ucIndex == 4)
//printf ("b(%d) Set Repeat : %d\n", ucIndex, p_pstHAL->astLED [ucIndex][0].ucCount);
					}
				}
				else
				{
					if (0 != p_pstHAL->astLED [ucIndex][0].ucCount)
					{
//if (ucIndex == 4)
//printf ("(%d) current count : %d\n", ucIndex, p_pstHAL->astLED [ucIndex][0].ucCount);
						(p_pstHAL->astLED [ucIndex][0].ucCount)--;
//            if (1 == p_pstHAL->astLED [ucIndex][0].ucCount)
  //            p_pstHAL->astLED [ucIndex][0].ucCount = 0;
					}
					p_pstHAL->astLED [ucIndex][0].ucState = p_pstHAL->astLED [ucIndex][1].ucState;//HAL_LED_STATE_IDLE;
				}
			}
		}

		if (ulCurTime >= p_pstHAL->astLED [ucIndex][0].ulExpiry || ulCurTime < p_pstHAL->astLED [ucIndex][0].ulLastTime ||
			0 == p_pstHAL->astLED [ucIndex][0].ulExpiry || 0 == p_pstHAL->astLED [ucIndex][0].ulLastTime)
		{
			p_pstHAL->astLED [ucIndex][0].ucIsUsed = 0x01;
		}
	}

	return 0x00;
}
