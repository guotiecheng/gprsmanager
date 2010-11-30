#include "CUtility.h"

unsigned char ucIntToByte (int p_iInput, int p_iLen, unsigned char *p_pucOutput)
{
	int i;

	if (0 > p_iInput || 1 > p_iLen || !p_pucOutput) return 0x01;

	for (i = 0; i < p_iLen; i++)
		(p_pucOutput)[p_iLen - i - 1] = (p_iInput >> ((8 * i) & 0xFF));

	return 0x00;
}

unsigned char ucUShortToByte (unsigned short p_usInput, int p_iLen, unsigned char *p_pucOutput)
{
	int i;

	if (1 > p_iLen || !p_pucOutput) return 0x01;

	for (i = 0; i < p_iLen; i++)
		(p_pucOutput)[p_iLen - i - 1] = (p_usInput >> ((8 * i) & 0xFF));

	return 0x00;
}

unsigned char ucULongToByte (unsigned long p_ulInput, int p_iLen, unsigned char *p_pucOutput)
{
	int i;

	if (1 > p_iLen || !p_pucOutput) return 0x01;

	for (i = 0; i < p_iLen; i++)
		(p_pucOutput)[p_iLen - i - 1] = (unsigned char) (p_ulInput >> ((8 * i) & 0xFF));

	return 0x00;
}

unsigned char ucByteToInt (unsigned char *p_pucInput, int p_iLen, int *p_piOutput)
{
	int i;

	if (!p_pucInput) return 0x01;

	if (p_iLen > sizeof (int))
		return 0x02;

	*p_piOutput = 0;
	for (i = 0; i < p_iLen; i++) 
	{
		*p_piOutput += p_pucInput[i];
		if (i < p_iLen - 1) *p_piOutput = *p_piOutput << 8;
	}

	return 0x00;
}

unsigned char ucByteToUShort (unsigned char *p_pucInput, int p_iLen, unsigned short *p_pusOutput)
{
	int i;

	if (!p_pucInput) return 0x01;

	if (p_iLen > sizeof (unsigned short))
		return 0x02;

	*p_pusOutput = 0;
	for (i = 0; i < p_iLen; i++) 
	{
		*p_pusOutput += p_pucInput[i];
		if (i < p_iLen - 1) *p_pusOutput = *p_pusOutput << 8;
	}

	return 0x00;
}

unsigned char ucByteToULong (unsigned char *p_pucInput, int p_iLen, unsigned long *p_pulOutput)
{
	int i;

	if (!p_pucInput) return 0x01;

	if (p_iLen > sizeof (unsigned long))
		return 0x02;

	*p_pulOutput = 0;
	for (i = 0; i < p_iLen; i++) 
	{
		*p_pulOutput += p_pucInput[i];
		if (i < p_iLen - 1) *p_pulOutput = *p_pulOutput << 8;
	}

	return 0x00;
}

unsigned char ucHexToByte (char *p_pcInput, unsigned char *p_pucOutput, int *p_piLen)
{
	int iLen, iPos = 0, iLeft, iRight, iIdx;

	if (!p_pcInput) return 0x01;

	iLen = strlen (p_pcInput);

	for (iIdx = 0; iIdx < iLen; iIdx++, iPos++) 
	{
		iLeft = p_pcInput[iIdx++];
		iRight = p_pcInput[iIdx];

		iLeft = (iLeft < 58) ? (iLeft - 48) : (iLeft - 55);
		iRight = (iRight < 58) ? (iRight - 48) : (iRight - 55);

        p_pucOutput[iPos] = (iLeft << 4) | iRight;
	}

	*p_piLen = iPos;

	return 0x00;
}

unsigned char ucByteToHex (unsigned char *p_pucInput, int p_iLen, char *p_pcOutput)
{
	int iLeft, iRight, iPos = 0, iLoop;

	if (p_iLen < 0) return 0x01;

	for (iLoop = 0; iLoop < p_iLen; iLoop++) 
	{
		iLeft = (p_pucInput[iLoop] & 0xF0) >> 4;
		iRight = p_pucInput[iLoop] & 0x0F;
		p_pcOutput[iPos++] = (iLeft < 10) ? iLeft + 48 : iLeft + 55;
		p_pcOutput[iPos++] = (iRight < 10) ? iRight + 48 : iRight + 55;
	}
	p_pcOutput[iPos] = 0;

	return 0x00;
}

