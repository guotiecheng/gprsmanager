/*
 * =====================================================================================
 *
 *       Filename:  gprs.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/22/2010 10:15:11 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Guo TieCheng, guotiecheng@ghlsystems.com
 *        Company:  GHL System Berhad.
 *
 * =====================================================================================
 */
#include <common/common.h>

/********************************************************************************************
**************************time functions ********************************************************
**********************************************************************************************/
void get_current_time(GPRS_TIME_TYPE * timer )
{
	time_t sttime;
	
	struct tm *ptime;
	time(&sttime);
	ptime = localtime(&sttime);
	memset(timer, 0, sizeof(GPRS_TIME_TYPE));
	timer->year = 1900 + ptime->tm_year;
	timer->month = 1 + ptime->tm_mon;
	timer->day = ptime->tm_mday;

	timer->hour = ptime->tm_hour;
	timer->minute = ptime->tm_min;
	timer->second = ptime->tm_sec;
	

	
}


void clear_gprs_powerup_time(GPRS_ATTR *gprs)
{
    gprs->gprsInfo.timer.startTimeForCal  = 0;
    memset(&(gprs->gprsInfo.timer.startTime), 0, sizeof(GPRS_TIME_TYPE));	
}

long  get_sys_uptime(void)
{
    struct  sysinfo     stInfo;
    sysinfo(&stInfo);
    return stInfo.uptime;
}

void	get_gprs_powerup_time( GPRS_ATTR *gprs )
{
       
	get_current_time( &( gprs->gprsInfo.timer.startTime));
   	gprs->gprsInfo.timer.startTimeForCal = get_sys_uptime();
  
}


int get_gprs_poweron_uptime( GPRS_ATTR *gprs )
{

	long		int    poweron_uptime = 0 , tmp = 0;  
	long        int    cur_uptime = get_sys_uptime() ;

	if( ( cur_uptime<  gprs->gprsInfo.timer.startTimeForCal ) )
		return FAILURE;

	if( POWER_ON == gprs->gprsInfo.power )
	{
		poweron_uptime = cur_uptime - gprs->gprsInfo.timer.startTimeForCal ;

		int days=0, hours=0,minutes=0, seconds =0;
    		days 	=   poweron_uptime / ONEDAY ;
		gprs->gprsInfo.timer.timeLen.day = days;
		
    		tmp     =   poweron_uptime - days * ONEDAY;
   		hours =	 tmp / ONEHOUR;
	   	gprs->gprsInfo.timer.timeLen.hour = hours;
		
   		tmp     =   tmp - hours * ONEHOUR;
   		minutes = tmp / ONEMIN;
	   	gprs->gprsInfo.timer.timeLen.minute = minutes;
		
    		tmp     =   tmp - minutes * ONEMIN;
		seconds = tmp / ONESECOND;
		gprs->gprsInfo.timer.timeLen.second = seconds;
		
		return OK;
	}
	else	
		return POWER_OFF;
	
}

/********************************************************************************************
* Function      : int select_sim( GPRS_ATTR  *gprs);
* Description   : If  no sim  can  be used ( because  sim_door  isn't  closed ,or check_gprs_status( ) 
*					return ERR ), return  FAILURE;  only if  one  sim  can be used , return SUCCESS                    
*
* Parameter     : GPRS_ATTR *gprs
*
* Return        : int , if sim_door1 sim_door2 are all opened or the sim check gprs_status failure, return
*				  FAILURE  
* Author		: GuoTieCheng( GHL.WuHan )
*********************************************************************************************/

int select_sim(GPRS_ATTR * gprs)
{
//  gprs->sim_door.cur_sim_slot  = 0 ;
	int ucRet = 0;

retry:
	gprs->curSimDoorNo = 
		((gprs->simDoor1.simDoorState == 1&& gprs->sim_1_Failure != 1) ? 1 : (gprs->simDoor2.simDoorState == 1 ? 2 : 0));
																						
	gprs->sim_1_Failure = 0;

	if (0 == gprs->curSimDoorNo)	// 0  sim_door1  sim_door2  are  all opened  , no sim can be used 
	{
		nc_log (&logger, NC_MOBILED_NAME, NA_LOG_ERROR, "@%04d  SimDoor1  AND  SimDoor2  are  all  opened ! Please  insert  SIM !! ", __LINE__);
		return FAILURE;
	}
	else
	{
		ucRet = gprs_set_sim_slot(gprs, gprs->curSimDoorNo);
		if (OK != ucRet)
		{
			if (SIM_DOOR_1 == gprs->curSimDoorNo)
			{
				gprs->sim_1_Failure = 1;
				goto retry;
			}
			else
			{
				nc_log (&logger, NC_MOBILED_NAME, NA_LOG_ERROR, "@%04d  Sim1  and  Sim2  are  all  can't  be  used !", __LINE__);
				return FAILURE;
			}
		}

		gprs->curSimNo = gprs->curSimDoorNo;
#if 0
		ucRet = gprs_reset(gprs);
		sleep(10);
		if (OK != ucRet)
		{
			logs(LOG_CRIT, "Gprs  reset  error ! \n");
			return FAILURE;
		}
#endif

		msleep(10000);
		ucRet = check_gprs_status(gprs);
		if (OK == ucRet)
		{
			SIM *pSim = NULL;
			if (SIM_1 == gprs->curSimNo)
			{
				pSim = &(gprs->sim1);
				nc_log(&logger, NC_MOBILED_NAME, NA_LOG_WARNING, "@%04d  Select  sim  %d  work!", __LINE__, gprs->curSimNo);
			}
			else if (SIM_2 == gprs->curSimNo)
			{
				pSim = &(gprs->sim2);
				nc_log (&logger, NC_MOBILED_NAME, NA_LOG_ERROR, "@%04d  Select  sim  %d work!", __LINE__, gprs->curSimNo);
			}
			else
				return FAILURE;

			ucRet = check_primary_sim(pSim->carrier, gprs);
			if ((NO_CONFIG_PRIMARY_SIM == ucRet) || (OK == ucRet))
			{
				gprs->curSimNo = gprs->curSimDoorNo;
			}
			else
			{
				if (SIM_2 == gprs->curSimNo || (0 == gprs->simDoor2.simDoorState))
				{
					gprs->curSimNo = gprs->curSimDoorNo;
				}
				else										//curSim is sim1, and simDoor2 is close.
				{
					switch_sim(gprs->curSimNo, gprs);
					ucRet = check_gprs_status(gprs);
					if (OK != ucRet)
					{
						switch_sim(gprs->curSimNo, gprs);
					}

				}
			}
			nc_log(&logger, NC_MOBILED_NAME, NA_LOG_DETAIL,
						 "@%04d Gprs current sim is sim %d\n", __LINE__, gprs->curSimNo);
			return SUCCESS;
		}
		else
		{
			if (SIM_DOOR_1 == gprs->curSimDoorNo)
			{
				gprs->sim_1_Failure = 1;
				nc_log (&logger, NC_MOBILED_NAME, NA_LOG_ERROR, "@%04d  Check gprs sim1 status   return  Failure .", __LINE__);

				goto retry;
			}
			else
			{
				gprs->curSimNo = 0;
				//logs(LOG_ERRO, "Sim1  and  Sim2  are  all  can't  be  used . \n");
				nc_log (&logger, NC_MOBILED_NAME, NA_LOG_ERROR, "@%04d  Sim1  and  Sim2  are  all  can't  be  used .", __LINE__);

				return FAILURE;
			}
		}
	}

}

