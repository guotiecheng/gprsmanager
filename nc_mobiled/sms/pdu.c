#include <common/common.h>

unsigned  char ucRet = 0x00;

unsigned  char pk_total = 0 , pk_rcv_total = 0;
unsigned  char pk_num = 1 , pk_rcv_num = 0;
unsigned  char sms_ref_id = 0;
unsigned  char sms_ref_rcv_id = 0;

extern const unsigned char singlebytecharacter[127];
extern const unsigned char doublebytecharacter[103];

char recv_buf[SIZE_1024];

void get_sms_ref_id()
{
	if (sms_ref_id < 255)
		sms_ref_id ++;
	else
		sms_ref_id = 1;
}

#if 0
int Init_SMS()
{
	ucRet = send_atcmd(&comport, "AT+CMGF=0\r", "OK", "ERROR", 0, 0, 5000, recv_buf);
	if(0x00 != ucRet)
	{
	        logs(LOG_ERRO, "Send AT command \"AT+CMGF=0\" failure\n"); 
		return -1;
	}
	return 0;
}
#endif

int gsmEncode7bit(const char* pSrc, unsigned char* pDst, int nSrcLength)
{
    int nSrc = 0;      
    int nDst = 0;       
    int nChar = 0;       
    unsigned char nLeft = 0;   
    
    nSrc = 0;
    nDst = 0;
    
    while(nSrc<nSrcLength)
    {
        nChar = nSrc & 7;
    
        if(nChar == 0)
        {
            nLeft = *pSrc;
        }
        else
        {
            *pDst = (*pSrc << (8-nChar)) | nLeft;
            nLeft = *pSrc >> nChar;
            pDst++;
            nDst++; 
        } 
        
        pSrc++; 
        nSrc++;
    }

    return nDst; 
    //return nSrcLength;
}
    

int gsmDecode7bit(const unsigned char* pSrc, char* pDst, int nSrcLength)
{
    int nSrc = 0;        
    int nDst = 0;        
    int nByte = 0;       
    unsigned char nLeft = 0;    
    
    while(nSrc<nSrcLength)
    {
        *pDst = ((*pSrc << nByte) | nLeft) & 0x7f;
        nLeft = *pSrc >> (7-nByte);
    
        pDst++;
        nDst++;
        nByte++;
    
        if(nByte == 7)
        {
            *pDst = nLeft;
  
            pDst++;
            nDst++;
            nByte = 0;
            nLeft = 0;
        }
    
        pSrc++;
        nSrc++;
    }
    
    *pDst = '\0';
    
    return nDst;
}

void GSM2ASCII(const unsigned char* pSrc, unsigned char* pDst, int nSrcLength)
{
	int i;

	for(i = 0; i < nSrcLength; i++)
	{
		if(*pSrc != 27)    //so this character is single byte character
		{
			*pDst = singlebytecharacter[*pSrc];
			pDst ++;
		}
		else              //so this character is double bytes character
		{
			pSrc ++;
			*pDst = doublebytecharacter[*pSrc];
			pDst ++;
			nSrcLength --;
		}
		pSrc ++;
	}

	*pDst = '\0';
}

int gsmEncode8bit(const char* pSrc, unsigned char* pDst, int nSrcLength)
{
	memcpy(pDst, pSrc, nSrcLength);
	return nSrcLength;
}


int gsmDecode8bit(const unsigned char* pSrc, char* pDst, int nSrcLength)
{
	memcpy(pDst, pSrc, nSrcLength);
	*(pDst+ nSrcLength)= '\0';

	return nSrcLength;
}

#if 0
int EncodeWithUnicodeAlphabet(const char *src, wchar_t *dest)
{
    int retval; 
    
    switch (retval = mbtowc(dest, (const char*)src, MB_CUR_MAX)) 
    { 
         case -1 : 
         case  0 : 
               return 1; 
         
         default : 
               return retval; 
    } 
}

int gsmEncodeUcs2(const char* pSrc, unsigned char* pDst, int nSrcLength)
{
    int             i_len = 0, o_len;
    wchar_t         wc;

    for (o_len = 0; i_len < nSrcLength; o_len++)
    {
         i_len += EncodeWithUnicodeAlphabet(&pSrc[i_len], &wc); 
         pDst[o_len*2]           = (wc >> 8) & 0xff;
         pDst[(o_len*2)+1]       = wc & 0xff;
    }

    pDst[o_len*2]           = 0;
    pDst[(o_len*2)+1]       = 0;

    return 2*o_len;
}
    