unsigned char ucNumToInt (unsigned char *p_pucNum, int p_iSize, int p_iBase, int *p_piOutput)
{
	int				iIndex,
					iMult			= 1,
					iRet			= 0;

	unsigned char	ucLN,
					ucRN;

	for (iIndex = p_iSize - 1; iIndex != -1; iIndex--)
	{
		ucLN = (unsigned char)(*(p_pucNum + iIndex) & 0xF0);
		ucRN = (unsigned char)(*(p_pucNum + iIndex) & 0x0F);
		ucLN >>= 4;
		
		iRet += ucRN * iMult;
		iMult *= p_iBase;
		iRet += ucLN * iMult;
		iMult *= p_iBase;
	}	

	*p_piOutput = iRet;

	return 0x00;
}

unsigned char ucGenLRC (unsigned char *p_pucData, int p_iLen, unsigned char *p_pucLRC)
{
	unsigned char	ucRet		= 0x00;
	
	int				iIndex;

	for (iIndex = 0; iIndex < p_iLen; iIndex++)
	{
		ucRet ^= *(p_pucData + iIndex);
	}

	*p_pucLRC = ucRet;

	return 0x00;
}

unsigned char ucGetValue (char *p_pcBuf, int *p_piIdx, char *p_pcField, char *p_pcValue)
{
	char *pcTemp = 0;
	int iLen, iIdx;

	if (!p_pcBuf || !p_pcField || *p_piIdx < 0 || !p_pcValue) return 0x01;

	iLen = strlen (p_pcField);
	iIdx = 0;

	pcTemp = strstr (p_pcBuf + *p_piIdx, p_pcField);

	if (0 == pcTemp)	return 0x02;

	*p_piIdx = pcTemp - p_pcBuf + 1;

	pcTemp = pcTemp + iLen;

	while (pcTemp[iIdx] != '<') 
	{
		p_pcValue[iIdx] = pcTemp[iIdx];
		iIdx++;
	}

	*p_piIdx = *p_piIdx + iLen + iIdx;
	p_pcValue[iIdx] = 0;

	return 0x00;
}

unsigned char ucReplace (char *p_pcInput, char *p_pcFindStr, char *p_pcRepStr)
{
	char	*pcInput = 0;
	char	*pcTemp = 0;
	int		iCurIdx = 0, iPreIdx = 0, iPos = 0, iFindLen, iRepLen, iBufLen;

	if (!p_pcInput || !p_pcFindStr || !p_pcRepStr) return 0x01;

	STR_COPY (pcInput, p_pcInput);

	iFindLen = strlen (p_pcFindStr);
	iRepLen = strlen (p_pcRepStr);
	iBufLen = strlen (pcInput);

	while (1)
	{
		pcTemp = strstr (pcInput + iPreIdx, p_pcFindStr);
		if (!pcTemp)
		{
			strcpy (p_pcInput + iPos, pcInput + iPreIdx);
			break;
		}

		iCurIdx = pcTemp - pcInput;

		strncpy (p_pcInput + iPos, pcInput + iPreIdx, iCurIdx - iPreIdx);
		iPos += iCurIdx - iPreIdx;

		strcpy (p_pcInput + iPos, p_pcRepStr);
		iPos += iRepLen;

		iPreIdx = iCurIdx + iFindLen;
	}

	MEM_CLEAN (pcInput);

	return 0x00;
}

unsigned long ulDirExits (char *p_pcPath)
{
	if (0 != p_pcPath)
	{
		unsigned long		ulRet		= 1;

		#if defined (WIN32)

			HANDLE			fdFindFile	= 0;
			WIN32_FIND_DATA	stFileData;

			fdFindFile = FindFirstFile (p_pcPath, &stFileData);

			if (INVALID_HANDLE_VALUE == fdFindFile || 0 == fdFindFile)
			{
				ulRet = GetLastError ();	// Retrieve windows error code
				ulRet = (0 == ulRet) ? 3 : ulRet;	// If windows error code is not available then set it to 3
			}
			else
			{
				FindClose (fdFindFile); fdFindFile = 0;
				ulRet = 0;
			}

		#elif defined (LINUX) || defined (UCLINUX)

			DIR		*fdFindFile	= 0;

			fdFindFile = opendir (p_pcPath);

			if (0 == fdFindFile)
			{
				ulRet = 2;	// Directory or file not exists
			}
			else
			{
				closedir (fdFindFile); fdFindFile = 0;
				ulRet = 0;
			}
		#endif

		return ulRet;
	}
	else
	{
		return 1;
	}
}

