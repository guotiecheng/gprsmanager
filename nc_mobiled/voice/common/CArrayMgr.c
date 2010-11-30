#include "CArrayMgr.h"

unsigned char array_init (ARRAY_LIST *p_pstArray, int p_iSize, unsigned char p_ucOrder, unsigned char p_ucAllowDuplicate, ARRAY_DESTROY p_pfDestroy)
{
	int		iIndex;

	if (0 > p_iSize)
	{
		return 0x01;
	}

	if (ARRAY_ORDER_DIRECT != p_ucOrder && ARRAY_ORDER_FIFO != p_ucOrder)
	{
		return 0x02;
	}

	// It is initialized before
	if ((0x80 & p_pstArray->ucFlags) && 0 < p_pstArray->iSize && (ARRAY_ORDER_DIRECT == p_pstArray->ucOrder || ARRAY_ORDER_FIFO == p_pstArray->ucOrder))
	{
		array_term (p_pstArray);
	}
	
	p_pstArray->iHeadIndex = 0;
	p_pstArray->iTailIndex = 0;

	p_pstArray->iSize = p_iSize;
	p_pstArray->iCount = 0;
	p_pstArray->ucOrder = p_ucOrder;
	p_pstArray->pfDestroy = p_pfDestroy;

	critical_init (&p_pstArray->stLock);

	MEM_ALLOC ((p_pstArray->pastList), (p_pstArray->iSize * sizeof (ARRAY_DATA)), (ARRAY_DATA *));

	for (iIndex = 0; iIndex < p_pstArray->iSize; iIndex++)
	{
		p_pstArray->pastList [iIndex].iIndex = iIndex;
	}

	p_pstArray->ucFlags = (0x00 == p_ucAllowDuplicate) ? 0x80 : 0xC0;

	return 0x00;
}

void array_term (ARRAY_LIST *p_pstArray)
{
	int		iIndex,
			iCount;

	// It is not initialized before
	if (0x80 & p_pstArray->ucFlags)
	{
		critical_lock (&p_pstArray->stLock, 0x01);
		
		for (iIndex = 0, iCount = 0; iIndex < p_pstArray->iSize && iCount < p_pstArray->iCount; iIndex++)
		{
			if (0x80 & p_pstArray->pastList [iIndex].ucFlags)
			{
				iCount++;

				if (0x40 & p_pstArray->pastList [iIndex].ucFlags)
				{
					if (0 != p_pstArray->pfDestroy)
					{
						(p_pstArray->pfDestroy) (p_pstArray, &p_pstArray->pastList [iIndex]);
					}
					else
					{
						MEM_CLEAN (p_pstArray->pastList [iIndex].pvData);
					}
				}

				p_pstArray->pastList [iIndex].pvData = 0;
				p_pstArray->pastList [iIndex].ucFlags = 0x00;
			}
		}

		MEM_CLEAN (p_pstArray->pastList);

		p_pstArray->iHeadIndex = 0;
		p_pstArray->iTailIndex = 0;

		p_pstArray->iSize = 0;
		p_pstArray->iCount = 0;	
		p_pstArray->ucOrder = 0x00;

		critical_unlock (&p_pstArray->stLock);
		critical_term (&p_pstArray->stLock);

		p_pstArray->ucFlags = 0x00;
	}
}

unsigned char array_lock (ARRAY_LIST *p_pstArray)
{
	return critical_lock (&p_pstArray->stLock, 0x01);
}

unsigned char array_unlock (ARRAY_LIST *p_pstArray)
{
	return critical_unlock (&p_pstArray->stLock);
}

