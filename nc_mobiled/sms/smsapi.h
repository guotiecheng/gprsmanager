#ifndef __smsapid_h__             
#define __smsapid_h__

#include          <unistd.h>
#include          <stdio.h>
#include          <string.h>
#include          <stdlib.h>
#include          <errno.h>
#include          <ctype.h>
#include 	  <sys/select.h>

#include          <time.h>
#include          <fcntl.h>
#include          <getopt.h>
#include          <signal.h>

#include          <sys/types.h>
#include          <sys/wait.h>
#include          <sys/stat.h>
#include          <sys/socket.h>
#include          <sys/time.h>
#include          <sys/un.h>
#include          <netinet/in.h>
#include          <sys/ioctl.h>
#include          <pthread.h>

//#include          "common/common.h"
//#include          "gprs/gprs.h"
//#include          "socket.h"
//#include 	  <locale.h>       //setlocale function

//#include	   "libs/iconv.h"
#include	   "../../libs/libna.h"
#include           "../nc_mobile.h"
#include           "../nc_mobiled.h"

extern int          domainSockfd;

/*  Unused till now
#define SMSAPI_CONFIG_DIR             "/etc"
#define SMSAPI_READER_CONFIG          SMSAPI_CONFIG_DIR "/reader.conf"
*/

#define PID_ASCII_SIZE                11
#define SHARDFILE_PAGES               2

#define SMS_THREAD_FUNCTION(f)      void *(*f)(void *)

#define      INTNAL     1   // International Phone Address
#define      NATNAL     0   // National  Phone Address

#define      SCA_ADDR   0   // SCA Address
#define      TPA_ADDR   1   // Destination or Reply Address

#define      GSM_7BIT   0
#define      GSM_8BIT   4
#define      GSM_UCS2   8

#define	     CanRead    1
#define	     CanWrite   2
#define	     TimeIsOut  0
#define	     Error     -1

#define      TotalMsgNum  50
#define      TimeOut     120      /*120s*/

//#define      UseGPRSTime  20     /*When API send a message,it still owns GPRS for 20s,*/

#define      TP_UDHI    1<<6
#define      TP_SRR     1<<5
#define      TP_VPF     2<<3
#define      TP_MTI     1

#define      GWHANDLED           1
#define      APINOT              1<<1
#define      OTHERNOTCONN        1<<2
#define      GPRSBUSY            1<<3
#define      ATFAILURE           1<<4
#define      INITGPRSERR         1<<5
#define      UNKNOWNFAILURE      1<<6
#define      GWLISTFULL          1<<7
#define      NONEWMSG            1<<8
#define      GWMEMERR            1<<9
#define      MSGTIIMEOUT         1<<10

#define	     APIHANDLED 	 1<<16
#define	     GATEWAYNOT	         1<<17
#define	     HOSTNOT	         1<<18
#define	     OTHERFAILURE	 1<<19
#define	     APILISTFULL	 1<<20

#if 0
enum sms_msg_status
{
	SMS_OTHERCLIENT_NOT = 0x02,    //gateway know the netrouter is not connect to api or netrouter know gateway is not conect to api
	SMS_GPRS_BUSY=0x03,            //api can't ask for gprs,because gprs is busy
	SMS_AT_FAILURE=0x04,	       //api send at command failure
	SMS_OTHER_FAILURE=0x05,        //other failure
	SMS_NO_NEWMSG=0x06,            //after check,now is no new message
	SMS_HAVE_NEWMSG=0x07,          //after check,now have new message
	SMS_GT_NEWMSG=0x0a,            //after api send  msg,then ask gateway whether have more new message
	SMS_SEND_HAVENEW=0x0b,         //receive the ask,gateway tell api have more new msg to send
	SMS_SEND_NONEW=0x0c,           //receive the ask,gateway tell api have no more new msg to send
	SMS_GPRSTO_NET=0x0e,           //when api process somethings,but netrouter ask for gprs at once,so api close gprs andstop do this,and tell gateway the gprs is not here now

	SMS_GPRS_BUSYNOW=0x30,         //netrouter ask api that gprs is busy now
	SMS_GPRS_ON=0x40,              //netrouter tell api that ask for gprs successfully or api tell netrouter gprs can use
};
#endif

enum sms_type
{
	ACK_NAK = 1,
	
	MOBILED_SMS_SEND = 2,
	MOBILED_SMS_QURRY = 3,
	MOBILED_SMS_RECV = 4,           //Mobiled ctrl thread  tell sms thread have new msg,SMS thread recv the msg and store it.
};

// Message Parameter: Used by Encode(Send) and Decode(Receive).
typedef struct
{
	char SCA[16];       // Service Ceter Address.
	char TPA[64];       // Destination or Reply Address(TP-DA or TP-RA)
	char TP_PID;        // Message Protocol Identify (TP-PID)
	char TP_DCS;        // Message Coding Scheme (TP-DCS)
	char TP_SCTS[16];   // Service Time Stample String(TP_SCTS), used by Receive.
	unsigned char TP_UD[1024];    // Original User Information(Encode or Decode TP-UD)
	char TP_TYPE;
}SM_PARAM;
typedef struct
{
	char cPhoneNum[64];
	unsigned int uiStatus;
	int index;
//	int indexGW;
//	int iHostID;
	time_t  ComeInTime;
}NAK_PARAM;

typedef struct
{
//	 char cType;
	 NAK_PARAM ack_nak;
	 SM_PARAM msgstruct;
}GW_API_PARAM;

// ====================
// ** MAIN STRUCTURE **
// ====================

typedef struct NC_MOBILED_SMS_CLIENT_T
{
	  unsigned char     *rbuf,
                        *sbuf;

	  unsigned int      rlen,
		            slen;

	  unsigned long     lact;

	  NA_SOCK           sock;

	  NC_MOBILE_EVENT   revent,
	                    sevent;   // Frame 
}
NC_MOBILED_SMS_CLIENT;

typedef struct NC_MOBILED_SMS_SERVER_T
{
	  NA_SOCK           sock;     //server socket 

	  NA_SOCK_FDS       fds;     // server socket fd 

	  NC_MOBILED_SMS_CLIENT *client;    //  client point 

	  NA_ARRAY          clients;      //clients array 
}
NC_MOBILED_SMS_SERVER;

typedef struct NC_MOBILED_SMS_T
{
  int               magic,
                    init,
                    good;

  NC_MOBILED_SMS_SERVER server;
}NC_MOBILED_SMS;

void * SMS_API_Thread(void *arg);

#endif