int switch_sim(int currentSim, GPRS_ATTR * gprs)
{
	int simChangeTo;
	if (SIM_1 == currentSim)
		simChangeTo = SIM_2;
	else if (SIM_2 == currentSim)
		simChangeTo = SIM_1;
	else
		return FAILURE;

	gprs_set_sim_slot(gprs, simChangeTo);
	gprs->curSimNo = simChangeTo;
	gprs_reset(gprs);

	return OK;

}

/********************************************************************************************
* Function      : int select_apn( GPRS_ATTR  *gprs);
* Description   : select apn  from  history_apn  or  gprs_conf.xml  or  apns_conf.xml
*                   
* Parameter     : GPRS_ATTR *gprs 
*
* Return        : FAILURE  or  SUCCESS 
*
*********************************************************************************************/

int select_apn(GPRS_ATTR * gprs)
{
	SIM *pSim = NULL;
	if (SIM_1 == gprs->curSimNo)
	{
		pSim = &(gprs->sim1);
		nc_log (&logger, NC_MOBILED_NAME, NA_LOG_DETAIL,  "@%04d  Select  apn for sim  %d  now .", __LINE__, gprs->curSimNo);
		
	}
	else if (SIM_2 == gprs->curSimNo)
	{
		pSim = &(gprs->sim2);
		nc_log (&logger, NC_MOBILED_NAME, NA_LOG_DETAIL, "@%04d  Select  apn for sim  %d  now .", __LINE__, gprs->curSimNo);
	}
	else
		return SIM_DOOR_ERROR;

	switch (pSim->apnFlag)
	{

	case (APN_NULL):							//  apnFlag = APN_NULL  
		if (0 != strlen(pSim->apnHistory))	// apnHistory is not  NULL 
		{
			if (FAILURE == gprs_set_apn(gprs, pSim->apnHistory))
			{
				return FAILURE;
			}
			pSim->apnFlag = APN_HISTORY;
			nc_log (&logger, NC_MOBILED_NAME, NA_LOG_DETAIL, "@%04d  Select  APN  from HistoryApn .", __LINE__);
			break;
		}
	case (APN_HISTORY):					//  apnFlag =   APN_HISTORY
		if (0 != strlen(pSim->apnGprsConf))	//  apnGprsConf is not NULL 
		{
			if (FAILURE == gprs_set_apn(gprs, pSim->apnGprsConf))
			{
				return FAILURE;
			}
			pSim->apnFlag = APN_GPRS_CONF;
			nc_log (&logger, NC_MOBILED_NAME, NA_LOG_DETAIL, "@%04d  Select  APN  from  mobiled.conf: mobiled.sim  . ", __LINE__);
			break;
		}
	case (APN_GPRS_CONF):				//  apnFlag =  APN_GPRS_CONF
		if (OK == gprs_get_carrier(gprs->curSimNo, gprs))
		{
			if (OK == parse_apn_conf(gprs))
			{
				if (0 != strlen(pSim->apnApnsConf))
				{
					if (FAILURE == gprs_set_apn(gprs, pSim->apnApnsConf))
						return FAILURE;
					pSim->apnFlag = APN_APNS_CONF;
					nc_log (&logger, NC_MOBILED_NAME, NA_LOG_DETAIL, "@%04d  Select  APN  from  mobiled.conf: mobiled.apns . ", __LINE__);
					break;
				}
			}
		}
	case (APN_APNS_CONF):				//  apnFlag = APN_APNS_CONF
		pSim->apnFlag = APN_NULL;
		nc_log (&logger, NC_MOBILED_NAME, NA_LOG_WARNING, "@%04d  Select  APN failure !", __LINE__);
		return FAILURE;
		break;
	default:
		return FAILURE;
		break;
	}

	return OK;

}

/********************************************************************************************
* Function      : int  check_gprs_status( GPRS_ATTR  *gprs);
* Description   : 1	check_gprs_insert() , judge  the sim in the door  is a  usefule  card ;   
*                 2	check_gprs_register() , judge  the  sim  in the  door  is  registed;
*                 3	check_gprs_signal(), judge  now   the  signal strenth is  normal ?
*			//	  4	check_gprs_carrier(), judge the  ISP of the  sim  //
*				  if  all  are  OK  above , return OK; else  return  correspond err_code .
* Parameter     : GPRS_ATTR  *gprs
*
* Return        : correspond err_code or  SUCCESS 
*
*********************************************************************************************/

