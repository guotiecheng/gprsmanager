#include "GTLV.h"

void gtlv_init (GTLV_DATA *p_pstTLV)
{
	p_pstTLV->iMstCnt = 0;
	p_pstTLV->iRcyCnt = 0;

	p_pstTLV->ulDataLen = 0;
	
	p_pstTLV->pstMstFront = 0;
	p_pstTLV->pstMstRear = 0;

	p_pstTLV->pstRcyFront = 0;
	p_pstTLV->pstRcyRear = 0;

	p_pstTLV->iTagDef = GTLV_TAG_2_BYTE_CODE;
}

unsigned char gtlv_set_opt (GTLV_DATA *p_pstTLV, int p_iOptID, unsigned char *p_pucOptPtr, unsigned int p_uiOptValLen)
{
	int		iBuf;

	switch (p_iOptID)
	{
	case GTLV_OPTION_TAG_CODE:
		if (0 == p_pucOptPtr)
		{
			return 0x02;	// Option value is not valid
		}
		else if (sizeof (int) != p_uiOptValLen)
		{
			return 0x02;	// Option value is not valid
		}

		memcpy (&iBuf, p_pucOptPtr, p_uiOptValLen);
		
		if (GTLV_TAG_STANDARD_CODE != iBuf && GTLV_TAG_1_BYTE_CODE != iBuf && GTLV_TAG_2_BYTE_CODE != iBuf)
		{
			return 0x03;	// Option value is not valid
		}

		p_pstTLV->iTagDef = iBuf;
		break;

	default:
		return 0x04;	// Option is not found
	}

	return 0x00;
}

void gtlv_term (GTLV_DATA *p_pstTLV)
{
	GTLV_NODE	*pstTarget,
				*pstBuffer;

	// Point to master list
	pstTarget = p_pstTLV->pstMstFront;

	while (0 != pstTarget)
	{
		MEM_CLEAN ((pstTarget->pucValue));

		pstBuffer = pstTarget;
		pstTarget = pstTarget->pstTail;

		MEM_CLEAN (pstBuffer);
	}

	p_pstTLV->ulDataLen = 0;
	p_pstTLV->iMstCnt = 0;

	p_pstTLV->pstMstFront = 0;
	p_pstTLV->pstMstRear = 0;

	// Point to recycle list
	pstTarget = p_pstTLV->pstRcyFront;

	while (0 != pstTarget)
	{
		pstBuffer = pstTarget;
		pstTarget = pstTarget->pstTail;

		MEM_CLEAN (pstBuffer);
	}

	p_pstTLV->iRcyCnt = 0;

	p_pstTLV->pstRcyFront = 0;
	p_pstTLV->pstRcyRear = 0;
}

void gtlv_reset (GTLV_DATA *p_pstTLV)
{
	GTLV_NODE	*pstTarget,
				*pstBuffer;

	// Point to master list
	pstTarget = p_pstTLV->pstMstFront;

	while (0 != pstTarget)
	{
		MEM_CLEAN ((pstTarget->pucValue));

		pstTarget->iLen = 0;
		pstTarget->iTag = 0;

		pstBuffer = pstTarget;
		pstTarget = pstTarget->pstTail;

		// Migrate to recycle list
		if (0 != p_pstTLV->pstRcyRear)
		{
			p_pstTLV->pstRcyRear->pstTail = pstBuffer;
			pstBuffer->pstHead = p_pstTLV->pstRcyRear;
			pstBuffer->pstTail = 0;
			p_pstTLV->pstRcyRear = pstBuffer;
		}
		else	// Empty list
		{
			pstBuffer->pstHead = 0;
			pstBuffer->pstTail = 0;

			p_pstTLV->pstRcyFront = pstBuffer;
			p_pstTLV->pstRcyRear = pstBuffer;
		}

		(p_pstTLV->iRcyCnt)++;
	}

	p_pstTLV->ulDataLen = 0;
	p_pstTLV->iMstCnt = 0;

	p_pstTLV->pstMstFront = 0;
	p_pstTLV->pstMstRear = 0;
}