int DecodeWithUnicodeAlphabet(wchar_t src, char *dest)
{
    int retval;

    if(setlocale(LC_ALL, "") == NULL)
    {
	    IPRINT("setlocale error!\n");
	    return 1;
    }

    switch (retval = wctomb(dest, src))
    { 
          case -1: 
               *dest = '?'; 
	       IPRINT("change failure !\n");
               return 1; 
          
          default: 
	       IPRINT("change successfully!\n");
               return retval;
    }
}

int gsmDecodeUcs2(const unsigned char* pSrc, char* pDst, int nSrcLength)
{
     int             i=0,o=0;
     wchar_t         wc;

     while (pSrc[(2*i)+1]!=0x00 || pSrc[2*i]!=0x00)
     {
          wc = pSrc[(2*i)+1] | (pSrc[2*i] << 8);
          o += DecodeWithUnicodeAlphabet(wc, pDst + o);
          i++;
     }

     pDst[o]=0;

     return nSrcLength/2;

}
#endif

int gsmEncodeUcs2(char* pSrc, unsigned char* pDst, int nSrcLength)
{
	iconv_t cd;

	size_t nOutLength = 1024;
	size_t nInLength = nSrcLength;
	char out[1024];
	memset(out,0,sizeof(out));
	char *pOut = out;
	char *pIn = pSrc;

//	unsigned char *tmpDst = pDst;

	int count = 0;

	cd = iconv_open("UCS-2BE", "GB2312") ;
	if ((iconv_t)-1 == cd)
	{
//		logs(LOG_ERRO, "can't iconv_open \n");
		nc_log (&logger, NC_MOBILED_NAME, NA_LOG_ERROR, "@%04d Mobiled SMS thread:Can't iconv_open!", __LINE__);
		return -1;
	}

	if (iconv(cd, (char**)&pIn, &nInLength, &pOut, &nOutLength) == -1)
	{
	//	logs(LOG_ERRO, "can't iconv\n");
		nc_log (&logger, NC_MOBILED_NAME, NA_LOG_ERROR, "@%04d Mobiled SMS thread:Can't iconv!", __LINE__);
		return -1;
	}
//	logs(LOG_INFO, "iconv function finished!\n");
	nc_log (&logger, NC_MOBILED_NAME, NA_LOG_ERROR, "@%04d Mobiled SMS thread:Iconv function finished!", __LINE__);

	count = 1024 - nOutLength;
	memcpy(pDst, out, count);
	pDst[count] = '\0';

	iconv_close(cd);
	return count;
}

int gsmDecodeUcs2(unsigned char* pSrc, char* pDst, int nSrcLength)
{
	iconv_t cd;

	size_t nBuffLen = 1024;
	size_t nOutLength = nBuffLen;	
	size_t nInLength = nSrcLength;
	char out[1024];
	memset(out,0,sizeof(out));
	
	char *pOut = out;
	char *pIn = (char *)pSrc;

	int count = 0;

	cd = iconv_open("GB2312", "UCS-2BE") ; //== ((iconv_t)-1)  
	if ((iconv_t)-1 == cd)
	{
		return -1;
	}

	if (iconv(cd, (char**)&pIn, &nInLength, &pOut, &nOutLength) == -1)
	{
		return -1;
	}

	count = nBuffLen - nOutLength;	

	memcpy(pDst, out, count);
	pDst[count] = '\0';

	iconv_close(cd);
	return count;
}

// Convert  "C8329BFD0E01" to {0xC8, 0x32, 0x9B, 0xFD, 0x0E, 0x01}
// pSrc: Source String Pointer
// pDst: Destination Data Pointer
// nSrcLength: Source String length
// Return:   Destination Data length
int gsmString2Bytes(const char* pSrc, unsigned char* pDst, int nSrcLength)
{
    int    i = 0;
    for(i=0; i<nSrcLength; i+=2)
    {
        if(*pSrc>='0' && *pSrc<='9')
        {
            *pDst = (*pSrc - '0') << 4;
        }
        else
        {
            *pDst = (*pSrc - 'A' + 10) << 4;
        }
    
        pSrc++;
    
        if(*pSrc>='0' && *pSrc<='9')
        {
            *pDst |= *pSrc - '0';
        }
        else
        {
            *pDst |= *pSrc - 'A' + 10;
        }
        pSrc++;
        pDst++;
    }
    
    return (nSrcLength / 2);
}
    

// Convert {0xC8, 0x32, 0x9B, 0xFD, 0x0E, 0x01} to "C8329BFD0E01" 
// pSrc: Source Data length
// pDst: Destination String Length
// nSrcLength: Source Data Length
// Return:  Destination String Length
int gsmBytes2String(const unsigned char* pSrc, char* pDst, int nSrcLength)
{
    const char tab[]="0123456789ABCDEF";  
    int        i = 0;
    
    for(i=0; i<nSrcLength; i++)
    {
        *pDst++ = tab[*pSrc >> 4];
        *pDst++ = tab[*pSrc & 0x0f];
    
        pSrc++;
    }
    
    *pDst = '\0';
    
    return nSrcLength * 2;
}