int check_gprs_status(GPRS_ATTR * gprs)
{
	int retries = 3;

CHECK_SIGNAL:
	if (OK != check_gprs_signal(gprs))	//  AT+CSQ?
	{
		if (retries > 0)
		{
			msleep(5000);
			retries--;
			goto CHECK_SIGNAL;
		}
		else
		{
			nc_log (&logger, NC_MOBILED_NAME, NA_LOG_WARNING, "@%04d  Check gprs :  signal  failure . ", __LINE__);
			return GPRS_SIGNAL_ERROR;
		}
	}
	nc_log (&logger, NC_MOBILED_NAME, NA_LOG_DETAIL, "@%04d  Check gprs :  signal  is OK  . ", __LINE__);

	retries = 3;
CHECK_INSERT:
	if (OK != check_sim_insert(gprs))	//  AT+CPIN? 
	{
		if (retries > 0)
		{
			msleep(5000);
			retries--;
			goto CHECK_INSERT;
		}
		else
		{
			nc_log (&logger, NC_MOBILED_NAME, NA_LOG_ERROR, "@%04d  Check gprs : gprs  sim  insert  error !", __LINE__);
			return SIM_INSERT_ERROR;
		}
	}
	nc_log (&logger, NC_MOBILED_NAME, NA_LOG_DETAIL, "@%04d  Check gprs : sim   is ready  . ", __LINE__);

	retries = 3;
CHECK_REGISTER:
	if (OK != check_gprs_register(gprs))	//   AT+CREG\n  x (X=2ÍÑÍø£¬X=1×¢²á£¬X=0×´Ì¬²»Ã÷)
	{
		if (retries > 0)
		{
			msleep(5000);
			retries--;
			goto CHECK_REGISTER;
		}
		else
		{
			nc_log (&logger, NC_MOBILED_NAME, NA_LOG_ERROR, "@%04d  Check gprs :  regist  error !", __LINE__);
			return GPRS_REGISTER_ERROR;
		}
	}
	nc_log (&logger, NC_MOBILED_NAME, NA_LOG_DETAIL, "@%04d  Check gprs :  register   OK . ", __LINE__);

	return OK;

}

/***************************************************************** 	

				gprs  sim_door  manager  functions   

******************************************************************/

//  if  not  set , it return 1
int gprs_get_sim_slot(GPRS_ATTR * gprs, int sim_number)
{
	int sim_slot_number = 0;
	if ((sim_slot_number = ioctl(gprs->ctrl_port.fd_com, GPRS_GET_SIMSLOT, sim_number)) < 0)
	{
		nc_log (&logger, NC_MOBILED_NAME, NA_LOG_ERROR, "@%04d  Ioctl COM  GPRS_GET_SIM_SLOT error !", __LINE__);
		return FAILURE;
	}
	return sim_slot_number;
}

int gprs_set_sim_slot(GPRS_ATTR * gprs, int sim_number)
{
	int ucRet = 0;
	/*
	   int the  driver , GPRS_SET_SIMSLOT cmd  means two steps :
	   uc864e_set_sim (sim);
	   uc864e_reset ();
	 */
	if ((ucRet = ioctl(gprs->ctrl_port.fd_com, GPRS_SET_SIMSLOT, sim_number)) < 0)
	{
//		nc_log (&logger, NC_MOBILED_NAME, NA_LOG_ERROR, "@%04d  Ioctl COM  GPRS_SET_SIM_SLOT error !", __LINE__);
		return FAILURE;
	}
	else if (ucRet == 0)
	{
//		nc_log (&logger, NC_MOBILED_NAME, NA_LOG_NORMAL, "@%04d  Gprs   selected sim %ld", __LINE__, sim_number);
	}

	return OK;
}

/********************************************************************************************
* Function      : int  check_sim_door( GPRS_ATTR  *gprs);
* Description   : check  sim1  and  sim2  are close  or open 
*					gprs->simDoor1.simDoorState   gprs->simDoor2.simDoorState
* Parameter     : GPRS_ATTR  *gprs
*
* Return        : int , OK or  FAILURE  
*
*********************************************************************************************/
int check_gprs_sim_door(GPRS_ATTR * gprs)
{
	int ucRet = 0;

	if ((ucRet = ioctl(gprs->ctrl_port.fd_com, GPRS_CHK_SIM, SIM_DOOR_1)) < 0)
	{
		nc_log (&logger, NC_MOBILED_NAME, NA_LOG_ERROR, "@%04d  Ioctl COM  GPRS_CHK_SIM error !", __LINE__);
		return FAILURE;
	}
	else if (0 == ucRet)
	{
		nc_log (&logger, NC_MOBILED_NAME, NA_LOG_NORMAL, "@%04d  Gprs  SimDoor 1  is  closed . ", __LINE__);
		gprs->simDoor1.simDoorState = 1;	// SIM_DOOR_1 is closed
	}
	else
	{
		gprs->simDoor1.simDoorState = 0;	// SIM_DOOR_1 is opened 
		nc_log (&logger, NC_MOBILED_NAME, NA_LOG_NORMAL, "@%04d  Gprs  SimDoor 1  is  opend . ", __LINE__);
	}

	if ((ucRet = ioctl(gprs->ctrl_port.fd_com, GPRS_CHK_SIM, SIM_DOOR_2)) < 0)
	{
		nc_log (&logger, NC_MOBILED_NAME, NA_LOG_ERROR, "@%04d  GPRS_CHK_SIM ioctl  error !", __LINE__);
		return FAILURE;
	}
	else if (0 == ucRet)
	{
		nc_log (&logger, NC_MOBILED_NAME, NA_LOG_NORMAL, "@%04d  Gprs  simDoor 2 is closed .", __LINE__);
		gprs->simDoor2.simDoorState = 1;	// SIM_DOOR_2 is closed
	}
	else
	{
		nc_log (&logger, NC_MOBILED_NAME, NA_LOG_NORMAL, "@%04d  Gprs simDoor 2  is opened . ", __LINE__);
		gprs->simDoor2.simDoorState = 0;	// SIM_DOOR_2 is opened 
	}

	return OK;

}

