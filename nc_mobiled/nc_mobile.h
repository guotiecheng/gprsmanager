#ifndef NC_MOBILE_H
#define NC_MOBILE_H

#ifdef __cplusplus
extern "C" {
#endif

#define NC_MOBILED_MAX_CONF    256
#define NC_MOBILE_MAX_CLIENT_BUF            10240
#define NC_MOBILE_SMS_MAX_CLIENT_BUF        2048
#define NC_MOBILE_TIMEOUT                   10000

#define NC_MOBILE_SERVER_ADDR               "127.0.0.1"
#define NC_MOBILE_SERVER_PORT               8000
#define NC_MOBILE_MAX_CLIENT               64

#define NC_MOBILE_SMS_SERVER_ADDR               "127.0.0.1"
#define NC_MOBILE_SMS_SERVER_PORT               8005

/* State information */
//HardWare information
//#define STATE_GPRS_HWINFO_EXIST_YES
//#define STATE_GPRS_HWINFO_EXIST_NO

//#define STATE_GPRS_HWINFO_GOOD_YES
//#define STATE_GPRS_HWINFO_GOOD_NO

// Error  code 
//hardware error
#define ERR_GPRS_HW_NO_MODULE         	0x0001
#define ERR_GPRS_HW_MODULE_BREAK		0x0002
#define ERR_GPRS_HW_NO_SIGNAL		0x0003
#define ERR_GPRS_HW_SIM_DOOR_OPEN	0x0004  //means sim_door_1 sim_door_2 are all open
#define ERR_GPRS_HW_SIM_NOT_READY 	0x0005
#define ERR_GPRS_HW_SIM_REGISTER_FAILURE	0x0006

//software error
#define ERR_GPRS_SW_PPP_FAILURE	0x0011


//#define STATE_GPRS_HWINFO_POWER_ON
#define STATE_GPRS_HWINFO_POWER_OFF  	0x0101


//SoftWare information
//#define STATE_GPRS_SWINFO_SIM_CURRENT_READY_YES  
//#define STATE_GPRS_SWINFO_SIM_CURRENT_READY_NO

#define STATE_GPRS_SWINFO_REGISTER_HOMEWORK  	0x0111
#define STATE_GPRS_SWINFO_REGISTER_ROAMING 		0x0112
//#define STATE_GPRS_SWINFO_REGISTER_NO

//info
#define STATE_GPRS_HWINFO_TYPE_UC864E			0x0121
#define STATE_GPRS_HWINFO_TYPE_GTM900B			0x0122
#define STATE_GPRS_HWINFO_TYPE_GTM900C			0x0123
#define STATE_GPRS_HWINFO_TYPE_G600				0x0124
#define STATE_GPRS_HWINFO_TYPE_GOSPELL			0x0125



/* CMD use */
#define CMD_GPRS_PPP_START   	0x0001
#define CMD_GPRS_PPP_STOP		0x0002
#define CMD_GPRS_SMS_SEND		0x0011
#define CMD_GPRS_SMS_QUERY	0x0012
#define CMD_GPRS_MODULE_POWER_DOWN   		0x0021	// power off GPRS module
#define CMD_GPRS_MODULE_POWER_UP    			0x0022	//power on GPRS module 

/* CMD get gprs  hardWare infomation */
#define CMD_GPRS_GET_HWINFO_EXIST			0x0101   //Gprs module is exist ? or no 
#define CMD_GPRS_GET_HWINFO_GOOD   			0x0102	//Gprs module is good or  bad 
#define CMD_GPRS_GET_HWINFO_MODULE_TYPE  	0x0103	// GTM900B  or  UC864E or G600
#define CMD_GPRS_GET_HWINFO_POWER_STATE   	0x0104	// poeroff  or  poweron 
#define CMD_GPRS_GET_HWINFO_SIMDOOR_1 	    	0x0105	// simdoor is close or  open 
#define CMD_GPRS_GET_HWINFO_SIMDOOR_2   		0x0106	// same as above


/* CMD get gprs sofrware information */
#define CMD_GPRS_GET_SWINFO_SIM_CURRENT_READY	0x0111    //ready or no sim  AT+CPIN?
#define CMD_GPRS_GET_SWINFO_SIGNAL_VALUE     			0x0112	// current signal value 
#define CMD_GPRS_GET_SWINFO_REGISTER      			0x0113	//  regist  , at+creg 1:regist local   5  roaming 


/* CMD get gprs work status information */
#define CMD_GPRS_GET_WORKSTATUS_NET_TYPE  			0x0121		//  2G   3G
#define CMD_GPRS_GET_WORKSTATUS_GPRS_WORK_STATUS	0x0122  	//SYS_IDLE  PPP_BUSY  PPP_IDLE SMS_BUSY  VOICE_BUSY  
#define CMD_GPRS_GET_WORKSTATUS_POWON_UPTIME   	0x0123	//  The time since GPRS  power up 
#define CMD_GPRS_GET_WORKSTATUS_NET_FLOW    			0x0124	// total packets of received  and  total  packets of send since GPRS power up


 /*CMD set gprs */
#define CMD_GPRS_SET_SIM_1 	0x0201
#define CMD_GPRS_SET_SIM_2 	0x0202
#define CMD_GPRS_SET_APN   		0x0203	// set apn for sim

#define CMD_GPRS_SET_NET_TYPE_2G  0x0211
#define CMD_GPRS_SET_NET_TYPE_3G 	0x0212

#define CMD_GPRS_SET_PPP_DHCP_START	0x0221  //set GPRS DHCP service start
#define CMD_GPRS_SET_PPP_DHCP_CLOSE	0x0222
#define CMD_GPRS_SET_PPP_IP   			0x0223	// close DHCP service , and set a fixed IP

#define CMD_GPRS_SET_SMS_SEND_REPORT   	0x0231		//after SMS has been send , report 
#define CMD_GPRS_SET_SMS_SEND_AMOUNT_LIMITED  	0x0232	//how many SMS to be send  can be stored 
#define CMD_GPRS_SET_SMS_SEND_STORE_TIMEOUT 	0x0233
#define CMD_GPRS_SET_SMS_RECEIVE_AMOUNT_LIMITED 0x0234
#define CMD_GPRS_SET_SMS_RECEIVE_STORE_TIMEOUT	0x0235




//VOICE SET CMD 
//#defne CMD_GPRS_SET_VOICE_

#define NC_MOBILE_OPS_REQ  1 
#define NC_MOBILE_OPS_RES  2 
#define NC_MOBILE_MSG_LEN 512
#define NC_MOBILE_CONTENT_LEN 1024


typedef struct NC_MOBILE_SMS_T
{

  char	acTelNum[64];

  char 	acMsg[NC_MOBILE_MSG_LEN];

}NC_MOBILE_CONTENT_SMS;

typedef struct NC_MOBILE_PPP_T
{
	
}NC_MOBILE_CONTENT_PPP;

typedef struct NC_MOBILE_VOICE_T
{
  int    	iSrcPort;

  char    	acTelNum[64];
}NC_MOBILE_CONTENT_VOICE;

typedef struct NC_MOBILE_FAX_T
{

}NC_MOBILE_CONTENT_FAX;

typedef struct NC_MOBILE_USSD_T
{

}NC_MOBILE_CONTENT_USSD;

typedef struct NC_MOBILE_DATA_T
{
//  unsigned short     aliagment;  /*Padding the structure to 4 bytes alignment, not used*/
  unsigned short	iCmd; 

  unsigned char  ucContent[NC_MOBILE_CONTENT_LEN];     

}NC_MOBILE_DATA;

typedef struct NC_MOBILE_RESULT_T
{
// unsigned short     aliagment;  /*Padding the structure to 4 bytes alignment, not used*/
  unsigned short		iResult; 

  unsigned char  ucContent[NC_MOBILE_CONTENT_LEN];     

}NC_MOBILE_RESULT;

typedef struct NC_MOBILE_EVENT_T
{
 
  unsigned int      clen;  //only used when receive, it equal content length 
 
  unsigned char     *data;  //only used when receive ,it points content 
	
  NC_FRAME          frame;
  
}NC_MOBILE_EVENT;


typedef struct NC_MOBILE_T	
{
  int               magic,  //   head  flag
                    init;

  unsigned long     lact;
  NA_TIME_RETRY     retry;
  NA_SOCK           sock;
  NC_MOBILE_EVENT   event;

  unsigned char    *buf;      //send ; receive
  unsigned int      len;     // frame_head_len + content_len
  
}NC_MOBILE;


// ================
// ** PUBLIC USE **
// ================

int nc_mobile_init(NC_MOBILE * ctx);
void nc_mobile_term (NC_MOBILE *ctx);
int nc_mobile (NC_MOBILE *ctx , NC_MOBILE_DATA *dat, int flag);

// ================
// ** PRIVATE USE **
// ================
int nc_mobile_data_build(NC_MOBILE *ctx , NC_MOBILE_DATA *dat );
int nc_mobile_frame_build (NC_MOBILE_EVENT *event);
int nc_mobile_frame_parse(NC_MOBILE_EVENT * event, unsigned char * rbuf, unsigned int rlen, unsigned int * used);


#ifdef __cplusplus
}
#endif


#endif