unsigned char array_add (ARRAY_LIST *p_pstArray, void *p_pvData, char *p_pcID, unsigned char p_ucDelFlag, ARRAY_DATA **p_ppstItem)
{
	unsigned char	ucRet;

	int				iIndex;

	if (0 != p_ppstItem)
	{
		*p_ppstItem = 0;
	}

	critical_lock (&p_pstArray->stLock, 0x01);

	if (p_pstArray->iCount == p_pstArray->iSize)
	{
		ucRet = 0x02;
		goto CleanUp;
	}

	if (ARRAY_ORDER_DIRECT == p_pstArray->ucOrder)
	{
		for (iIndex = 0; iIndex < p_pstArray->iSize; iIndex++)
		{
			// Is used flag
			if (!(0x80 & p_pstArray->pastList [iIndex].ucFlags))
			{
				break;
			}
		}
	}
	else if (ARRAY_ORDER_FIFO == p_pstArray->ucOrder)
	{
		// Calculate next index
		if (p_pstArray->iHeadIndex != p_pstArray->iTailIndex || 0 < p_pstArray->iCount)
		{
			iIndex = (p_pstArray->iTailIndex + 1) % p_pstArray->iSize;
		}
		else
		{
			iIndex = p_pstArray->iTailIndex;
		}

		if (0x80 & p_pstArray->pastList [iIndex].ucFlags)
		{
			ucRet = 0x03;	// No free slot
			goto CleanUp;
		}
	}
	else
	{
		iIndex = p_pstArray->iSize;
	}

	if (0 <= iIndex && iIndex < p_pstArray->iSize)
	{
		p_pstArray->pastList [iIndex].pvData = p_pvData;

		memset (p_pstArray->pastList [iIndex].acID, 0x00, sizeof (p_pstArray->pastList [iIndex].acID));

		if (0 != p_pcID)
		{
			strncpy (p_pstArray->pastList [iIndex].acID, p_pcID, sizeof (p_pstArray->pastList [iIndex].acID) - 1);
		}

		if (0x00 == p_ucDelFlag)
		{
			p_pstArray->pastList [iIndex].ucFlags = 0x80;
		}
		else
		{
			p_pstArray->pastList [iIndex].ucFlags = 0xC0;
		}

		if (ARRAY_ORDER_FIFO == p_pstArray->ucOrder)
		{
			p_pstArray->iTailIndex = iIndex;
		}

		(p_pstArray->iCount)++;

		if (0 != p_ppstItem)
		{
			*p_ppstItem = &p_pstArray->pastList [iIndex];
		}

		ucRet = 0x00;
		goto CleanUp;
	}

	ucRet = 0x04;

CleanUp:
	critical_unlock (&p_pstArray->stLock);
	
	return ucRet;
}

unsigned char array_del (ARRAY_LIST *p_pstArray, void **p_ppvData)
{
	unsigned char	ucRet;

	int				iIndex,
					iCount;

	critical_lock (&p_pstArray->stLock, 0x01);

	if (0 == p_pstArray->iCount)
	{
		ucRet = 0x00;
		goto CleanUp;
	}

	if (ARRAY_ORDER_DIRECT == p_pstArray->ucOrder)
	{
		for (iIndex = 0, iCount = 0; iIndex < p_pstArray->iSize && iCount < p_pstArray->iCount; iIndex++)
		{
			// Is used flag
			if (0x80 & p_pstArray->pastList [iIndex].ucFlags)
			{
				iCount++;

				// Is delete flag
				if (0 == p_ppvData)
				{
					if (0x40 & p_pstArray->pastList [iIndex].ucFlags)
					{
						if (0 != p_pstArray->pfDestroy)
						{
							(p_pstArray->pfDestroy) (p_pstArray, &p_pstArray->pastList [iIndex]);
						}
						else
						{
							MEM_CLEAN (p_pstArray->pastList [iIndex].pvData);
						}
					}
				}
				else
				{
					*p_ppvData = p_pstArray->pastList [iIndex].pvData;
				}

				memset (p_pstArray->pastList [iIndex].acID, 0x00, sizeof (p_pstArray->pastList [iIndex].acID) - 1);
				p_pstArray->pastList [iIndex].pvData = 0;
				p_pstArray->pastList [iIndex].ucFlags = 0x00;
				
				if (0 != p_pstArray->iCount)
				{
					(p_pstArray->iCount)--;
				}

				ucRet = 0x00;
				goto CleanUp;
			}
		}
	}
	else if (ARRAY_ORDER_FIFO == p_pstArray->ucOrder)
	{
		if (0x80 & (p_pstArray->pastList [p_pstArray->iHeadIndex]).ucFlags)
		{
			// Is delete flag
			if (0 == p_ppvData)
			{
				if (0x40 & p_pstArray->pastList [p_pstArray->iHeadIndex].ucFlags)
				{
					if (0 != p_pstArray->pfDestroy)
					{
						(p_pstArray->pfDestroy) (p_pstArray, &p_pstArray->pastList [p_pstArray->iHeadIndex]);
					}
					else
					{
						MEM_CLEAN (p_pstArray->pastList [p_pstArray->iHeadIndex].pvData);
					}
				}
			}
			else
			{
				*p_ppvData = p_pstArray->pastList [p_pstArray->iHeadIndex].pvData;
			}

			memset (p_pstArray->pastList [p_pstArray->iHeadIndex].acID, 0x00, sizeof (p_pstArray->pastList [p_pstArray->iHeadIndex].acID) - 1);
			p_pstArray->pastList [p_pstArray->iHeadIndex].pvData = 0;
			p_pstArray->pastList [p_pstArray->iHeadIndex].ucFlags = 0x00;
			
			// Adjust the head index
			if (p_pstArray->iHeadIndex < p_pstArray->iTailIndex)
			{
				(p_pstArray->iHeadIndex)++;
			}
			else
			{
				if (p_pstArray->iHeadIndex != p_pstArray->iTailIndex)
				{
					p_pstArray->iHeadIndex = (p_pstArray->iHeadIndex + 1) % p_pstArray->iSize;
				}
			}

			if (0 != p_pstArray->iCount)
			{
				(p_pstArray->iCount)--;
			}
		}

		ucRet = 0x00;
		goto CleanUp;
	}

	ucRet = 0x01;

CleanUp:
	critical_unlock (&p_pstArray->stLock);

	return ucRet;
}