unsigned char gtlv_parse (unsigned char *p_pucRaw, unsigned long p_ulLen, GTLV_DATA *p_pstTLV, unsigned long *p_pulErrPos)
{
	unsigned char	ucRet,
					*pucStart,
					*pucEnd;

	GTLV_NODE		*pstNode	= 0;

	if (0 == p_pucRaw || 0 == p_pstTLV)
	{
		return 0x01;	// Pass in parameter(s) is not correct
	}

	gtlv_reset (p_pstTLV);
		
	pucStart = p_pucRaw,
	pucEnd = p_pucRaw + p_ulLen;

	while (pucStart < pucEnd)
	{
		switch (p_pstTLV->iTagDef)
		{
		case GTLV_TAG_2_BYTE_CODE:
			// Check got buffer for tag (2 bytes) and length (2 bytes) or not
			if (4 > (pucEnd - pucStart))
			{
				// Return back the error position in raw data
				if (0 != p_pulErrPos)
				{
					*p_pulErrPos = pucStart - p_pucRaw;
				}

				ucRet = 0x04;	// Parse error - buffer underflow
				goto CleanUp;
			}
			break;

		case GTLV_TAG_1_BYTE_CODE:
			// Check got buffer for tag (1 bytes) and length (2 bytes) or not
			if (3 > (pucEnd - pucStart))
			{
				// Return back the error position in raw data
				if (0 != p_pulErrPos)
				{
					*p_pulErrPos = pucStart - p_pucRaw;
				}

				ucRet = 0x04;	// Parse error - buffer underflow
				goto CleanUp;
			}
			break;

		default:
			ucRet = 0x06;	// Definition is not supported
			goto CleanUp;
		}

		// Try to reclaim from recycle list
		if (0 < p_pstTLV->iRcyCnt)
		{
			pstNode = p_pstTLV->pstRcyFront;

			if (p_pstTLV->pstRcyRear != p_pstTLV->pstRcyFront)
			{
				p_pstTLV->pstRcyFront = pstNode->pstTail;
			}
			else
			{
				p_pstTLV->pstRcyFront = 0;
				p_pstTLV->pstRcyRear = 0;
			}

			(p_pstTLV->iRcyCnt)--;
		}
		else
		{
			MEM_ALLOC_EX (pstNode, sizeof (GTLV_NODE), (GTLV_NODE *));
		}

		if (0 == pstNode)
		{
			ucRet = 0x02;	// Failed to allocate memory
			goto CleanUp;
		}
		else
		{
			pstNode->pucValue = 0;
			pstNode->pstHead = 0;
			pstNode->pstTail = 0;
		}

		switch (p_pstTLV->iTagDef)
		{
		case GTLV_TAG_2_BYTE_CODE:
			// Set the tag
			pstNode->iTag = (*pucStart) << 8;
			pucStart++;

			pstNode->iTag += *pucStart;
			pstNode->iTag = ntoh_short (pstNode->iTag);	// Convert the byte order (Big endian or little endian)
			pucStart++;
			break;

		case GTLV_TAG_1_BYTE_CODE:
			// Set the tag
			pstNode->iTag = *pucStart;
			pucStart++;
			pstNode->iTag = ntoh_short (pstNode->iTag);	// Convert the byte order (Big endian or little endian)
			break;

		default:
			ucRet = 0x06;	// Definition is not supported
			goto CleanUp;
		}

		// Set the length
		pstNode->iLen = (*pucStart) << 8;
		pucStart++;

		pstNode->iLen += *pucStart;
		pstNode->iLen = ntoh_short (pstNode->iLen);	// Convert the byte order (Big endian or little endian)
		pucStart++;

		// Set the value
		if (0 < pstNode->iLen)
		{
			if ((pucStart + pstNode->iLen) <= pucEnd)
			{
				MEM_ALLOC_EX (pstNode->pucValue, (sizeof (unsigned char) * pstNode->iLen), (unsigned char *));

				if (0 == pstNode->pucValue)
				{
					ucRet = 0x02;	// Failed to allocate memory
					goto CleanUp;
				}
				else
				{
					memcpy (pstNode->pucValue, pucStart, pstNode->iLen);
					pucStart += (pstNode->iLen);
				}
			}
			else
			{
				// Return back the error position in raw data
				if (0 != p_pulErrPos)
				{
					*p_pulErrPos = pucStart - p_pucRaw;
				}

				ucRet = 0x03;	// Parse error - buffer overflow
				goto CleanUp;
			}
		}
		else
		{
			pstNode->pucValue = 0;
		}

		// Adjust linked-list pointer
		if (p_pstTLV->pstMstFront != p_pstTLV->pstMstRear)	// Many item in list
		{
			pstNode->pstHead = p_pstTLV->pstMstRear;
			p_pstTLV->pstMstRear->pstTail = pstNode;
			p_pstTLV->pstMstRear = pstNode;
		}
		else
		{
			if (0 == p_pstTLV->pstMstFront)	// Empty list
			{
				p_pstTLV->pstMstFront = p_pstTLV->pstMstRear = pstNode;
			}
			else	// Only one item in list
			{
				pstNode->pstHead = p_pstTLV->pstMstRear;
				p_pstTLV->pstMstRear->pstTail = pstNode;
				p_pstTLV->pstMstRear = pstNode;
			}
		}

		// Adjust the whole calc of data length
		(p_pstTLV->ulDataLen) += (pstNode->iLen);

		// Adjust data count
		(p_pstTLV->iMstCnt)++;

		pstNode = 0;
	}

	ucRet = 0x00;

CleanUp:
	if (0 != pstNode)
	{
		MEM_CLEAN ((pstNode->pucValue));
		MEM_CLEAN (pstNode);
	}

	return ucRet;
}