// Convert Address from "8613851872468" to "683158812764F8"
// pSrc: Source String Pointer
// pDst: Destination String Pointer
// nSrcLength: Source String Length,
// Return: Destination String Length
int gsmInvertNumbers(char *addrHead, const char* pSrc, char* pDst, int nSrcLength)
{
    int      nDstLength = 0;  
    char     ch;          
    int      i;
    


    nDstLength = nSrcLength;

    strncpy(pDst, addrHead, 4);

    nDstLength += 4;
    pDst += 4;

    for(i=0; i<nSrcLength;i+=2)  // Reverse
    {
        ch = *pSrc++;        
        *pDst++ = *pSrc++;   
        *pDst++ = ch;      
    }
    
    if(nSrcLength & 1) // Add 'F'
    {
        *(pDst-2) = 'F';    
        nDstLength++;      
    }
    
    *pDst = '\0';
    
    return nDstLength;
}
    

// Convert "683158812764F8" to "8613851872468"
// pSrc: Source String Pointer
// pDst: Destination String Pointer
// nSrcLength: Source String Length
// Return:  Destination String Length
int gsmSerializeNumbers(const char* pSrc, char* pDst, int nSrcLength)
{
    int     nDstLength;   
    char    ch;         
    int     i;

    nDstLength = nSrcLength;

    for(i=0; i<nSrcLength;i+=2)
    {
        ch = *pSrc++;        
        *pDst++ = *pSrc++;  
        *pDst++ = ch;    
    }
    
    if(*(pDst-1) == 'F')
    {
        pDst--;
        nDstLength--;     
    }
    
    *pDst = '\0';
   
    return nDstLength;
}


int gsmEncodePhoneNumber(unsigned char type, const char* pSrc, char *pDst, int nSrcLength)
{
  unsigned  char    addr_type;
  int               nDstLength = 0; 
  char              pduAddrHead[5];
  int               pduAddrLen = 0;

  if( '\0'==*pSrc && SCA_ADDR==type)
  {
       strcpy(pDst, "00");
       return 2;
  }

  memset(pduAddrHead, 0, sizeof(pduAddrHead));

  if('+' == *pSrc)
  {
       addr_type = INTNAL;
       pSrc++; // Skip '+'
  }
  else
  {
       addr_type = NATNAL;
  }

  pduAddrLen = strlen(pSrc);
  if( SCA_ADDR == type )
  {
       if(pduAddrLen & 1)
         pduAddrLen = (pduAddrLen+1)/2 + 1;
       else
         pduAddrLen = pduAddrLen/2 + 1;      // 1 is 91 or A1
  }


  if( INTNAL == addr_type)
  {
       snprintf(pduAddrHead, 5, "%02X%s", pduAddrLen, "91");
  }
  else if( NATNAL == addr_type)
  {
       snprintf(pduAddrHead, 5, "%02X%s", pduAddrLen, "81");
  }

  nDstLength = gsmInvertNumbers(pduAddrHead, pSrc, pDst, strlen(pSrc));

  return nDstLength;
}
   


// pSrc: Source PDU Parameter Pointer
// pDst: Target PDU string pointer
// Return:  The 'Length' send by "AT+CMGS=<Length>", which exclude SMSC address.