unsigned char array_del_by_index (ARRAY_LIST *p_pstArray, int p_iIndex, void **p_ppvData)
{
	unsigned char	ucRet;

	if (0 > p_iIndex || p_pstArray->iSize <= p_iIndex)
	{
		return 0x01;
	}
	else if (ARRAY_ORDER_DIRECT != p_pstArray->ucOrder)
	{
		return 0x02;
	}

	critical_lock (&p_pstArray->stLock, 0x01);

	// Is used flag
	if (0x80 & p_pstArray->pastList [p_iIndex].ucFlags)
	{
		// Is delete flag
		if (0 == p_ppvData)
		{
			if (0x40 & p_pstArray->pastList [p_iIndex].ucFlags)
			{
				if (0 != p_pstArray->pfDestroy)
				{
					(p_pstArray->pfDestroy) (p_pstArray, &p_pstArray->pastList [p_iIndex]);
				}
				else
				{
					MEM_CLEAN (p_pstArray->pastList [p_iIndex].pvData);
				}
			}
		}
		else
		{
			*p_ppvData = p_pstArray->pastList [p_iIndex].pvData;
		}

		memset (p_pstArray->pastList [p_iIndex].acID, 0x00, sizeof (p_pstArray->pastList [p_iIndex].acID));
		p_pstArray->pastList [p_iIndex].pvData = 0;
		p_pstArray->pastList [p_iIndex].ucFlags = 0x00;

		if (0 != p_pstArray->iCount)
		{
			(p_pstArray->iCount)--;
		}

		ucRet = 0x00;
		goto CleanUp;
	}

	ucRet = 0x03;

CleanUp:
	critical_unlock (&p_pstArray->stLock);

	return ucRet;
}

unsigned char array_del_by_id (ARRAY_LIST *p_pstArray, char *p_pcID, void **p_ppvData)
{
	int		iIndex,
			iCount;

	if (0 == p_pcID)
	{
		return 0x01;
	}
	else if (ARRAY_ORDER_DIRECT != p_pstArray->ucOrder)
	{
		return 0x02;
	}

	critical_lock (&p_pstArray->stLock, 0x01);

	for (iIndex = 0, iCount = 0; iIndex < p_pstArray->iSize && iCount < p_pstArray->iCount; iIndex++)
	{
		// Is used flag
		if (0x80 & p_pstArray->pastList [iIndex].ucFlags)
		{
			iCount++;

			if (0 == strncmp (p_pstArray->pastList [iIndex].acID, p_pcID, sizeof (p_pstArray->pastList [iIndex].acID) - 1))
			{
				if (0 == p_ppvData)
				{
					// Is delete flag
					if (0x40 & p_pstArray->pastList [iIndex].ucFlags)
					{
						if (0 != p_pstArray->pfDestroy)
						{
							(p_pstArray->pfDestroy) (p_pstArray, &p_pstArray->pastList [iIndex]);
						}
						else
						{
							MEM_CLEAN (p_pstArray->pastList [iIndex].pvData);
						}
					}
				}
				else
				{
					*p_ppvData = p_pstArray->pastList [iIndex].pvData;
				}

				memset (p_pstArray->pastList [iIndex].acID, 0x00, sizeof (p_pstArray->pastList [iIndex].acID) - 1);
				p_pstArray->pastList [iIndex].pvData = 0;
				p_pstArray->pastList [iIndex].ucFlags = 0x00;

				if (0 != p_pstArray->iCount)
				{
					(p_pstArray->iCount)--;
				}

				critical_unlock (&p_pstArray->stLock);
				return 0x00;
			}
		}
	}
		
	critical_unlock (&p_pstArray->stLock);
	
	return 0x03;
}

void array_del_all (ARRAY_LIST *p_pstArray)
{
	int		iIndex,
			iCount;

	critical_lock (&p_pstArray->stLock, 0x01);
	
	for (iIndex = 0, iCount = 0; iIndex < p_pstArray->iSize && iCount < p_pstArray->iCount; iIndex++)
	{
		if (0x80 & p_pstArray->pastList [iIndex].ucFlags)
		{
			iCount++;

			if (0x40 & p_pstArray->pastList [iIndex].ucFlags)
			{
				if (0 != p_pstArray->pfDestroy)
				{
					p_pstArray->pfDestroy (p_pstArray, &p_pstArray->pastList [iIndex]);
				}
				else
				{
					MEM_CLEAN (p_pstArray->pastList [iIndex].pvData);
				}
			}

			p_pstArray->pastList [iIndex].pvData = 0;
			p_pstArray->pastList [iIndex].ucFlags = 0x00;
		}
	}

	p_pstArray->iHeadIndex = 0;
	p_pstArray->iTailIndex = 0;

	p_pstArray->iCount = 0;	

	critical_unlock (&p_pstArray->stLock);
}

