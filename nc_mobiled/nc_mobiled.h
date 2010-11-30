#ifndef NC_MOBILED_H
#define NC_MOBILED_H

#ifdef __cplusplus
extern "C" {
#endif

//create thread errorNo
#define MODULE_INIT_FAILURE 0x10
#define MODULE_EXEC_FAILURE 0x11
#define MODULE_DATA_FAILURE 0x12
#define MODULE_SMS_FAILURE 0x13
#define MODULE_TEL_FAILURE 0x14
#define MODULE_USSD_FAILURE 0x15 

#define NC_MOBILED_MODEM_GTM900B_NAME           "gtm900b"
#define NC_MOBILED_MODEM_GTM900C_NAME           "gtm900c"
#define NC_MOBILED_MODEM_G600_NAME              "g600"
#define NC_MOBILED_MODEM_UC864E_NAME            "uc864e"

#define NC_MOBILED_MODEM_GTM900B                1
#define NC_MOBILED_MODEM_GTM900C                2
#define NC_MOBILED_MODEM_G600                   3
#define NC_MOBILED_MODEM_UC864E                 4

#define NC_MOBILED_SIM1                         1
#define NC_MOBILED_SIM2                         2

#define NC_MOBILED_DATA_PLAN_UNLIMITTED_NAME    "unlimitted"
#define NC_MOBILED_DATA_PLAN_LIMITTED_NAME      "limitted"
#define NC_MOBILED_DATA_PLAN_UNLIMITTED         1
#define NC_MOBILED_DATA_PLAN_LIMITTED           2

#define NC_MOBILED_CONF                         "mobiled"
#define  NC_MOBILED_CONF_MODEM                  "module"
#define  NC_MOBILED_CONF_IDLETIME		"idelTime"
#define  NC_MOBILED_CONF_PINGTIME		"pingTime"
#define  NC_MOBILED_CONF_SIM_NETWORKTYPE	"netWorkType"
#define  NC_MOBILED_CONF_PRIMARY_SIM_CARRIER  	"priSimCarrier"
#define  NC_MOBILED_CONF_SECONDARY_SIM_CARRER 	"secSimCarrier"
#define  NC_MOBILED_CONF_SIM			"sim"
#define  NC_MOBILED_CONF_APNS			"apns"

//#define  NC_MOBILED_CONF_SIM_NETWORKTYPE	"netWorkType"
#define  NC_MOBILED_CONF_SIM_USER		"user"
#define  NC_MOBILED_CONF_SIM_PASSWORD  	"passWord"
#define  NC_MOBILED_CONF_SIM_PING_IP  "pingIp"
#define  NC_MOBILED_CONF_SIM_APN	"apn"

#define  NC_MOBILED_CONF_APNS_CARRIER	"carrier"
#define  NC_MOBILED_CONF_APNS_APN		"apn"
#define  NC_MOBILED_CONF_APNS_USER		"user"
#define  NC_MOBILED_CONF_APNS_PASSWORD	"passWord"

enum ModeType
{
	IDLE = 0x01,
	SMS_GW = 0X02,
	SMS_CTRL = 0x03,
	PPP_START = 0x04,
	PPP_STOP = 0x05,
	VOICE = 0x06,
	FAX = 0x07,
};

enum GprsState
{
	SYS_IDLE = 0x01, /*No application holds the GPRS module*/
	PPP_BUSY = 0x02,
	PPP_IDLE = 0x03,
	VOICE_BUSY = 0X04, /*VOICE pthread is holding the GPRS module*/
	SMS_BUSY = 0x05,  
};


typedef struct mutex_Gprs_Status{
	unsigned char ucGprsStatus;	   /*Now which pthread hold the GPRS module*/	
	pthread_mutex_t f_lock;
} MutexGprsStatus;

extern unsigned char g_ucWorkMode;  /*This global variable shows which thread can use GPRS module*/
extern MutexGprsStatus  g_stGprsStatus; /*Every thread holds or frees GPRS module,then modify this variable*/

#if 0
enum 	NETWORK_TYPE
{
   MODE_UNKNOW = 0x00,
   MODE_GPRS,
   MODE_EDGE,
   MODE_3G,
   MODE_HSDPA = MODE_3G
};
#endif 

typedef struct NC_MOBILED_MOD_DATA_T
{
  int           event;
  
  NC_MOBILE_CONTENT_PPP	  *pstPppContent;
}
NC_MOBILED_MOD_PPP;

typedef struct NC_MOBILED_MOD_VOICE_T
{
  int           event;

  //         int   src_port;
 // char          tel_no [64];
  NC_MOBILE_CONTENT_VOICE	*pstTelContent;
}
NC_MOBILED_MOD_VOICE;

typedef struct NC_MOBILED_MOD_SMS_T
{
  int           event;

  NC_MOBILE_CONTENT_SMS	*pstSmsContent;
}
NC_MOBILED_MOD_SMS;

typedef struct NC_MOBILED_MOD_USSD_T
{
  int           event;

 // char          *msg;
  NC_MOBILE_CONTENT_USSD	*pstUssdContent;
 
}
NC_MOBILED_MOD_USSD;

typedef struct NC_MOBILED_AT_T
{
  char          req [64],
                res [128],
                err [32],
                ret [64];

  unsigned long start,
                delay,
                timeout;
}
NC_MOBILED_AT;

typedef struct NC_MOBILED_INFO_T
{
  int           roaming,
                signal;

  char          *provider,
                *lac;
}
NC_MOBILED_INFO;



typedef struct NC_MOBILED_PROC_T
{
  int           init,
                modem,

                data_plan;

  char          apn [128],
                uid [128],
                pwd [128];

  NC_MOBILED_INFO info;
}
NC_MOBILED_PROC;


typedef struct NC_MOBILED_CLIENT_T
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
NC_MOBILED_CLIENT;

typedef struct NC_MOBILED_SERVER_T
{
  NA_SOCK           sock;     //server socket 
 
  NA_SOCK_FDS       fds;     // server socket fd 

  NC_MOBILED_CLIENT *client;    //  client point 



  NA_ARRAY          clients;      //clients array 
 
}
NC_MOBILED_SERVER;


// ======================
// ** FUNCTION POINTER **
// ======================

typedef int   (*NC_MOBILED_MOD_F_INIT)  (void *_ctx, void *hook);
typedef void  (*NC_MOBILED_MOD_F_TERM)  (void *_ctx);
typedef int   (*NC_MOBILED_MOD_F_EXEC)  (void *_ctx);

typedef int   (*NC_MOBILED_MOD_F_DATA)  (void *_ctx, NC_MOBILED_MOD_PPP *event);
typedef int   (*NC_MOBILED_MOD_F_USSD)  (void *_ctx, NC_MOBILED_MOD_USSD *event);
typedef int   (*NC_MOBILED_MOD_F_TEL)   (void *_ctx, NC_MOBILED_MOD_VOICE *event);
typedef int   (*NC_MOBILED_MOD_F_SMS)   (void *_ctx, NC_MOBILED_MOD_SMS *event);

// ====================
// ** MAIN STRUCTURE **
// ====================

typedef struct NC_MOBILED_T
{
  int               magic,
                    init,
                    good;

  void              *module;

  NC_MOBILED_SERVER server;

  NC_GPRS_CONFIG config;

 GPRS_ATTR  gprs;
  
}
NC_MOBILED;

// =================
// ** PRIVATE USE **
// =================

int   nc_mobiled_main (int argc, char *argv []);

int   nc_mobiled_init (NC_MOBILED *ctx);
void  nc_mobiled_term (NC_MOBILED *ctx);

int   nc_mobiled_good (NC_MOBILED *ctx);
int   nc_mobiled_proc (NC_MOBILED *ctx);
int   nc_mobiled_serve(NC_MOBILED * ctx, NC_MOBILED_CLIENT * client);

int   nc_mobiled_config (NC_MOBILED *ctx);

int   nc_mobiled_resp(NC_MOBILED * ctx, NC_MOBILED_CLIENT * client);

void nc_mobiled_set_state(unsigned char holder);

#ifdef __cplusplus
}
#endif

#endif