unsigned char gtlv_build (GTLV_DATA *p_pstTLV, unsigned char **p_ppucRaw, unsigned long *p_pulLen)
{
	unsigned long	ulLen;

	int				iByteOrder;

	unsigned char	*pucRaw,
					*pucPtr;

	GTLV_NODE		*pstNode;

	if (0 == p_pstTLV || 0 == p_ppucRaw || 0 == p_pulLen)
	{
		return 0x01;	// Pass in parameter(s) is not correct
	}

	if (0 < p_pstTLV->ulDataLen)
	{
		pstNode = p_pstTLV->pstMstFront;

		if (0 != pstNode)
		{
			switch (p_pstTLV->iTagDef)
			{
			case GTLV_TAG_2_BYTE_CODE:
				// Whole data length + (Number of data * (2 bytes "Tag" + 2 bytes "Length"))
				ulLen = (p_pstTLV->ulDataLen) + (p_pstTLV->iMstCnt * 4);

				MEM_ALLOC_EX (pucRaw, (sizeof (unsigned char) * ulLen), (unsigned char *));

				if (0 == pucRaw)
				{
					return 0x02;	// Failed to allocate memory
				}
				
				pucPtr = pucRaw;

				while (0 != pstNode)
				{
					// Set the tag (2 bytes)
					iByteOrder = hton_short (pstNode->iTag);
					*pucPtr = (iByteOrder >> 8 & 0xff); pucPtr++;
					*pucPtr = (iByteOrder >> 0 & 0xff); pucPtr++;

					// Set the length (2 bytes)
					iByteOrder = hton_short (pstNode->iLen);
					*pucPtr = (iByteOrder >> 8 & 0xff); pucPtr++;
					*pucPtr = (iByteOrder >> 0 & 0xff); pucPtr++;

					// Set the value
					memcpy (pucPtr, pstNode->pucValue, pstNode->iLen);
					pucPtr += (pstNode->iLen);

					pstNode = pstNode->pstTail;
				}
				break;

			case GTLV_TAG_1_BYTE_CODE:
				// Whole data length + (Number of data * (1 bytes "Tag" + 2 bytes "Length"))
				ulLen = (p_pstTLV->ulDataLen) + (p_pstTLV->iMstCnt * 3);

				MEM_ALLOC_EX (pucRaw, (sizeof (unsigned char) * ulLen), (unsigned char *));

				if (0 == pucRaw)
				{
					return 0x02;	// Failed to allocate memory
				}
				
				pucPtr = pucRaw;

				while (0 != pstNode)
				{
					// Set the tag (1 bytes)
					iByteOrder = hton_short (pstNode->iTag);
					*pucPtr = (iByteOrder >> 0 & 0xff); pucPtr++;

					// Set the length (2 bytes)
					iByteOrder = hton_short (pstNode->iLen);
					*pucPtr = (iByteOrder >> 8 & 0xff); pucPtr++;
					*pucPtr = (iByteOrder >> 0 & 0xff); pucPtr++;

					// Set the value
					memcpy (pucPtr, pstNode->pucValue, pstNode->iLen);
					pucPtr += (pstNode->iLen);

					pstNode = pstNode->pstTail;
				}
				break;

			default:
				return 0x04;	// Definition is not supported
			}

			*p_ppucRaw = pucRaw;
			*p_pulLen = ulLen;
		}
		else
		{
			*p_ppucRaw = 0;
			*p_pulLen = 0;

			return 0x03;	// Data structure corrupted
		}
	}
	else
	{
		*p_ppucRaw = 0;
		*p_pulLen = 0;
	}

	return 0x00;
}

