
#ifndef GPRS_H
#define GPRS_H


#define DEVNAME_LEN  64
#define APN_LEN      64
#define CARRIER_LEN  64
#define USER_LEN 32
#define PASSWD_LEN 32
#define IP_LEN 64

#define SIZE_1024	1024

#define POWER_ON     1
#define POWER_OFF    2
#define NO_CONFIG_PRIMARY_SIM	0x02

#define SIM_1  1
#define SIM_2  2
#define SIM_DOOR_1  1
#define SIM_DOOR_2  2

#define PPP_ALIVE  1
#define PPP_DEAD   0

#define  REG_UNREGIST  0x00
#define  REG_HOMEWORK  0x01
#define  REG_ROAMING   0x05

#define ONEDAY     	( 24*60*60 )
#define ONEHOUR    	(  60*60 )
#define ONEMIN     	(  60 )
#define ONESECOND 	( 60 )

#define IDLETIME_DEFAULT  ( 10 * 60 * 1000 )
#define PINGTIME_DEFAULT (  60 * 1000 )

enum	MODULE_TYPE
{
   MODULE_GTM900B = 0x01,
   MODULE_G600,
   MODULE_UC864E,
   MODULE_LC6311
};

enum 	NETWORK_TYPE
{
   MODE_UNKNOW = 0x00,
   MODE_GPRS,
   MODE_EDGE,
   MODE_3G,
   MODE_HSDPA = MODE_3G
};

enum	APN_FLAG
{
   APN_NULL = 0x00,
   APN_HISTORY ,
   APN_GPRS_CONF,
   APN_APNS_CONF
};


typedef struct __CTRL_PORT
{
   char              dev_name[DEVNAME_LEN];
   int               fd_com;  
}CTRL_PORT;



typedef struct __SIM
{   


   short  int  	        signal;			//  signal Value   0 ~ 99
   short  int             regist;			//  
   unsigned 	long   int 		 fallBackTime; /* when arrive this time , if current sim is  SIM2, we try to switch to SIM1  */
   
   char      	    carrier[CARRIER_LEN];  // AT + COPS ? 
   char		    apnApnsConf[APN_LEN];  // apn from apns_conf.xml
   char		    apnGprsConf[APN_LEN];  // apn from gprs_conf.xml
   char		    apnHistory[APN_LEN];  // apn used  last 
   char 		    apnCur[APN_LEN];  // apn  used  currently  
   char 		    apnFlag; // apn  is null ?  or from gprs_conf.xml or from apns_conf.xml
   char		    historyApnFlag; // historyApn from GPRS

   char               userGprsConf[USER_LEN];    /* PPP dial up username  from  gprsConf.xml */
   char		   userApnsConf[USER_LEN];  /* PPP dial up username from apnsConf.xml */
   char		   userHistory[USER_LEN];
   
   char               passwordGprsConf[PASSWD_LEN];    /* PPP dial up password from gprsConf.xml */
   char		   passwordApnsConf[PASSWD_LEN];   /* PPP dial up password from  apnsConf.xml */
   char	  	   passwordHistory[PASSWD_LEN];
   
   char    		   pingIp[IP_LEN] ; /*  GPRS  check_ppp_status  ,we need  to  ping  this  IP   */

		
}SIM;


typedef  struct __SIM_DOOR
{
   short  int            	simDoorState; // 1, sim_door1  closed ;  0, sim_door1 opened 
}SIM_DOOR;


typedef struct _GPRS_TIME_TYPE
{
 int year;
 int month;
 int day;
 int hour;
 int minute;
 int second;
}GPRS_TIME_TYPE;

typedef struct _GPRS_TIME
{
   GPRS_TIME_TYPE    startTime;
   GPRS_TIME_TYPE    timeLen;
   long			    startTimeForCal;   // get poweron time , seconds
}GPRS_TIME;