/***************************************************************** 	

				gprs power  manager   functions   

******************************************************************/

 /*  gprs  power  manage  : power up      power  down           reset */
int gprs_reset(GPRS_ATTR * gprs)
{

	if (MODULE_UC864E == gprs->gprsInfo.module)
	{
		if (ioctl(gprs->ctrl_port.fd_com, GPRS_RESET, gprs->curSimNo) < 0)
		{
			nc_log (&logger, NC_MOBILED_NAME, NA_LOG_ERROR, "@%04d  Ioctl COM  reset error !", __LINE__);
			return FAILURE;
		}
	}
	else
	{
		gprs_power_down(gprs);
		gprs_power_up(gprs);				//  the  sim_number is  only  as  a  arg  .  

	}

	if (POWER_ON == check_gprs_power(gprs))
	{
		get_gprs_powerup_time( gprs);
		nc_log (&logger, NC_MOBILED_NAME, NA_LOG_NORMAL, "@%04d  Gprs  reset  success . ", __LINE__);
		return OK;
	}
	else
	{
		nc_log (&logger, NC_MOBILED_NAME, NA_LOG_WARNING, "@%04d  Gprs reset  failure . ", __LINE__);
		return FAILURE;
	}

}

int gprs_power_up(GPRS_ATTR * gprs)
{
	int	retry = 0;
	if (POWER_ON == check_gprs_power(gprs))
	{
		nc_log (&logger, NC_MOBILED_NAME, NA_LOG_ERROR, "@%04d  GPRS  already power up now.", __LINE__);
		return OK;
	}

POWERUP:
	if (ioctl(gprs->ctrl_port.fd_com, GPRS_POWERON, 1) < 0)
	{
		nc_log (&logger, NC_MOBILED_NAME, NA_LOG_ERROR, "@%04d  Ioctl power up GPRS module failure.", __LINE__);
		return FAILURE;
	}

	sleep(10);										
	if (POWER_OFF == check_gprs_power(gprs))
	{
		if( retry > 0 )
		{
		retry--;
		goto  POWERUP;
		}
		nc_log (&logger, NC_MOBILED_NAME, NA_LOG_ERROR, "@%04d  AT check GPRS Power up failure.", __LINE__);
		return FAILURE;
	}

	get_gprs_powerup_time( gprs );
	return OK;
}

int gprs_power_down(GPRS_ATTR * gprs)
{
	unsigned char ucRet;
	int retry = 3;

	ucRet = check_gprs_power( gprs );

	if( POWER_OFF == ucRet )
		nc_log (&logger, NC_MOBILED_NAME, NA_LOG_NORMAL, "@%04d  Gprs already powered down.", __LINE__);

	ucRet = send_atcmd(&gprs->data_port, "AT+CFUN=0\r", "OK", "ERROR", 1000, 500,	 2000, NULL);
	goto CHECK;

HARDOFF:
	if (ioctl(gprs->ctrl_port.fd_com, GPRS_POWERDOWN, 0) < 0)
	{
		nc_log (&logger, NC_MOBILED_NAME, NA_LOG_ERROR, "@%04d  Ioctl power down GPRS module failure.", __LINE__);
		return FAILURE;
	}	
	sleep(5);											// wait a mement , then , check

CHECK:
	sleep(5);
	ucRet = check_gprs_power(  gprs );
	if (POWER_ON == ucRet)
	{
		if (retry > 0)
		{
			retry--;
			goto HARDOFF;
		}
		else
		{
			nc_log (&logger, NC_MOBILED_NAME, NA_LOG_NORMAL, "@%04d  Try to use ioctl to shutdown the GPRS module.", __LINE__);
			return FAILURE;
		}
	}	
	
	clear_gprs_powerup_time( gprs );
	gprs->gprsInfo.power = POWER_OFF;
	
	return OK;

}

//  On  return POWER_ON;  Off  return POWER_OFF
int check_gprs_power(GPRS_ATTR * gprs)
{
	unsigned ucRet;
	ucRet =
			send_atcmd(&gprs->data_port, "AT\r", "OK", "ERROR", 500, 100, 300, NULL);
	if (0x00 != ucRet && 0x03 != ucRet)	// GPRS  not  return  OK  and  not  return   ERROR, no respond
	{
		gprs->gprsInfo.power = POWER_OFF;
		goto RET;
	}

	gprs->gprsInfo.power = POWER_ON;

RET:
	return gprs->gprsInfo.power;
}

/***************************************************************** 	

				gprs  apn  manager  functions 

******************************************************************/