unsigned char gtlv_get (GTLV_DATA *p_pstTLV, int p_iTag, unsigned char **p_ppucData, int *p_piLen, unsigned char p_ucSkip)
{
	GTLV_NODE	*pstTarget;

	// Point to first TLV node
	pstTarget = p_pstTLV->pstMstFront;

	while (0 != pstTarget)
	{
		if (pstTarget->iTag == p_iTag)	// Compare the tag
		{
			if (0 < p_ucSkip)
			{
				p_ucSkip--;
			}
			else
			{
				if (0 != p_ppucData)
				{
					*p_ppucData = pstTarget->pucValue;
				}

				if (0 != p_piLen)
				{
					*p_piLen = pstTarget->iLen;
				}

				return 0x00;
			}
		}

		pstTarget = pstTarget->pstTail;
	}

	return 0x03;	// Tag not found
}

unsigned char gtlv_set (GTLV_DATA *p_pstTLV, int p_iTag, unsigned char *p_pucData, int p_iLen)
{
	GTLV_NODE		*pstTarget;

	unsigned char	*pucData;

	if (0 == p_pstTLV || 0 > p_iLen)
	{
		return 0x01;	// Pass in parameter(s) is not correct
	}

	// Point to first TLV node
	pstTarget = p_pstTLV->pstMstFront;

	while (0 != pstTarget)
	{
		if (pstTarget->iTag == p_iTag)	// Compare the tag
		{
			// Allocate memory for new data
			MEM_ALLOC_EX (pucData, (sizeof (unsigned char) * p_iLen), (unsigned char *));

			if (0 == pucData)
			{
				return 0x02;	// Failed to allocate memory
			}

			memcpy (pucData, p_pucData, p_iLen);

			MEM_CLEAN ((pstTarget->pucValue));

			// Adjust the whole calc of data length
			p_pstTLV->ulDataLen -= pstTarget->iLen;
			p_pstTLV->ulDataLen += p_iLen;

			pstTarget->pucValue = pucData;
			pstTarget->iLen = p_iLen;

			return 0x00;
		}

		pstTarget = pstTarget->pstTail;
	}

	return 0x03;	// Tag not found
}