int gsmEncodePdu(SM_PARAM* pSrc, char* pDst, int long_sms)
{
    int               nLength = 0;           
    int               nDstLength = 0;        
    int               offset = 0,fillbits = 0;    
    unsigned char     buf[256]; 
    unsigned char     smscLength;

    smscLength =  gsmEncodePhoneNumber(SCA_ADDR, pSrc->SCA, &pDst[nDstLength], nLength);  
    nDstLength += smscLength; 

    memset(buf,0,256);
//    buf[0] = 0x11;            // TP-Type=0x11:   TP-MTI=01 TP-VPF=10
    buf[0] = pSrc->TP_TYPE;
    buf[1] = 0x00;            // TP-MR=0
    nDstLength += gsmBytes2String(buf, &pDst[nDstLength], 2); 

    nDstLength += gsmEncodePhoneNumber(TPA_ADDR, pSrc->TPA, &pDst[nDstLength], nLength);  
    

    nLength = strlen((char *)(pSrc->TP_UD));    
    buf[0] = pSrc->TP_PID;        // (TP-PID)
    buf[1] = pSrc->TP_DCS;        // (TP-DCS)
    buf[2] = 0;            // (TP-VP) for 5 minutes

    if(long_sms)
    {
	    buf[4] = 0x05;
	    buf[5] = 0x00;
	    buf[6] = 0x03;
	    buf[7] = sms_ref_id;
	    buf[8] = pk_total;
	    buf[9] = pk_num;

	    offset = 6;
    }

    if(pSrc->TP_DCS == GSM_7BIT)    
    {
	if(offset == 6)
	{
		buf[3] = nLength + offset +1;

		buf[4+offset] = (pSrc->TP_UD[0] << 1);
		fillbits += 1;
	        nLength = gsmEncode7bit((char *)(pSrc->TP_UD+fillbits), &buf[4+offset+fillbits], nLength) + 4;   
	}
	else
	{
		buf[3] = nLength + offset;
		nLength = gsmEncode7bit((char *)(pSrc->TP_UD+fillbits), &buf[4+offset+fillbits], nLength+1) + 4;
	}

	nLength += fillbits;
        nLength += offset;
    }

    else if(pSrc->TP_DCS == GSM_UCS2)
    {
        buf[3] = gsmEncodeUcs2((char *)(pSrc->TP_UD), &buf[4+offset], nLength);    // Convert TP-DA to target PDU string
        buf[3] += offset;
	nLength = buf[3] + 4;       
    }
    else
    {
        buf[3] = gsmEncode8bit((char *)(pSrc->TP_UD), &buf[4+offset], nLength);    // Convert TP-DA to target PDU string
	buf[3] += offset;
	nLength = buf[3] + 4;
    }
//    nLength += offset;
    nDstLength += gsmBytes2String(buf, &pDst[nDstLength], nLength);    
    
    return (nDstLength - smscLength)/2;
}
    
int gsmDecodePdu(const char* pSrc, SM_PARAM* pDst)
{
    int nDstLength = 0,nSrcLength = 0, long_sms = 0,head_len = 0,ref_len = 0;   
    int offset = 0;
    unsigned char tmp = 0,udl = 0;    
    unsigned char buf[1024]; 
    unsigned char TP_UD[1024]; 

    // SMSC Address information
    gsmString2Bytes(pSrc, &tmp, 2);    // Get the SMSC address length.
    tmp = (tmp - 1) * 2;    
    pSrc += 4;             
    gsmSerializeNumbers(pSrc, pDst->SCA, tmp);    // Convert SMSC address to target PDU string.
    pSrc += tmp;  

    // TPDU basic parameter, reply address etc.
    gsmString2Bytes(pSrc, &tmp, 2);    // Get the basic parameter.
    pSrc += 2;       

    if(tmp&0x40)
    {
	    long_sms = 1;
    }
    
//    if(!(tmp & 0x80))
//  {        
        gsmString2Bytes(pSrc, &tmp, 2);    //Get Reply Address
        if(tmp & 1) tmp += 1;    // Adjust Odd
        pSrc += 4;         
        gsmSerializeNumbers(pSrc, pDst->TPA, tmp);    // Get TP-RA  number
   	pSrc += tmp;    
//    }

    gsmString2Bytes(pSrc, (unsigned char*)&pDst->TP_PID, 2);    // Get (TP-PID)
    pSrc += 2;       
    gsmString2Bytes(pSrc, (unsigned char*)&pDst->TP_DCS, 2);    // Get (TP-DCS)
    pSrc += 2;    
    gsmSerializeNumbers(pSrc, pDst->TP_SCTS, 14);        // Get (TP_SCTS) 
    pSrc += 14;     
    gsmString2Bytes(pSrc, &tmp, 2);    // Get (TP-UDL)
    nSrcLength = tmp;
    pSrc += 2;   
    udl = tmp;

    if(long_sms)
    {
	    gsmString2Bytes(pSrc, &tmp, 2);    
	    head_len = tmp;
	    pSrc += 2;
	    offset += 1;

	    gsmString2Bytes(pSrc, &tmp, 2);    
	    ref_len = tmp == 0 ? 1 : 2; 
	    pSrc += 2;
	    offset += 1;

	    gsmString2Bytes(pSrc, &tmp, 2);    
	    pSrc += 2;
	    offset += 1;

	    if (ref_len == 1)
	    {
		    gsmString2Bytes(pSrc, &sms_ref_rcv_id, 2);
		    pSrc += 2;
		    offset += 1;
	    }
	    else
	    {
		    gsmString2Bytes(pSrc, &sms_ref_rcv_id, 4);
		    pSrc += 4;
		    offset += 2;
	    }

	    gsmString2Bytes(pSrc, &tmp, 2);   
	    pk_rcv_total= tmp; 
	    pSrc += 2;
	    offset += 1;
	    
	    gsmString2Bytes(pSrc, &tmp, 2);   
	    pk_rcv_num= tmp; 
	    pSrc += 2;
	    offset += 1;

	    tmp = udl - offset;

	    nSrcLength -= 7;
    }
    if(pDst->TP_DCS == GSM_7BIT)    
    {
	memset(TP_UD,0,sizeof(TP_UD));
        nDstLength = gsmString2Bytes(pSrc, buf, tmp & 7 ? (int)tmp * 7 / 4 + 2 : (int)tmp * 7 / 4); // convert format

	if(offset == 6)
	{
		offset = 1;
		buf[0] = buf[0] >> offset;
	//	gsmDecode7bit(buf, pDst->TP_UD, 1);   
		gsmDecode7bit(buf, (char *)TP_UD, 1);   
//		nDstLength -= 1;
	}
	else
		offset = 0;
	
	gsmDecode7bit(buf+offset, (char *)(TP_UD+offset), nDstLength-offset);   // Convert to  TP-DU
	nDstLength += offset;
        GSM2ASCII(TP_UD, pDst->TP_UD, nSrcLength);
    }
    else if(pDst->TP_DCS == GSM_UCS2)
    {
        nDstLength = gsmString2Bytes(pSrc, buf, tmp * 2);        // Convert Format
        nDstLength = gsmDecodeUcs2(buf, (char *)(pDst->TP_UD), nDstLength);    // Convert to  TP-DU
    }
    else // 8 bit
    {
        nDstLength = gsmString2Bytes(pSrc, buf, tmp * 2);        // Convert Format
        nDstLength = gsmDecode8bit(buf,(char *)(pDst->TP_UD), nDstLength);    // Convert to TP-DU
    }
    
    return nDstLength;
}