unsigned char array_get_by_index (ARRAY_LIST *p_pstArray, int p_iIndex, ARRAY_DATA **p_ppstArrayData)
{
	if (0 > p_iIndex || p_pstArray->iSize <= p_iIndex)
	{
		return 0x01;
	}

	critical_lock (&p_pstArray->stLock, 0x01);

	// Is used flag
	if (0x80 & p_pstArray->pastList [p_iIndex].ucFlags)
	{
		if (0 != p_ppstArrayData)
		{
			*p_ppstArrayData = &(p_pstArray->pastList [p_iIndex]);
		}

		critical_unlock (&p_pstArray->stLock);
		return 0x00;
	}
	
	critical_unlock (&p_pstArray->stLock);

	return 0x02;
}

unsigned char array_get_by_id (ARRAY_LIST *p_pstArray, char *p_pcID, ARRAY_DATA **p_ppstArrayData)
{
	int		iIndex,
			iCount;

	if (0 == p_pcID)
	{
		return 0x01;
	}

	critical_lock (&p_pstArray->stLock, 0x01);

	for (iIndex = 0, iCount = 0; iIndex < p_pstArray->iSize && iCount < p_pstArray->iCount; iIndex++)
	{
		// Is used flag
		if (0x80 & p_pstArray->pastList [iIndex].ucFlags)
		{
			iCount++;

			if (0 == strncmp (p_pstArray->pastList [iIndex].acID, p_pcID, sizeof (p_pstArray->pastList [iIndex].acID) - 1))
			{
				if (0 != p_ppstArrayData)
				{
					*p_ppstArrayData = &(p_pstArray->pastList [iIndex]);
				}

				critical_unlock (&p_pstArray->stLock);
				return 0x00;
			}
		}
	}
		
	critical_unlock (&p_pstArray->stLock);
	
	return 0x02;
}

unsigned char array_get_first (ARRAY_LIST *p_pstArray, ARRAY_DATA **p_ppstData)
{
	unsigned char	ucRet;

	int				iIndex;

	*p_ppstData = 0;

	critical_lock (&p_pstArray->stLock, 0x01);

	if (ARRAY_ORDER_DIRECT == p_pstArray->ucOrder)
	{
		for (iIndex = 0; iIndex < p_pstArray->iSize; iIndex++)
		{
			// Is used flag
			if (0x80 & p_pstArray->pastList [iIndex].ucFlags)
			{
				*p_ppstData = &(p_pstArray->pastList [iIndex]);

				ucRet = 0x00;
				goto CleanUp;
			}
		}
	}
	else if (ARRAY_ORDER_FIFO == p_pstArray->ucOrder)
	{
		*p_ppstData = &(p_pstArray->pastList [p_pstArray->iHeadIndex]);

		if (0x80 & p_pstArray->pastList [p_pstArray->iHeadIndex].ucFlags)
		{
			ucRet = 0x00;
			goto CleanUp;
		}
	}

	ucRet = 0x02;

CleanUp:
	critical_unlock (&p_pstArray->stLock);
	
	return ucRet;
}

unsigned char array_get_next (ARRAY_LIST *p_pstArray, ARRAY_DATA *p_pstCurData, ARRAY_DATA **p_ppstNextData)
{
	unsigned char	ucRet;

	int				iIndex;

	*p_ppstNextData = 0;

	critical_lock (&p_pstArray->stLock, 0x01);

	if (ARRAY_ORDER_DIRECT == p_pstArray->ucOrder)
	{
		for (iIndex = p_pstCurData->iIndex + 1; iIndex < p_pstArray->iSize; iIndex++)
		{
			// Is used flag
			if (0x80 & p_pstArray->pastList [iIndex].ucFlags)
			{
				*p_ppstNextData = &(p_pstArray->pastList [iIndex]);

				ucRet = 0x00;
				goto CleanUp;
			}
		}
	}
	else if (ARRAY_ORDER_FIFO == p_pstArray->ucOrder)
	{
		if (p_pstCurData->iIndex != p_pstArray->iTailIndex)
		{
			iIndex = (p_pstCurData->iIndex + 1) % p_pstArray->iSize;

			if (0x80 & p_pstArray->pastList [iIndex].ucFlags)
			{
				*p_ppstNextData = &(p_pstArray->pastList [iIndex]);

				ucRet = 0x00;
				goto CleanUp;
			}
		}
	}

	ucRet = 0x01;

CleanUp:
	critical_unlock (&p_pstArray->stLock);
	
	return ucRet;
}