unsigned char gtlv_add (GTLV_DATA *p_pstTLV, int p_iTag, unsigned char *p_pucData, int p_iLen, unsigned char p_ucAllowDupl)
{
	GTLV_NODE *pstNode	= 0;

	if (0 == p_pstTLV || 0 > p_iLen)
	{
		return 0x01;	// Pass in parameter(s) is not correct
	}

	// Check for duplication before insertion
	if (0x00 == p_ucAllowDupl)
	{
		if (0 == gtlv_get (p_pstTLV, p_iTag, 0, 0, 0))
		{
			return 0x02;	// Duplicate tag
		}
	}

	// Try to reclaim from recycle list
	if (0 < p_pstTLV->iRcyCnt)
	{
		pstNode = p_pstTLV->pstRcyFront;

		if (p_pstTLV->pstRcyRear != p_pstTLV->pstRcyFront)
		{
			p_pstTLV->pstRcyFront = pstNode->pstTail;
			p_pstTLV->pstRcyFront->pstHead = 0;
		}
		else
		{
			p_pstTLV->pstRcyFront = 0;
			p_pstTLV->pstRcyRear = 0;
		}

		(p_pstTLV->iRcyCnt)--;
	}
	else
	{
		MEM_ALLOC_EX (pstNode, sizeof (GTLV_NODE), (GTLV_NODE *));
	}

	if (0 == pstNode)
	{
		return 0x03;	// Failed to allocate memory
	}
	else
	{
		pstNode->pstHead = 0;
		pstNode->pstTail = 0;
	}

	// Copy data
	if (0 != p_pucData)
	{
		MEM_ALLOC_EX (pstNode->pucValue, p_iLen, (unsigned char *));

		if (0 == pstNode->pucValue)
		{
			MEM_CLEAN (pstNode);
			return 0x03;	// Failed to allocate memory
		}

		memcpy (pstNode->pucValue, p_pucData, p_iLen);
		pstNode->iLen = p_iLen;
	}
	else
	{
		pstNode->pucValue = 0;
		pstNode->iLen = 0;
	}

	// Copy tag
	pstNode->iTag = p_iTag;

	// Adjust linked-list pointer
	if (p_pstTLV->pstMstFront != p_pstTLV->pstMstRear)	// Many item in list
	{
		pstNode->pstHead = p_pstTLV->pstMstRear;
		p_pstTLV->pstMstRear->pstTail = pstNode;
		p_pstTLV->pstMstRear = pstNode;
	}
	else
	{
		if (0 == p_pstTLV->pstMstFront)	// Empty list
		{
			p_pstTLV->pstMstFront = pstNode;
			p_pstTLV->pstMstRear = pstNode;
		}
		else	// Only one item in list
		{
			pstNode->pstHead = p_pstTLV->pstMstRear;
			p_pstTLV->pstMstRear->pstTail = pstNode;
			p_pstTLV->pstMstRear = pstNode;
		}
	}

	// Adjust the whole calc of data length
	(p_pstTLV->ulDataLen) += p_iLen;

	// Adjust data count
	(p_pstTLV->iMstCnt)++;

	return 0x00;
}

unsigned char gtlv_del (GTLV_DATA *p_pstTLV, int p_iTag)
{
	GTLV_NODE	*pstTarget,
				*pstFront,
				*pstRear;

	if (0 == p_pstTLV)
	{
		return 0x01;	// Pass in parameter(s) is not correct
	}

	// Point to first TLV node
	pstTarget	= p_pstTLV->pstMstFront;

	while (0 != pstTarget)
	{
		if (pstTarget->iTag == p_iTag)	// Compare the tag
		{
			// Adjust linked-list pointer
			pstFront = pstTarget->pstHead;
			pstRear = pstTarget->pstTail;

			if (0 != pstFront) pstFront->pstTail = pstRear;
			if (0 != pstRear) pstRear->pstHead = pstFront;
			if (p_pstTLV->pstMstFront == pstTarget) p_pstTLV->pstMstFront = pstRear;
			if (p_pstTLV->pstMstRear == pstTarget) p_pstTLV->pstMstRear = pstFront;

			// Delete value
			MEM_CLEAN ((pstTarget->pucValue));

			// Adjust the whole calc of data length
			(p_pstTLV->ulDataLen) -= (pstTarget->iLen);

			// Migrate to recycle list
			pstTarget->iLen = 0;
			pstTarget->iTag = 0;

			if (0 != p_pstTLV->pstRcyRear)
			{
				p_pstTLV->pstRcyRear->pstTail = pstTarget;
				pstTarget->pstHead = p_pstTLV->pstRcyRear;
				pstTarget->pstTail = 0;
				p_pstTLV->pstRcyRear = pstTarget;
			}
			else	// Empty list
			{
				pstTarget->pstHead = 0;
				pstTarget->pstTail = 0;

				p_pstTLV->pstRcyFront = pstTarget;
				p_pstTLV->pstRcyRear = pstTarget;
			}

			(p_pstTLV->iRcyCnt)++;

			// Adjust master data count
			(p_pstTLV->iMstCnt)--;

			return 0x00;
		}

		pstTarget = pstTarget->pstTail;
	}

	return 0x02;	// Tag not found
}