typedef struct MOD_PPP_STAT_T
{
		unsigned long long ullRx_Packets;	// total packets received
		unsigned long long ullTx_Packets;	// total packets transmitted
		unsigned long long ullRx_Bytes;		// total bytes received
		unsigned long long ullTx_Bytes;		// total bytes transmitted

		unsigned long ulRx_Errors;				// bad packets received
		unsigned long ulTx_Errors;				// packet transmit problems
		unsigned long ulRx_Dropped;				// no space in linux buffers
		unsigned long ulTx_Dropped;				// no space available in linux
		unsigned long ulRx_Multicast;			// multicast packets received
		unsigned long ulRx_Compressed;
		unsigned long ulTx_Compressed;
		unsigned long ulCollisions;

		// detailed rx_errors:
		unsigned long ulRx_Length_Errors;
		unsigned long ulRx_Over_Errors;			// receiver ring buff overflow
		unsigned long ulRx_CRC_Errors;			// recved pkt with crc error
		unsigned long ulRx_Frame_Errors;		// recv'd frame alignment error
		unsigned long ulRx_FIFO_Errors;			// recv'r fifo overrun
		unsigned long ulRx_Missed_Errors;		// receiver missed packet

		// detailed tx_errors
		unsigned long ulTx_Aborted_Errors;
		unsigned long ulTx_Carrier_Errors;
		unsigned long ulTx_FIFO_Errors;
		unsigned long ulTx_Heartbeat_Errors;
		unsigned long ulTx_Window_Errors;
}MOD_PPP_STAT;

typedef struct _GPRS_INFO
{
   	unsigned  char 	module;    /* GPRS Module model: MODULE_GTM900B, MODULE_UC864E ...*/ 
   	unsigned  char   	type;      /* GPRS Module type, MODE_GPRS or MODE_3G mode*/
   	unsigned  char    	power;     /* GPRS Power ON/OFF */ 
	int				signalStrength; /*GPRS signale sthength*/
   	int				pppAlive; 
		
  	MOD_PPP_STAT	netFlow;
	GPRS_TIME		timer;
	
}GPRS_INFO;


typedef struct NC_GPRS_SIM_T
{
   unsigned 	long   int 		 fallBackTime; /* when arrive this time , if current sim is  SIM2, we try to switch to SIM1  */
   char		    apnGprsConf[APN_LEN];  // apn from gprs_conf.xml
   char               userGprsConf[USER_LEN];    /* PPP dial up username  from  gprsConf.xml */
   char               passwordGprsConf[PASSWD_LEN];    /* PPP dial up password from gprsConf.xml */
   char    		   pingIp[IP_LEN] ; /*  GPRS  check_ppp_status  ,we need  to  ping  this  IP   */
}
NC_GPRS_SIM;

typedef struct NC_GPRS_APN_T
{
   char		    carrier[CARRIER_LEN];
   char		    apnApnConf[APN_LEN];  // apn from gprs_conf.xml
   char               userApnConf[USER_LEN];    /* PPP dial up username  from  gprsConf.xml */
   char               passwordApnConf[PASSWD_LEN];    /* PPP dial up password from gprsConf.xml */
}
NC_GPRS_APN;

typedef struct NC_GPRS_CONFIG_T
{

  int           init;

  NC_GPRS_SIM  sim;

  NC_GPRS_APN  apn;

  NC_CONFIG     proxy;

  NA_ARRAY    array_sim;
  NA_ARRAY	array_apns;
  
}
NC_GPRS_CONFIG;

typedef struct __GPRS
{

   unsigned 	long   int 	   	pingTime;  /* if arrive this time , we ping  pingIp which is set by user  in gprs_conf.xml to check if ppp is alive */
   unsigned     long   int 		 idleTime; /* when arrive this time which is set bu user in gprs_conf.xml, we check if  PPP  is  idle  */
   unsigned	long	  long 	 sendBytesThreshold ; /* It is set in .conf file .*/
   unsigned 	long	  long	 receiveBytesThreshold ; /* It is set in .conf file .*/
   char   		netWorkType;  /*User defined  in gprs_conf.xml  . GPRS Module work mode: MODE_GPRS,MODE_EDGE,MODE_HSDPA */
  unsigned  char 	module;    /* GPRS Module model: MODULE_GTM900B, MODULE_UC864E ...*/ 
   unsigned  char   type;      /* GPRS Module type, MODE_GPRS or MODE_3G mode*/
  
  
   char      	primarySimCarrier[CARRIER_LEN];  	
   char		secondarySimCarrier[CARRIER_LEN];
   
   CTRL_PORT        ctrl_port;  
   COM_PORT         data_port;	//  UC864E  hate  tow  date  ports  .     1  /devttyUC864E0   2  /dev/ttyS2 

   int				curSimNo ;   //cur  used  Sim  :SIM_1  or  SIM_2
   SIM              	sim1, sim2; 	//two sim card 
   SIM_DOOR	       simDoor1 , simDoor2; // 
   int				curSimDoorNo;  //  cur  used SimDoor  :  SIM_DOOR_1  or  SIM_DOOR_2
   int				sim_1_Failure; // 1  sim_1 is failure;   0  sim_1  is  normal 

   GPRS_INFO	 	gprsInfo;
   NC_GPRS_CONFIG  config; 
 
} GPRS_ATTR;