int gsmSendLongSMS(const SM_PARAM* pSrc, COM_PORT *sms_comport)
{
	int len = strlen((char *)(pSrc->TP_UD));
	int unit = 0,  offset = 0 , left = 0;
	char *msg ,buff[1024];
	SM_PARAM SmParam;
	int pduLength = 0;
	char pduString[SIZE_1024];  /* The last PDU string send by AT command */
    	char atcmd[SIZE_1024];
	
	memset(buff,0,sizeof(buff));
	strncpy(buff,(char *)(pSrc->TP_UD),sizeof(pSrc->TP_UD));
	msg = buff;

	if(pSrc->TP_DCS==GSM_7BIT)
		unit = 153;  //160-7
	else if(pSrc->TP_DCS==GSM_8BIT)
		unit = 134;  //140-6
	else 
		unit = 67;   //(70-3)

	pk_total = len/unit;
	if(0 != (len%unit))
		pk_total++;
	pk_num = 1;

	get_sms_ref_id();      //this id is the label of one long msg,when send a long msg,this label adds one time

	left = len;
	offset = 0;

	while(offset<len)
	{
		memset(&SmParam,0,sizeof(SmParam));
		
		SmParam.TP_TYPE = pSrc->TP_TYPE | TP_UDHI;
		SmParam.TP_DCS = pSrc->TP_DCS;
		strncpy(SmParam.TPA,pSrc->TPA,sizeof(pSrc->TPA));
		memcpy(SmParam.TP_UD, msg+offset, unit < left ? unit : left);
	
		offset += unit;
		left -= unit;

		memset(pduString,0,SIZE_1024);
		pduLength = gsmEncodePdu(&SmParam, pduString,1);
		
	//	logs(LOG_INFO, "Generate PDU String is:%s\n",pduString);
		nc_log (&logger, NC_MOBILED_NAME, NA_LOG_ERROR, "@%04d Mobiled SMS thread:Generate PDU String is:%s!", __LINE__, pduString);
	//	logs(LOG_INFO, "Generate PDU String Length is:%d\n",pduLength);
		nc_log (&logger, NC_MOBILED_NAME, NA_LOG_ERROR, "@%04d Mobiled SMS thread:Generate PDU String Length is:%d!", __LINE__, pduLength);

		memset(recv_buf, 0, sizeof(recv_buf));
		ucRet = send_atcmd(sms_comport, "AT+CMGF=0\r", "OK", "ERROR", 0, 0, 5000, recv_buf);
		if(0x00 != ucRet)
		{
		//	logs(LOG_ERRO, "Send AT command \"AT+CMGF=0\" failure\n"); 
			nc_log (&logger, NC_MOBILED_NAME, NA_LOG_ERROR, "@%04d Mobiled SMS thread:Send AT command \"AT+CMGF=0\" failure!", __LINE__);
			return -3;
		}

        	memset(recv_buf, 0, sizeof(recv_buf));
	       ucRet = send_atcmd(sms_comport, "AT+CSCA?\r", "OK", "ERROR", 0, 0, 5000, recv_buf);
	       if(0x00 != ucRet)
	       {
	//	       logs(LOG_ERRO, "Send AT command \"AT+CSCA?\" failure\n"); 
		       nc_log (&logger, NC_MOBILED_NAME, NA_LOG_ERROR, "@%04d Mobiled SMS thread:Send AT command \"AT+CSCA?\" failure!", __LINE__);
		    return -3;
	       }

	       if(GSM_7BIT==pSrc->TP_DCS || GSM_8BIT==pSrc->TP_DCS)
	       {
               memset(recv_buf, 0, sizeof(recv_buf));
	           ucRet = send_atcmd(sms_comport, "AT+CSCS=\"IRA\"\r", "OK", "ERROR", 0, 0, 5000, recv_buf);
	       }
	       else if(GSM_UCS2 == pSrc->TP_DCS)
	       {
               memset(recv_buf, 0, sizeof(recv_buf));
	           ucRet = send_atcmd(sms_comport, "AT+CSCS=\"UCS2\"\r", "OK", "ERROR", 0, 0, 5000, recv_buf);
	       }

	       if(0x00 != ucRet)
	       {
//	            logs(LOG_ERRO, "Send AT command \"AT+CSCS=\"****\"\" failure\n"); 
		
		    nc_log (&logger, NC_MOBILED_NAME, NA_LOG_ERROR, "@%04d Mobiled SMS thread:Send AT command \"AT+CSCS=\"****\"\" failure!", __LINE__);
		    return -4;
	       }

	       memset(atcmd,0, SIZE_1024);
	       snprintf(atcmd, SIZE_1024, "AT+CMGS=%03d\r", pduLength);
//	       logs(LOG_INFO, "pduLength is:%d\n",pduLength);

               memset(recv_buf, 0, sizeof(recv_buf));
	       ucRet = send_atcmd(sms_comport, atcmd, ">", "ERROR", 0, 0, 5000, recv_buf);
	       if(0x00 != ucRet)
	       {
//	            logs(LOG_ERRO, "Send AT command \"AT+CMGS=***\" failure\n"); 
		
		    nc_log (&logger, NC_MOBILED_NAME, NA_LOG_ERROR, "@%04d Mobiled SMS thread:Send AT command \"AT+CMGS=***\" failure!", __LINE__);
		    return -5;
	       }

	       memset(atcmd,0, SIZE_1024);
	       snprintf(atcmd, SIZE_1024, "%s\x1a", pduString);

	       ucRet = send_atcmd(sms_comport, atcmd, "OK", "ERROR", 0, 500, 25000, recv_buf);
	       if(0x00 != ucRet)
	       {
	       //     logs(LOG_ERRO, "Send PDU String Failure, RetCode [0x%02x]\n", ucRet); 
		
		    nc_log (&logger, NC_MOBILED_NAME, NA_LOG_ERROR, "@%04d Mobiled SMS thread:Send PDU String Failure, RetCode [0x%02x]!", __LINE__, ucRet);
		    return -6;
	       }

		pk_num++;
	}

	return -2;
}