unsigned long ulFileExits (char *p_pcPath)
{
	if (0 != p_pcPath)
	{
		unsigned long		ulRet		= 1;

		#if defined (WIN32)

			HANDLE			fdFindFile	= 0;
			WIN32_FIND_DATA	stFileData;

			fdFindFile = FindFirstFile (p_pcPath, &stFileData);

			if (INVALID_HANDLE_VALUE == fdFindFile || 0 == fdFindFile)
			{
				ulRet = GetLastError ();	// Retrieve windows error code
				ulRet = (0 == ulRet) ? 3 : ulRet;	// If windows error code is not available then set it to 3
			}
			else
			{
				FindClose (fdFindFile); fdFindFile = 0;
				ulRet = 0;
			}

		#elif defined (LINUX) || defined (UCLINUX)


			FILE	*fdFindFile	= 0;

			FOPEN (fdFindFile, p_pcPath, "r");

			if (0 == fdFindFile)
			{
				ulRet = 2;	// Directory or file not exists
			}
			else
			{
				FCLOSE (fdFindFile);
				ulRet = 0;
			}
		#endif

		return ulRet;
	}
	else
	{
		return 1;
	}
}

unsigned char ucGetFolder (char *p_pcFullPath, char **p_ppcFolder)
{
	char	*pcBuffer	= 0,
			*pcPointer	= 0;

	if (0 == p_pcFullPath || 0 == p_ppcFolder)
	{
		return 0x01;
	}
	else
	{
		if ((char) 0 == *p_pcFullPath)
		{
			return 0x01;
		}

		*p_ppcFolder = 0;
	}

	pcPointer = strrchr (p_pcFullPath, OS_PATH_SEP);

	if (0 != pcPointer)
	{
		MEM_ALLOC_EX (pcBuffer, (sizeof (char) * (pcPointer - p_pcFullPath + 1)), (char *));

		if (0 == pcBuffer)
		{
			return 0x02;
		}

		strncpy (pcBuffer, p_pcFullPath, (pcPointer - p_pcFullPath));
		*(pcBuffer + (pcPointer - p_pcFullPath)) = (char) 0;
	}
	else
	{
		MEM_ALLOC_EX (pcBuffer, (sizeof (char) * (strlen (p_pcFullPath) + 1)), (char *));

		if (0 == pcBuffer)
		{
			return 0x02;
		}
			
		strcpy (pcBuffer, p_pcFullPath);
	}

	*p_ppcFolder = pcBuffer;

	return 0x00;
}

unsigned char ucGetFileName (char *p_pcFullPath, char **p_ppcFileName)
{
	char	*pcBuffer	= 0,
			*pcStart	= 0,
			*pcEnd		= 0;

	if (0 == p_pcFullPath || 0 == p_ppcFileName)
	{
		return 0x01;	// Pass in parameter(s) is not correct
	}
	else
	{
		if ((char) 0 == *p_pcFullPath)
		{
			return 0x01;	// Pass in parameter(s) is not correct
		}
	
		*p_ppcFileName = 0;
	}

	pcStart = strrchr (p_pcFullPath, OS_PATH_SEP);
	pcEnd = strrchr (p_pcFullPath, '.');

	if (0 != pcStart && 0 != pcEnd)
	{
		pcStart++;

		if (pcStart < pcEnd)
		{
			MEM_ALLOC_EX (pcBuffer, (sizeof (char) * (pcEnd - pcStart + 1)), (char *));

			if (0 == pcBuffer)
			{
				return 0x02;	// Insufficient memory
			}
			
			strncpy (pcBuffer, pcStart, (pcEnd - pcStart));
			*(pcBuffer + (pcEnd - pcStart)) = (char) 0;
		}
		else
		{
			if ((char) 0 != *pcStart)
			{
				MEM_ALLOC_EX (pcBuffer, (sizeof (char) * (strlen (pcStart) + 1)), (char *));

				if (0 == pcBuffer)
				{
					return 0x02;	// Insufficient memory
				}
				
				strcpy (pcBuffer, pcStart);
			}
		}
	}
	else if (0 != pcStart && 0 == pcEnd)
	{
		pcStart++;

		if ((char) 0 != *pcStart)
		{
			MEM_ALLOC_EX (pcBuffer, (sizeof (char) * (strlen(pcStart) + 1)), (char *));

			if (0 == pcBuffer)
			{
				return 0x02;	// Insufficient memory
			}
				
			strcpy (pcBuffer, pcStart);
		}
	}
	else if (0 == pcStart && 0 != pcEnd)
	{
		pcStart = p_pcFullPath;

		MEM_ALLOC_EX (pcBuffer, (sizeof (char) * (pcEnd - pcStart + 1)), (char *));

		if (0 == pcBuffer)
		{
			return 0x02;	// Insufficient memory
		}
		
		strncpy (pcBuffer, pcStart, (pcEnd - pcStart));
		*(pcBuffer + (pcEnd - pcStart)) = (char) 0;
	}
	else
	{
		MEM_ALLOC_EX (pcBuffer, (sizeof (char) * (strlen (p_pcFullPath) + 1)), (char *));

		if (0 == pcBuffer)
		{
			return 0x02;	// Insufficient memory
		}
		
		strcpy (pcBuffer, p_pcFullPath);
	}

	*p_ppcFileName = pcBuffer;

	return 0x00;
}