int gprs_get_carrier(int simNum, GPRS_ATTR * gprs)
{
	//judge sim-simNum is exist 

	unsigned char ucRet;
	int i = 0;
	int iRetries = 4;
	char *pcStart = NULL;
	char content[SIZE_1024];
	memset(content, 0, sizeof(content));

	SIM *pSim = NULL;

	if (OK != (ucRet = ioctl(gprs->ctrl_port.fd_com, GPRS_CHK_SIM, simNum)))
	{
		nc_log (&logger, NC_MOBILED_NAME, NA_LOG_ERROR, "@%04d  Ioctl COM  GPRS_CHK_SIM error !", __LINE__);
		return FAILURE;
	}

	gprs_set_sim_slot(gprs, simNum);

	if (POWER_ON != gprs->gprsInfo.power)
	{
		if (FAILURE == gprs_power_up(gprs))
			return FAILURE;
	}

	if (SIM_1 == simNum)
		pSim = &(gprs->sim1);
	else if (SIM_2 == simNum)
		pSim = &(gprs->sim2);
	else
		return FAILURE;

RETRY:
	ucRet = send_atcmd(&gprs->data_port, "AT+COPS?\r", "OK", "ERROR", 100, 100, 4000, content);
	if (0x00 == ucRet)
	{
		pcStart = strchr(content, '\"');
		if (NULL != pcStart)
		{
			while ('\"' != *(++pcStart) && i < CARRIER_LEN)
			{
				pSim->carrier[i] = *pcStart;
				i++;
			}
		}
		pSim->carrier[i++] = '\0';
		nc_log (&logger, NC_MOBILED_NAME, NA_LOG_NORMAL, "@%04d  Get sim carrier: %s", __LINE__, pSim->carrier);
		if ('\0' != pSim->carrier[0])
		{
			return OK;
		}
	}

	if (iRetries > 0)
	{
		iRetries--;
		goto RETRY;
	}

	pSim->carrier[0] = '\0';
	return FAILURE;

}

int check_primary_sim(char *carrier, GPRS_ATTR * gprs)
{
	if (0 == *gprs->primarySimCarrier)	// User not config primaryCarrier in gprs.conf 
		return NO_CONFIG_PRIMARY_SIM;
	else if (!(strcmp(carrier, gprs->primarySimCarrier)))
		return OK;
	else
		return FAILURE;
}

#if 0
int gprs_get_carrier(GPRS_ATTR * gprs)
{
	unsigned char ucRet;
	int i = 0;
	int iRetries = 4;
	char *pcStart = NULL;
	char content[SIZE_1024];
	memset(content, 0, sizeof(content));
	SIM *pSim = NULL;

	if (SIM_1 == gprs->curSimNo)
		pSim = &(gprs->sim1);
	else if (SIM_2 == gprs->curSimNo)
		pSim = &(gprs->sim2);
	else
		return FAILURE;

RETRY:
	ucRet =
			send_atcmd(&gprs->data_port, "AT+COPS?\r", "OK", "ERROR", 100, 100, 4000,
								 content);
	if (0x00 == ucRet)
	{
		pcStart = strchr(content, '\"');
		if (NULL != pcStart)
		{
			while ('\"' != *(++pcStart) && i < CARRIER_LEN)
			{
				pSim->carrier[i] = *pcStart;
				i++;
			}
		}
		pSim->carrier[i++] = '\0';
		logs(LOG_DEBG, "Get Network Operation: \"%s\"\n", pSim->carrier);
		if ('\0' != pSim->carrier[0])
		{
			return OK;
		}
	}

	if (iRetries > 0)
	{
		iRetries--;
		goto RETRY;
	}

	pSim->carrier[0] = '\0';
	return FAILURE;

}
#endif

/* */
int gprs_set_apn(GPRS_ATTR * gprs, char *apn)
{
	char cmd[64];
	int ucRet = 0;
	memset(cmd, 0, sizeof(cmd));

	sprintf(cmd, "AT+CGDCONT=1,\"IP\",\"%s\"\r", apn);
	ucRet = send_atcmd(&gprs->data_port, cmd, "OK", "ERROR", 0, 0, 5000, NULL);
	if (OK == ucRet)
	{
		nc_log (&logger, NC_MOBILED_NAME, NA_LOG_NORMAL, "@%04d  Set  apn %s", __LINE__, apn);
		return OK;
	}
	else
		return FAILURE;

}

/***************************************************************** 	

				gprs  status  manager functions

******************************************************************/

// Sim  inserted,   return OK;  SIM  not  inserted, FAILURE
int check_sim_insert(GPRS_ATTR * gprs)
{
	int ucRet = 0, i = 0;

	for (i = 0; i < 2; i++)
	{
		ucRet = send_atcmd(&gprs->data_port, "AT+CPIN?\r", "+CPIN: READY\r\n\r\nOK", "ERROR", 0, 0, 5000, NULL);
		if (OK == ucRet)
		{
			break;
		}
		sleep(5);
	}

	if (OK == ucRet)
	{
		return OK;
	}
	else
	{
		return FAILURE;
	}
}

/* AT + CREG? */
int check_gprs_register(GPRS_ATTR * gprs)
{
	unsigned char ucRet;
	char acRes[SIZE_128];
	char acRegist[4] = { 0 };
	int iRegist;
	int iRetries = 6;
	char *pcStart = NULL;
	char *pcEnd = NULL;

	SIM *pSim = NULL;

	char content[SIZE_1024];
	memset(content, 0, sizeof(content));

	if (SIM_1 == gprs->curSimNo)
		pSim = &(gprs->sim1);
	else if (SIM_2 == gprs->curSimNo)
		pSim = &(gprs->sim2);
	else
		return FAILURE;

RETRY:
	strcpy(acRes, "+CREG: 0,1\r\n\r\nOK    +CREG: 1,1\r\n\r\nOK    +CREG: 0,5\r\n\r\nOK    +CREG: 1,5\r\n\r\nOK");
	ucRet = send_atcmd(&gprs->data_port, "AT+CREG?\r", "OK", "ERROR", 100, 0, 1000, content);
	if (0x00 == ucRet)						/* Get Reply */
	{
		pcStart = strstr(content, "+CREG: ");	/*pcStart: "+CREG: 1,1***" */
		if (NULL != pcStart)
		{
			pcStart += 7;							/* pcStart: "1,1****" */
			pcStart = strchr(pcStart, ',');	/* pcStart: ",1*****" */
			if (NULL != pcStart)
			{
				pcStart += 1;
				pcEnd = strchr(pcStart, '\r');

				if (NULL != pcEnd && 0 < (pcEnd - pcStart)
						&& sizeof(acRegist) > (pcEnd - pcStart))
				{
					strncpy(acRegist, pcStart, pcEnd - pcStart);
					iRegist = atoi(acRegist);

					if (REG_HOMEWORK == iRegist)
					{
						nc_log (&logger, NC_MOBILED_NAME, NA_LOG_WARNING, "@%04d  SIM card Regist Home Work.", __LINE__);
						pSim->regist = REG_HOMEWORK;
						return OK;
					}
					else if (REG_ROAMING == iRegist)
					{
						nc_log (&logger, NC_MOBILED_NAME, NA_LOG_WARNING, "@%04d  SIM card Regist Roaming.", __LINE__);
						pSim->regist = REG_ROAMING;
						return OK;
					}
				}

			}													/* NULL!=pcStart */

		}														/* NULL!=pcStart */

	}															/* 0x00==ucRet */

	if (iRetries > 0)
	{
		sleep(3);
		iRetries--;
		goto RETRY;
	}

	nc_log (&logger, NC_MOBILED_NAME, NA_LOG_WARNING, "@%04d  Check SIM card Regist:    [FAILURE].", __LINE__);
	pSim->regist = REG_UNREGIST;
	return FAILURE;
}

