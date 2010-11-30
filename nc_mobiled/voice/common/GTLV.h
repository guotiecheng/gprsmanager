#ifndef GTLV_H__464A4C7E_DA2A_4D52_92BD_FA8434DB5633__INCLUDED_
#define GTLV_H__464A4C7E_DA2A_4D52_92BD_FA8434DB5633__INCLUDED_

#include <stdlib.h>
#include <string.h>

#include "GMaster.h"

typedef struct GTLV_NODE_T
{
	int					iTag,					// Tag (2 bytes)
						iLen;					// Length (2 bytes)

	unsigned char		*pucValue;

	struct GTLV_NODE_T	*pstHead,
						*pstTail;
} GTLV_NODE;

typedef struct GTLV_DATA_T
{
	int					iMstCnt,				// The number of tag in master list
						iRcyCnt,
						
						iTagDef;				// Define which method to parse the tag

	unsigned long		ulDataLen;				// Optimize the performance when building raw data from TLV

	struct GTLV_NODE_T	*pstMstFront,
						*pstMstRear,
						*pstRcyFront,
						*pstRcyRear;
} GTLV_DATA;


// ==============
// ** CONSTANT **
// ==============

#define	GTLV_OPTION_TAG_CODE					1

#define	GTLV_TAG_STANDARD_CODE					0
#define	GTLV_TAG_1_BYTE_CODE					1
#define	GTLV_TAG_2_BYTE_CODE					2

#ifdef __cplusplus
extern "C" {
#endif

// Functions
void gtlv_init (GTLV_DATA *p_pstTLV);
void gtlv_term (GTLV_DATA *p_pstTLV);
void gtlv_reset (GTLV_DATA *p_pstTLV);

unsigned char gtlv_set_opt (GTLV_DATA *p_pstTLV, int p_iOptID, unsigned char *p_pucOptPtr, unsigned int p_uiOptValLen);


unsigned char gtlv_parse (unsigned char *p_pucRaw, unsigned long p_ulLen, GTLV_DATA *p_pstTLV, unsigned long *p_pulErrPos);
unsigned char gtlv_build (GTLV_DATA *p_pstTLV, unsigned char **p_ppucRaw, unsigned long *p_pulLen);

unsigned char gtlv_get (GTLV_DATA *p_pstTLV, int p_iTag, unsigned char **p_ppucData, int *p_piLen, unsigned char p_ucSkip);
unsigned char gtlv_set (GTLV_DATA *p_pstTLV, int p_iTag, unsigned char *p_pucData, int p_iLen);

unsigned char gtlv_add (GTLV_DATA *p_pstTLV, int p_iTag, unsigned char *p_pucData, int p_iLen, unsigned char p_ucAllowDupl);
unsigned char gtlv_del (GTLV_DATA *p_pstTLV, int p_iTag);

#ifdef __cplusplus 
}
#endif 

#endif