unsigned char ucGetExtension (char *p_pcFullPath, char **p_ppcExt)
{
	char	*pcBuffer	= 0,
			*pcStart	= 0,
			*pcEnd		= 0;

	if (0 == p_pcFullPath || 0 == p_ppcExt)
	{
		return 0x01;
	}
	else
	{
		if ((char) 0 == *p_pcFullPath)
		{
			return 0x01;
		}

		*p_ppcExt = 0;
	}

	pcStart = strrchr (p_pcFullPath, '.');

	if (0 != pcStart)
	{
		pcEnd = strrchr (p_pcFullPath, OS_PATH_SEP);

		pcStart++;

		if (0 != pcEnd)
		{
			if (pcStart > pcEnd)
			{
				if ((char) 0 != *pcStart)
				{
					MEM_ALLOC_EX (pcBuffer, (sizeof (char) * (strlen (pcStart) + 1)), (char *));

					if (0 == pcBuffer)
					{
						return 0x02;
					}

					strcpy (pcBuffer, pcStart);
				}
			}
		}
		else
		{
			if ((char) 0 != *pcStart)
			{
				MEM_ALLOC_EX (pcBuffer, (sizeof (char) * (strlen (pcStart) + 1)), (char *));

				if (0 == pcBuffer)
				{
					return 0x02;
				}

				strcpy (pcBuffer, pcStart);
			}
		}
	}

	*p_ppcExt = pcBuffer;

	return 0x00;
}

unsigned char ucGenRandom (int p_iLength, char **p_ppcBuffer)
{
	char			*pcRandom		= 0;
	
	unsigned int	uiRandom1,
					uiRandom2;

	int				iLoop,
					iRandom;

	if (0 == p_ppcBuffer || 0 >= p_iLength)
	{
		return 0x01;
	}

	MEM_ALLOC (pcRandom, (sizeof (char) * (p_iLength + 1)), (char *));

	if (0 == pcRandom)
	{
		return 0x02;
	}

	uiRandom1 = rand () + 1;
	uiRandom1 = uiRandom1 % 7193;

	uiRandom2 = rand () + 1;
	uiRandom2 = uiRandom2 % 3917;

	srand (((unsigned) time (0)) + uiRandom1 + uiRandom2);

	for (iLoop = 0; iLoop < p_iLength; iLoop++)
	{
		iRandom = (rand () + 1) % (rand () + 1);
		iRandom = iRandom % 35;

		if (0 <= iRandom && 9 >= iRandom)
		{
			iRandom += 48;
		}
		else
		{
			iRandom = (iRandom - 9) + 64;
		}

		*(pcRandom + iLoop) = iRandom;
	}

	*p_ppcBuffer = pcRandom;
	
	return 0x00;
}