//	AT CMD funtions
unsigned char at_match(char * p_pcStr, char * p_pcMatch); // string  p_pcMatch is in p_pcStr ?
int 	send_atcmd(COM_PORT *st_comport, char *atCmd, char *expect, char *error, unsigned long delay, unsigned long gap, unsigned long timeout, char *content);

// 	gprs time functions 
void get_current_time( GPRS_TIME_TYPE *timer);
void get_continue_time( GPRS_TIME_TYPE *startTime , GPRS_TIME_TYPE *lastTime);

// 	gprs  check gprs_status functions 
int 	check_gprs_status( GPRS_ATTR *gprs );  // 	singnal;  sim  AT+CPIN?;  register; 
int	check_sim_insert(GPRS_ATTR * gprs); 	//	AT+CPIN?
int	check_gprs_register(GPRS_ATTR * gprs);	//	AT+CREG?
int	check_gprs_signal(GPRS_ATTR * gprs); //	AT+CSQ?

// 	gprs power  manage functions 
int	gprs_power_down(GPRS_ATTR * gprs);	//	ioctl  :  cmd -- 2
int	gprs_power_up(GPRS_ATTR * gprs );	//	ioctl:	  cmd -- 1
int	check_gprs_power(GPRS_ATTR * gprs);	//	send  AT   cmd
int	gprs_reset(  GPRS_ATTR *gprs ) ; 	// reset
int 	get_gprs_poweron_uptime( GPRS_ATTR *gprs );
void clear_gprs_powerup_time(GPRS_ATTR *gprs);
void get_gprs_powerup_time(GPRS_ATTR * gprs);
long  get_sys_uptime( void );
void get_current_time(GPRS_TIME_TYPE * timer );


//	gprs port manage  functions 	; dataport open  and close   in comport.c 
int	ctrlport_open(CTRL_PORT * ctrlport);
int	ctrlport_close(CTRL_PORT * ctrlport);

//	gprs sim manage functions 
int	select_sim( GPRS_ATTR  *gprs);  	//  sim_door close  and  check_gprs_status return OK   
int   	gprs_set_sim_slot(GPRS_ATTR * gprs, int sim_number);  //  set sim_door
int	gprs_get_sim_slot(GPRS_ATTR * gprs, int sim_number); // get current  sim_door  setted
int	check_gprs_sim_door(GPRS_ATTR * gprs);	// judge  sim_door is close  or open
int	check_primary_sim( char *carrier, GPRS_ATTR *gprs );
int  	switch_sim(int currentSim, GPRS_ATTR * gprs);

//	gprs init  and close  functions
int 	initial_gprs(GPRS_ATTR *gprs);   // open data_port and ctrl_port ; power_up 
void  init_gprs_setting(GPRS_ATTR *gprs); // which port ?
int  	close_gprs(GPRS_ATTR  *gprs);	 //	power_down gprs ;  close  ports -- ctrl_port  and  data_port 

//	gprs apn manage functions 
int 	select_apn( GPRS_ATTR  *gprs);	//  history_apn -> gprs_conf_apn -> apns_conf_apn >check_PPP_status  ,if no OK, reselect apn
int	gprs_set_apn(GPRS_ATTR  *gprs , char *apn ); // at+cgdcont=1,"ip","cmnet" ( "3gnet"  etc ...)
int	gprs_get_carrier(int simNum , GPRS_ATTR * gprs); // at+cops?   for  query respond  apn from  apns_conf.xml 


#endif