/* 1 ~ 31  return OK, else  return ERR */
int check_gprs_signal(GPRS_ATTR * gprs)
{
	int ucRet = 0;
	short int iSignal = 0;

	char *ptr_start = NULL, *ptr_end = NULL;

	char acBuf[16];

	char content[SIZE_1024];
	memset(content, 0, sizeof(content));

	SIM *pSim = NULL;

	if (SIM_1 == gprs->curSimNo)
		pSim = &(gprs->sim1);
	else if (SIM_2 == gprs->curSimNo)
		pSim = &(gprs->sim2);

	/* AT + CSQ ? */
	ucRet = send_atcmd(&gprs->data_port, "AT+CSQ\r", "OK", "ERROR", 0, 0, 5000, content);

	if (OK == ucRet)
	{
		ptr_start = strstr(content, "+CSQ: ");	//  "+CSQ: 23,99"  
		if (0 != ptr_start)
		{
			ptr_start += 6;
			ptr_end = strchr(ptr_start, ',');

			if (0 != ptr_end && 0 < (ptr_end - ptr_start)
					&& sizeof(content) > (ptr_end - ptr_start))
			{
				memset(acBuf, 0x00, sizeof(acBuf));
				strncpy(acBuf, ptr_start, ptr_end - ptr_start);

				iSignal = atoi(acBuf);
				pSim->signal = iSignal;

				if ((1 <= iSignal) && (10 >= iSignal))
				{
					nc_log (&logger, NC_MOBILED_NAME, NA_LOG_ERROR, "@%04d  Now GPRS Signal Value is  low : %d", __LINE__, iSignal);
					return OK;
				}
				else if ((11 <= iSignal) && (15 >= iSignal))
				{
					nc_log (&logger, NC_MOBILED_NAME, NA_LOG_WARNING, "@%04d  Now the signal value is %d!It is not strong enough !", __LINE__, iSignal);
					return OK;
				}
				else if ((16 <= iSignal) && (31 >= iSignal))
				{
					nc_log (&logger, NC_MOBILED_NAME, NA_LOG_ERROR, "@%04d  Now the signal value is %d . It is strong !", __LINE__, iSignal);
					return OK;
				}
				else if (99 == iSignal || iSignal < 0)
				{
					nc_log (&logger, NC_MOBILED_NAME, NA_LOG_ERROR, "@%04d  Now  GPRS Signal Value is %d ! There is no signal !", __LINE__, iSignal);
					return ERR;
				}
			}													/*if (0 != ptr_end && 0 < (ptr_end - ptr_start) && sizeof (content) > (ptr_end - ptr_start)) */
		}														/*if (0 != ptr_start) */

		nc_log (&logger, NC_MOBILED_NAME, NA_LOG_NORMAL, "@%04d  Didn't   find  +CSQ!", __LINE__);
	}															/*if (OK == ucRet) */

	nc_log (&logger, NC_MOBILED_NAME, NA_LOG_NORMAL, "@%04d  Send AT cmd AT+csq  ERR!", __LINE__);

	return ERR;
}

/***************************************************************** 	

				gprs AT CMD   manager  functions   

******************************************************************/

unsigned char at_match(char *p_pcStr, char *p_pcMatch)
{
	char acBuf[256], *pcStart = NULL, *pcTab = NULL;

	pcStart = p_pcMatch;

	while (0 != pcStart)
	{
		memset(acBuf, 0x00, sizeof(acBuf));

		pcTab = strchr(pcStart, 9);	// Find for TAB

		if (0 != pcTab)
		{
			if (pcTab != pcStart)
			{
				strncpy(acBuf, pcStart, pcTab - pcStart);
			}

			pcStart = (0 != *(++pcTab)) ? pcTab : 0;
		}
		else
		{
			strcpy(acBuf, pcStart);
			pcStart = NULL;
		}

		if (0 != acBuf[0] && 0 != strstr(p_pcStr, acBuf))
		{
			return 0x00;
		}
	}

	return 0x01;
}