unsigned char ucGenNumber (int p_iLength, char **p_ppcBuffer)
{
	char			*pcRandom		= 0;
	
	unsigned int	uiRandom1,
					uiRandom2;

	int				iLoop,
					iRandom;

	if (0 == p_ppcBuffer || 0 >= p_iLength)
	{
		return 1;
	}

	MEM_ALLOC (pcRandom, (sizeof (char) * (p_iLength + 1)), (char *));

	if (0 == pcRandom)
	{
		return 0x02;
	}

	uiRandom1 = rand () + 1;
	uiRandom1 = uiRandom1 % 7193;

	uiRandom2 = rand () + 1;
	uiRandom2 = uiRandom2 % 3917;

	srand (((unsigned) time (0)) + uiRandom1 + uiRandom2);

	for (iLoop = p_iLength-1; 0 <= iLoop; iLoop--)
	{
		iRandom = (rand () + 1) % (rand () + 1);
		iRandom = iRandom % 9;
		iRandom += 48;

		*(pcRandom + iLoop) = iRandom;
	}

	*p_ppcBuffer = pcRandom;
	
	return 0x00;
}

unsigned long ulGetAppPath (char **p_ppcPath)
{
	unsigned long	ulRet						= 1;

	char			acProgFileName [512]		= { 0 },
					*pcFolder					= 0;

#ifdef CPP_COMPILER_V2
	unsigned long	ulStart						= 0,
					ulEnd						= 0;

	char			acLine [1024]				= { 0 },
					*pcPath						= 0,
					*pcPtr						= 0;

	void			*pSymbol					= (void *) "";

	FILE			*fdFile						= 0;
#endif

	if (0 == p_ppcPath)
	{
		ulRet = 1;	// Pass in parameter(s) is not correct
		goto CleanUp;
	}
	else
	{
		*p_ppcPath = 0;
	}

	#if defined (WIN32)

		if (0 != GetModuleFileName (0, acProgFileName, sizeof (acProgFileName) - 1))
		{
			ulRet = ucGetFolder (acProgFileName, &pcFolder);

			if (0 != ulRet || 0 == pcFolder)
			{
				ulRet = 3;	// System proess format is not correct
			}
			else
			{
				*p_ppcPath = pcFolder; pcFolder = 0;
				ulRet = 0;
			}
		}
		else
		{
			ulRet = 4;	// Application path is not found
		}

	#elif defined (LINUX) || defined (UCLINUX)

		#ifdef CPP_COMPILER_V2
			
			FOPEN (fdFile, "/proc/self/maps", "r");

			if (0 != fdFile)
			{
				while (!feof (fdFile))
				{
					if (!fgets (acLine, sizeof (acLine), fdFile))
					{
						SLEEP (1);
						continue;
					}

					if (!strstr (acLine, " r-xp ") || !strchr (acLine, OS_PATH_SEP))
					{
						SLEEP (1);
						continue;
					}

					sscanf (acLine, "%lx-%lx ", &ulStart, &ulEnd);

					if (pSymbol >= (void *) ulStart && pSymbol < (void *) ulEnd)
					{
						pcPath = strchr (acLine, OS_PATH_SEP);

						if (0 != pcPath)
						{
							pcPtr = strchr (pcPath, '\n');

							if (0 != pcPtr)
							{
								*pcPtr = (char) 0;
							}

							MEM_CLEAN (pcFolder);

							ulRet = ucGetFolder (pcPath, &pcFolder);

							if (0 != ulRet || 0 == pcFolder)
							{
								ulRet = 3;	// System proess format is not correct
							}
							else
							{
								ulRet = 0;
							}
						}
						else
						{
							ulRet = 3;	// System proess format is not correct
						}

						break;
					}
				}

				FCLOSE (fdFile);

				if (0 == pcFolder)
				{
					ulRet = 4;	// Application path is not found
				}
				else
				{
					*p_ppcPath = pcFolder; pcFolder = 0;
					ulRet = 0;
				}
			}
			else
			{
				ulRet = 2;	// System process file is not found
			}
		#else
			readlink ("/proc/self/exe", acProgFileName, sizeof (acProgFileName) - 1); 

			ulRet = ucGetFolder (acProgFileName, &pcFolder);

			if (0 != ulRet || 0 == pcFolder)
			{
				ulRet = 3;	// System proess format is not correct
			}
			else
			{
				*p_ppcPath = pcFolder; pcFolder = 0;
				ulRet = 0;
			}
		#endif
	#endif

CleanUp:
	MEM_CLEAN (pcFolder);

	return ulRet;
}

