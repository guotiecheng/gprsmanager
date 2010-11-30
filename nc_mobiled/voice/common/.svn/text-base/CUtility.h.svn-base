#if !defined(_UTILITY_H_)
#define _UTILITY_H_

#include "GMaster.h"

#if defined (WIN32)

	#include <windows.h>

#elif defined (LINUX) || defined (UCLINUX)

	#include <stdio.h>
	#include <dirent.h>
	#include <unistd.h>

#endif

	// Data Conversion
	unsigned char	ucIntToByte (int p_iInput, int p_iLen, unsigned char *p_pucOutput);
	unsigned char	ucByteToInt (unsigned char *p_pucInput, int p_iLen, int *p_piOutput);
	unsigned char	ucULongToByte (unsigned long p_ulInput, int p_iLen, unsigned char *p_pucOutput);
	unsigned char	ucByteToULong (unsigned char *p_pucInput, int p_iLen, unsigned long *p_pulOutput);

	unsigned char	ucUShortToByte (unsigned short p_usInput, int p_iLen, unsigned char *p_pucOutput);
	unsigned char	ucByteToUShort (unsigned char *p_pucInput, int p_iLen, unsigned short *p_pusOutput);

	unsigned char	ucHexToByte (char *p_pcInput, unsigned char *p_pucOutput, int *p_piLen);
	unsigned char	ucByteToHex (unsigned char *p_pucInput, int p_iLen, char *p_pcOutput);
	unsigned char	ucNumToInt (unsigned char *p_pucNum, int p_iSize, int p_iBase, int *p_piOutput);
	
	unsigned char	ucGenLRC (unsigned char *p_pucData, int p_iLen, unsigned char *p_pucLRC);

	unsigned char	ucReplace (char *p_pcInput, char *p_pcFindStr, char *p_pcRepStr);

	// Folder and File name operation
	unsigned long	ulDirExits (char *p_pcPath);

	unsigned long	ulFileExits (char *p_pcPath);
	unsigned long	ulFileExitsEx (char *p_pcFolder, char *p_pcFileName, char *p_pcExt);

	unsigned char	ucGetFolder (char *p_pcFullPath, char **p_ppcFolder);
	unsigned char	ucGetFileName (char *p_pcFullPath, char **p_ppcFileName);
	unsigned char	ucGetExtension (char *p_pcFullPath, char **p_ppcExt);

	unsigned long	ulGetAppPath (char **p_ppcPath);

	unsigned long	ulEncodePath (char *p_pcFullPath, char *p_pcFmtPath, char **p_ppcRet);

	// Random generator
	unsigned char	ucGenRandom (int p_iLength, char **p_ppcBuffer);
	unsigned char	ucGenNumber (int p_iLength, char **p_ppcBuffer);

	// String operation
	unsigned char	at_match_voice (char *p_pcStr, char *p_pcMatch);

#endif // !defined(_UTILITY_H_)