int gsmSendSMS(SM_PARAM* pSrc, COM_PORT *sms_comport)
{
	int pduLength = 0,count=0,max;   //count:user data's length
	char pduString[SIZE_1024];  /* The last PDU string send by AT command */
    	char atcmd[SIZE_1024];
	
	count = strlen((char *)(pSrc->TP_UD));
//	logs(LOG_INFO, "The user's data length is:%d\n",count);
	nc_log (&logger, NC_MOBILED_NAME, NA_LOG_ERROR, "@%04d Mobiled SMS thread:The user's data length is:%d!", __LINE__, count);
	if(GSM_7BIT==pSrc->TP_DCS)
	{
		max = 160;
	}
	else if(GSM_8BIT==pSrc->TP_DCS)
	{
		max = 140;
	}
	else 
	{
		max = 70;   // 70*3   UTF-8 code needs 24bits to display a Chinese character
	}

	if(max<count)
	{
	//	logs(LOG_INFO, "This is a long message,the length is:%d\n", count);
		nc_log (&logger, NC_MOBILED_NAME, NA_LOG_ERROR, "@%04d Mobiled SMS thread:This is a long message,the length is:%d!", __LINE__, count);
		return gsmSendLongSMS(pSrc, sms_comport);
	}

	memset(pduString,0,SIZE_1024);
	pduLength = gsmEncodePdu(pSrc, pduString,0);

//	logs(LOG_INFO, "Generate PDU String is:%s\n",pduString);
//	logs(LOG_INFO, "Generate PDU String Length is:%d\n",pduLength);
  	
  	memset(recv_buf, 0, sizeof(recv_buf));
	ucRet = send_atcmd(sms_comport, "AT+CSCA?\r", "OK", "ERROR", 0, 0, 5000, recv_buf);
	if(0x00 != ucRet)
       {
//	    logs(LOG_ERRO, "Send AT command \"AT+CSCA?\" failure\n"); 
	
	    nc_log (&logger, NC_MOBILED_NAME, NA_LOG_ERROR, "@%04d Mobiled SMS thread:Send AT command \"AT+CSCA?\" failure!", __LINE__);
	    return -3;
       }

       if(GSM_7BIT==pSrc->TP_DCS || GSM_8BIT==pSrc->TP_DCS)
       {
	   memset(recv_buf, 0, sizeof(recv_buf));
	   ucRet = send_atcmd(sms_comport, "AT+CSCS=\"IRA\"\r", "OK", "ERROR", 0, 0, 5000, recv_buf);
       }
       else if(GSM_UCS2 == pSrc->TP_DCS)
       {
       memset(recv_buf, 0, sizeof(recv_buf));
	   ucRet = send_atcmd(sms_comport, "AT+CSCS=\"UCS2\"\r", "OK", "ERROR", 0, 0, 5000, recv_buf);
       }

       if(0x00 != ucRet)
       {
	 //   logs(LOG_ERRO, "Send AT command \"AT+CSCS=\"****\"\" failure\n"); 
		
	    nc_log (&logger, NC_MOBILED_NAME, NA_LOG_ERROR, "@%04d Mobiled SMS thread:Send AT command \"AT+CSCS=\"****\"\" failure!", __LINE__);
	    return -4;
       }

	memset(recv_buf, 0, sizeof(recv_buf));
	ucRet = send_atcmd(sms_comport, "AT+CMGF=0\r", "OK", "ERROR", 0, 0, 5000, recv_buf);
	if(0x00 != ucRet)
	{
	//        logs(LOG_ERRO, "Send AT command \"AT+CMGF=0\" failure\n"); 
		nc_log (&logger, NC_MOBILED_NAME, NA_LOG_ERROR, "@%04d Mobiled SMS thread:Send AT command \"AT+CMGF=0\" failure!", __LINE__);
		return -1;
	}

       memset(atcmd,0, SIZE_1024);
       snprintf(atcmd, SIZE_1024, "AT+CMGS=%03d\r", pduLength);
 //      logs(LOG_INFO, "pduLength is:%d\n",pduLength);
       
       nc_log (&logger, NC_MOBILED_NAME, NA_LOG_ERROR, "@%04d Mobiled SMS thread:pduLength is:%d!", __LINE__, pduLength);
    
       memset(recv_buf, 0, sizeof(recv_buf));
       ucRet = send_atcmd(sms_comport, atcmd, ">", "ERROR", 0, 0, 5000, recv_buf);
       if(0x00 != ucRet)
       {
//	    logs(LOG_ERRO, "Send AT command \"AT+CMGS=***\" failure\n"); 
	
	    nc_log (&logger, NC_MOBILED_NAME, NA_LOG_ERROR, "@%04d Mobiled SMS thread:Send AT command \"AT+CMGS=***\" failure!", __LINE__);
	    return -5;
       }

       memset(atcmd,0, SIZE_1024);
       snprintf(atcmd, SIZE_1024, "%s\x1a", pduString);

       memset(recv_buf, 0, sizeof(recv_buf));
       ucRet = send_atcmd(sms_comport, atcmd, "OK", "ERROR", 0, 500, 20000, recv_buf);
       if(0x00 != ucRet)
       {
//	    logs(LOG_ERRO, "Send PDU String Failure, RetCode [0x%02x]\n", ucRet); 
		
	    nc_log (&logger, NC_MOBILED_NAME, NA_LOG_ERROR, "@%04d Mobiled SMS thread:Send PDU String Failure, RetCode [0x%02x]!", __LINE__, ucRet);
	    return -6;
       }
	
	return -0;
}