int send_atcmd(COM_PORT * st_comport, char *atCmd, char *expect, char *error,
							 unsigned long delay, unsigned long gap, unsigned long timeout,
							 char *content)
{
	unsigned char ucRet = 0x00;
	unsigned long ulCurTime, ulStartTime;

	int iCmdLen = 0, iRecvLen = 0, iRecvSize = 0, iRet = 0;
	char acRecv[SIZE_1024];
	char *pcRecvPtr = NULL;

	if (st_comport->is_connted != 0x01)	/* Comport not opened */
	{
	//	nc_log (&logger, NC_MOBILED_NAME, NA_LOG_ERROR, "@%04d  Comport not opened.", __LINE__);
		return 0x01;
	}

		 /*=========================================
      *=  Pause a while before send AT command =
      *=========================================*/
	if (0 != delay)
	{
		GET_TICKCOUNT(ulCurTime);
		ulStartTime = ulCurTime;
		while ((ulCurTime - ulStartTime) < delay && ulStartTime <= ulCurTime)
		{
			SLEEP(1);
			GET_TICKCOUNT(ulCurTime);
		}
	}

		 /*====================
      *=  Throw Rubbish   =
      *====================*/
	GET_TICKCOUNT(ulCurTime);
	ulStartTime = ulCurTime;

	memset(&acRecv, 0, sizeof(acRecv));
	while ((ulCurTime - ulStartTime) < timeout && ulStartTime <= ulCurTime)
	{
		ucRet = comport_recv(st_comport, (unsigned char *)acRecv, 1, &iRet, 50);
		if (0x00 != ucRet || 0 == iRet)
		{
			break;
		}

		SLEEP(1);
		GET_TICKCOUNT(ulCurTime);
	}

		 /*====================
      *=  Send AT command =
      *====================*/

	iCmdLen = strlen(atCmd);
	ucRet = comport_send(st_comport, (unsigned char *)atCmd, iCmdLen);
	if (0 != ucRet)
	{
		ucRet = 0x02;
		goto CleanUp;
	}

		 /*===================================================
      *=  Pause a while before read command response. 
      *===================================================*/
	if (0 != gap)
	{
		GET_TICKCOUNT(ulCurTime);
		ulStartTime = ulCurTime;
		while ((ulCurTime - ulStartTime) < gap && ulStartTime <= ulCurTime)
		{
			SLEEP(1);
			GET_TICKCOUNT(ulCurTime);
		}
	}

	memset(acRecv, 0, sizeof(acRecv));
	pcRecvPtr = acRecv;
	iRecvLen = 0;
	iRecvSize = sizeof(acRecv);

	GET_TICKCOUNT(ulCurTime);
	ulStartTime = ulCurTime;

	while ((ulCurTime - ulStartTime) < timeout && ulStartTime <= ulCurTime)
	{
		if (iRecvLen < (iRecvSize - 1))
		{
			ucRet =
					comport_recv(st_comport, (unsigned char *)pcRecvPtr, 1, &iRet, 50);

			if (0x00 == ucRet && 0 < iRet)
			{
				iRecvLen += iRet;
				pcRecvPtr += iRet;
				acRecv[iRecvSize - 1] = 0;

									 /*========================================
                    * Match the received with expect String =
                    *========================================*/
				if (NULL != expect)
				{
					if (0x00 == at_match(acRecv, expect))
					{
						ucRet = 0x00;
	//					nc_log (&logger, NC_MOBILED_NAME, NA_LOG_NORMAL, "@%04d  Get  Correct  reply for command: \"%s\"", __LINE__, atCmd);
						goto CleanUp;
					}
				}

									 /*========================================
                    * Match the received with error String  =
                    *========================================*/
				if (NULL != error)
				{
					if (0x00 == at_match(acRecv, error))
					{

						ucRet = 0x03;
	//					nc_log (&logger, NC_MOBILED_NAME, NA_LOG_NORMAL, "@%04d  Get  ERROR  reply for command: \"%s\"", __LINE__, atCmd);
						goto CleanUp;
					}
				}

			}													/*End of (0x00==ucRet && 0<iRet)  */

			SLEEP(1);
			GET_TICKCOUNT(ulCurTime);
		}														/* End of ( (ulCurTime-ulStartTime)<timeout ) */

		if (NULL == expect)
			ucRet = 0x00;
		else
			ucRet = 0xFF;
	}

CleanUp:
	if (NULL != content)
	{
		memset(content, 0, SIZE_1024);
		strncpy(content, acRecv, SIZE_1024);
	}
//	nc_log (&logger, NC_MOBILED_NAME, NA_LOG_DETAIL, "@%04d  Send AT command:\n  %s \nWith Reply:   %s", __LINE__, atCmd, acRecv);

	return ucRet;

}

int gprs_atcmd_init(COM_PORT * comport)
{
	if (0 == send_atcmd(comport, "ATE0\r", "OK", "ERROR", 0, 0, 5000, NULL))
	{
		return OK;
	}
	else
		return FAILURE;
}

/**********************************************************************

			gprs  port  manager functions 

***********************************************************************/

int ctrlport_open(CTRL_PORT * ctrlport)
{
	ctrlport_close(ctrlport);

	if ((ctrlport->fd_com = open(ctrlport->dev_name, O_RDWR, 0666)) < 0)
	{
		nc_log (&logger, NC_MOBILED_NAME, NA_LOG_ERROR, "@%04d  Open GPRS control port \"%s\" failure.", __LINE__, ctrlport->dev_name);
		return FAILURE;
	}
	nc_log (&logger, NC_MOBILED_NAME, NA_LOG_NORMAL, "@%04d  Open GPRS control port \"%s\" success.", __LINE__, ctrlport->dev_name);

	return OK;
}

int ctrlport_close(CTRL_PORT * ctrlport)
{
	if (0 != ctrlport->fd_com)		// if now  ctrlport is open , we will close it . if it is closed , return ok .
	{
		if (close(ctrlport->fd_com) < 0)
		{
			nc_log (&logger, NC_MOBILED_NAME, NA_LOG_ERROR, "@%04d  Close GPRS control port \"%s\" failure.", __LINE__, ctrlport->dev_name);
			return FAILURE;
		}
		ctrlport->fd_com = 0;
		nc_log (&logger, NC_MOBILED_NAME, NA_LOG_ERROR, "@%04d  Close GPRS control port \"%s\" success.", __LINE__, ctrlport->dev_name);
	}

	return OK;
}