unsigned long ulEncodePath (char *p_pcFullPath, char *p_pcFmtPath, char **p_ppcRet)
{
	unsigned long	ulRet;

	int				iBackward,
					iIndex;

	char			acSep [2]		= { 0 },
					acSepEx [2]		= { 0 },
					*pcFmtPtr,
					*pcFullPtr;

	char			clsRet[512] = { 0x00 };

	if (0 == p_pcFullPath || 0 == p_pcFmtPath || 0 == p_ppcRet)
	{
		ulRet = 1;	// Pass in parameter(s) is not correct
		goto CleanUp;
	}
	else
	{
		*p_ppcRet = 0;
	}

	if ('.' == *p_pcFmtPath)
	{
		iBackward = 0;

		for (pcFmtPtr = p_pcFmtPath; (char ) 0 != *pcFmtPtr;)
		{
			if ('.' == *pcFmtPtr)
			{
				pcFmtPtr++;

				if ('.' == *pcFmtPtr)
				{
					iBackward++;
					pcFmtPtr++;
				}
				else if (OS_PATH_SEP == *pcFmtPtr || OS_PATH_SEP_EX == *pcFmtPtr)
				{
					pcFmtPtr++;
				}
				else
				{
					break;
				}
			}
			else if (OS_PATH_SEP == *pcFmtPtr || OS_PATH_SEP_EX == *pcFmtPtr)
			{
				pcFmtPtr++;
			}
			else
			{
				break;
			}
		}

		pcFullPtr = p_pcFullPath;

		if (0 < iBackward)
		{
			pcFullPtr += strlen (p_pcFullPath);

			// Get the cut off from full path
			for (iIndex = iBackward, pcFullPtr--; pcFullPtr >= p_pcFullPath; pcFullPtr--)
			{
				if (OS_PATH_SEP == *pcFullPtr || OS_PATH_SEP_EX == *pcFullPtr)
				{
					if (1 < iIndex)
					{
						iIndex--;
					}
					else
					{
						break;
					}
				}
			}
		}

		if (0 < (pcFullPtr - p_pcFullPath))
		{
			strncpy (clsRet,  p_pcFullPath, pcFullPtr - p_pcFullPath);
		}
		else if (pcFullPtr == p_pcFullPath)
		{
			strcpy (clsRet, p_pcFullPath);
		}
		else
		{
			#if defined (WIN32)

				strncpy (clsRet, p_pcFullPath, 1);
				strcat (clsRet, ":");
			
			#endif
		}

		if (OS_PATH_SEP != *pcFmtPtr || OS_PATH_SEP_EX != *pcFmtPtr)
		{
			strcat (clsRet, "\\");
		}

		strcat (clsRet, pcFmtPtr);

		acSep [0] = OS_PATH_SEP;
		acSepEx [0] = OS_PATH_SEP_EX;

		ucReplace(clsRet, acSepEx, acSep);

		STR_COPY (*p_ppcRet, clsRet);
	}
	else
	{
		pcFmtPtr = p_pcFmtPath;

		while ((char) 0 != *pcFmtPtr)
		{
			if (OS_PATH_SEP_EX == *pcFmtPtr)
			{
				*pcFmtPtr = OS_PATH_SEP;
			}

			pcFmtPtr++;
		}

		STR_COPY (*p_ppcRet, p_pcFmtPath);
	}


	ulRet = 0;

CleanUp:

	return ulRet;
}

unsigned char at_match_voice (char *p_pcStr, char *p_pcMatch)
{
	char			acBuf [256],
					*pcStart		= 0,
					*pcTab			= 0;

	pcStart = p_pcMatch;
	
	while (0 != pcStart)
	{
		memset (acBuf, 0x00, sizeof (acBuf));

		pcTab = strchr (pcStart, 9);	// Find for TAB

		if (0 != pcTab)
		{
			if (pcTab != pcStart)
			{
				strncpy (acBuf, pcStart, pcTab - pcStart);
			}

			pcStart = (0 != *(++pcTab)) ? pcTab : 0;
		}
		else
		{
			strcpy (acBuf, pcStart);
			pcStart = 0;
		}

		if (0 != acBuf [0] && 0 != strstr (p_pcStr, acBuf))
		{
			return 0x00;
		}
	}

	return 0x01;
}