int gsmRecvSMS(SM_PARAM *pDst, int i, COM_PORT *sms_comport)
{
	char *ptr;
	int nMsg = 0;    //the number of unread SMS

//	char    recvTemp[SIZE_1024];
	char	atcmd[SIZE_128];

//	char    rcv_ud[512];
//	int     nLength = 0;
//	int 	data_len = 0;
#if 0
	ucRet = send_atcmd(&comport, "AT+CNMI=1,1,2\r", "OK", "ERROR", 0, 0, 5000);
	if(0x00 != ucRet)
	{
//		EPRINT("Send AT command \"AT+CNMI=1,1,2\" failure\n"); 
		return -1;
	}
#endif
	memset(atcmd,0,SIZE_128);
	snprintf(atcmd, SIZE_128, "AT+CMGR=%d\r", i);
    
	memset(recv_buf, 0, sizeof(recv_buf));
	ucRet = send_atcmd(sms_comport, "AT+CMGF=0\r", "OK", "ERROR", 0, 0, 5000, recv_buf);
	if(0x00 != ucRet)
	{
	//        logs(LOG_ERRO, "Send AT command \"AT+CMGF=0\" failure\n"); 
		nc_log (&logger, NC_MOBILED_NAME, NA_LOG_ERROR, "@%04d Mobiled SMS thread:Send AT command \"AT+CMGF=0\" failure!", __LINE__);
		return -1;
	}

        memset(recv_buf, 0, sizeof(recv_buf));
	ucRet = send_atcmd(sms_comport, atcmd, "OK", "ERROR", 0, 0, 10000, recv_buf);
	if(0x00 != ucRet)
	{
	//	logs(LOG_ERRO, "Send AT command \"AT+CMGR=%d\" failure [0x%02x]\n",i,ucRet); 
		nc_log (&logger, NC_MOBILED_NAME, NA_LOG_ERROR, "@%04d Mobiled SMS thread:Send AT command \"AT+CMGR=%d\" failure [0x%02x]!", __LINE__, i, ucRet);
		nMsg = -1;
		goto CleanUp;
	}
	
//	memset(rcv_ud,0,sizeof(rcv_ud));
#if 0
	memset(recvTemp,0,sizeof(recvTemp));
	strncpy(recvTemp, recv_buf, sizeof(recv_buf));
	ptr = recvTemp;
#endif

	ptr = recv_buf;

	while((ptr = strstr(ptr, "+CMGR:")) != NULL)
	{
		ptr+=7;        //jump "+CMGR: "

		ptr = strstr(ptr, "\r\n");  //look for /r /n
		if(ptr!=NULL)
		{
			ptr += 2;     //jump to the next line   /r/n
			
			gsmDecodePdu(ptr, pDst);   

			nMsg = 1;
		}
	}
#if 0
	while((ptr = strstr(ptr, "+CMGL:")) != NULL)
	{
		ptr+=7;        //jump "+CMGL: "
//		sscanf(ptr,"%u",&pDst->index);
//		IPRINT("Message index is:%d\n",pDst->index);

		ptr = strstr(ptr, "\r\n");  //look for /r /n
		if(ptr!=NULL)
		{
			ptr += 2;     //jump to the next line   /r/n
			
			len = gsmDecodePdu(ptr, pDst);   

			nLength = strlen(pDst->TP_UD);

			if (pk_rcv_num > 0)             //it is a long msg
			{
				IPRINT("This is a long message!\n");
				memcpy(rcv_ud+data_len, pDst->TP_UD, nLength);
				data_len += nLength;
				
				if ( pk_rcv_total == pk_rcv_num)
				{
					memset(pDst->TP_UD,0,sizeof(pDst->TP_UD));
					memcpy(pDst->TP_UD, rcv_ud, data_len);

					pk_rcv_total = pk_rcv_num = 0;
					memset(rcv_ud,0,sizeof(rcv_ud));
					data_len = 0;
					
				//	pDst++;       //the next message
					nMsg++;
				}

			}
			else
			{
				pk_rcv_total = pk_rcv_num = 0;
				memset(rcv_ud,0,sizeof(rcv_ud));
				
			//	pDst++;       //the next message
				nMsg++;
			}
		}
	}
#endif

CleanUp:
	return nMsg;
}
    
// Delete Message
// index: Mesage serial number
int gsmDeleteSMS(int index, COM_PORT *sms_comport)
{
    char atcmd[16];       
    
    sprintf(atcmd, "AT+CMGD=%d\r", index); 
    
    memset(recv_buf, 0, sizeof(recv_buf));
    ucRet = send_atcmd(sms_comport, atcmd, "OK", "ERROR", 0, 50, 20000, recv_buf);
   if(0x00 != ucRet)
   {
//	   logs(LOG_ERRO, "Send Delete Command Failure, RetCode [0x%02x]\n", ucRet); 
		
	   nc_log (&logger, NC_MOBILED_NAME, NA_LOG_ERROR, "@%04d Mobiled SMS thread:Send Delete Command Failure, RetCode [0x%02x]!", __LINE__, ucRet);
	   return 1;
   } 
   return 0;
}