/*This function used to open the GPRS control and data port*/
int open_gprs_port(GPRS_ATTR * gprs)
{
	CTRL_PORT *ctrlport = &(gprs->ctrl_port);
	COM_PORT *comport = &(gprs->data_port);

	if (MODULE_UC864E != gprs->gprsInfo.module)
	{
		if ((ctrlport_open(ctrlport)) < 0)
			return ERR;
	}

	init_comport(comport);				/*Set the baudrate, stopbit, etc. */
	if (0 != comport_open(comport))
	{
		nc_log (&logger, NC_MOBILED_NAME, NA_LOG_ERROR, "@%04d  Open GPRS comport \"%s\" failure.", __LINE__, comport->dev_name);
		ctrlport_close(ctrlport);
		return ERR;
	}
	nc_log (&logger, NC_MOBILED_NAME, NA_LOG_WARNING, "@%04d  Open GPRS Comport \"%s\" success .", __LINE__, comport->dev_name);

	if (MODULE_UC864E == gprs->gprsInfo.module)
	{
		gprs->ctrl_port.fd_com = gprs->data_port.fd_com;
	}

	return OK;

}

/*This function used to close the GPRS control and data port*/
int close_gprs_port(GPRS_ATTR * gprs)
{
	CTRL_PORT *ctrlport = &(gprs->ctrl_port);
	COM_PORT *comport = &(gprs->data_port);

	if (MODULE_UC864E != gprs->gprsInfo.module)
	{
		if ((ctrlport_close(ctrlport)) < 0)
			return FAILURE;
	}

	comport_term(comport);

	return OK;
}

/*
	initial  gprs 
*/
int initial_gprs(GPRS_ATTR * gprs)
{
//     CTRL_PORT   *ctrlport = &(gprs->ctrl_port);
	COM_PORT *comport = &(gprs->data_port);
	int iRetOpenPort = 0,
//         iRetPowerUp = 0,
			iRetAtcmdInit = 0;
	int ucRet = 0, retrys = 3;

//Initial  gprs  ctrl_port   and data_port
	init_gprs_setting(gprs);			/*Initial struct gprs */
	iRetOpenPort = open_gprs_port(gprs);	/*Open the GPRS ctrl and data port */

//Check there  is  at lease  1 SIM  Door  IS in the Slot 
CHK_SIMDOOR:
	ucRet = check_gprs_sim_door(gprs);

	if (gprs->simDoor1.simDoorState == 0 && gprs->simDoor2.simDoorState == 0)
	{
		if (0 < retrys)
		{
			nc_log (&logger, NC_MOBILED_NAME, NA_LOG_ERROR, "@%04d  SIM door1 and SIM door2  are all  opened . ", __LINE__);
			sleep(2);
			retrys--;
			goto CHK_SIMDOOR;
		}
		else
		{
			nc_log (&logger, NC_MOBILED_NAME, NA_LOG_ERROR, "@%04d  SIM DOOR1  and  SIM DOOR 2 are all open ! Please insert a sim now !", __LINE__);
			return FAILURE;
		}
	}

	ucRet = gprs_power_up(gprs);
	if ( POWER_OFF== ucRet)
	{
		nc_log (&logger, NC_MOBILED_NAME, NA_LOG_ERROR, "@%04d  GPRS POWER UP  Failure !", __LINE__);
		return FAILURE;
	}
	
	nc_log (&logger, NC_MOBILED_NAME, NA_LOG_NORMAL, "@%04d  GPRS  POWER UP  SUCCESS!", __LINE__);

	retrys = 3;
	iRetAtcmdInit = gprs_atcmd_init(comport);	/*Send ATE0 */
ATCMD_INIT:
	if (FAILURE == iRetAtcmdInit)
	{
		if (retrys > 0)
		{
			retrys--;
			sleep(3);
			goto ATCMD_INIT;
		}
		else
		{
			nc_log (&logger, NC_MOBILED_NAME, NA_LOG_ERROR, "@%04d  GPRS AT  CMD  init  failure ! ", __LINE__);
			return FAILURE;
		}
	}

	nc_log (&logger, NC_MOBILED_NAME, NA_LOG_NORMAL, "@%04d  GPRS  AT  CMD init  SUCCESS !", __LINE__);

       get_gprs_powerup_time( gprs );
	return OK;

}

/* 
 *  This function used to set the GPRS control and data port from 
 * gprs->module, which get from the XML configure.
 */
void init_gprs_setting(GPRS_ATTR * gprs)
{
	if (MODULE_UC864E == gprs->gprsInfo.module)
	{
		strncpy(gprs->ctrl_port.dev_name, GPRS_UC864E_CTRL_PORT, DEVNAME_LEN);
		strncpy(gprs->data_port.dev_name, GPRS_UC864E_DATA_PORT, DEVNAME_LEN);

	}
	else													/*GTM900B,G600,LC6311,Gospell doesn't use USB driver */
	{
#if 0
		strncpy(gprs->ctrl_port.dev_name, "/dev/gprs", DEVNAME_LEN);
#ifdef ARCH_L200
		strncpy(gprs->data_port.dev_name, "/dev/ttyS1", DEVNAME_LEN);
#elif defined ARCH_L300
		strncpy(gprs->data_port.dev_name, "/dev/ttyS2", DEVNAME_LEN);
#endif

#endif

		strncpy(gprs->ctrl_port.dev_name, GPRS_GTM900B_CTRL_PORT, DEVNAME_LEN);
		strncpy(gprs->data_port.dev_name, GPRS_GTM900B_DATA_PORT, DEVNAME_LEN);

	}

	nc_log (&logger, NC_MOBILED_NAME, NA_LOG_NORMAL, "@%04d  Ctrl port: %s", __LINE__, gprs->ctrl_port.dev_name);
	nc_log (&logger, NC_MOBILED_NAME, NA_LOG_NORMAL, "@%04d  comport: %s", __LINE__, gprs->data_port.dev_name);
}

int close_gprs(GPRS_ATTR * gprs)
{
	gprs_power_down(gprs);
	close_gprs_port(gprs);
	return OK;
}
