
#include <common/common.h>
/*
	Function:	Get  current  datestructure of  /proc/net/dev  
	Return :	fopen  /proc/net/dev  error , can't  find ppp10 , return -1;  else , return  0 
*/
int check_ppp_stat (MOD_PPP_STAT *p_pstPPPStat)
{
	unsigned char	ucRet = 0x00;

	char		acIf [10] = "ppp10";
	char		acBuf [512];
	char		*pcStart;

	FILE		*hFd;

	memset (p_pstPPPStat, 0x00, sizeof (MOD_PPP_STAT));

	hFd = fopen ( MOD_PPP_PROC_NET_DEV, "r");

	if (0 == hFd)
	{
		ucRet = -1;
		goto CleanUp;
	}

//	sprintf (acIf, "ppp%d", p_pstPPP->pstCfg->iIfIndex);

	// Discard line
	fgets (acBuf, sizeof (acBuf), hFd);
	fgets (acBuf, sizeof (acBuf), hFd);

	while (fgets (acBuf, sizeof (acBuf), hFd))
	{
		pcStart = strstr (acBuf, acIf);

		if (0 != pcStart)
		{
			pcStart++;
			pcStart = strchr (pcStart, ':');

			if (0 != pcStart)
			{
				pcStart++;

				// Get packet count
				sscanf (pcStart, "%llu%llu%lu%lu%lu%lu%lu%lu%llu%llu%lu%lu%lu%lu%lu%lu",
					&p_pstPPPStat->ullRx_Bytes,
					&p_pstPPPStat->ullRx_Packets,
					&p_pstPPPStat->ulRx_Errors,
					&p_pstPPPStat->ulRx_Dropped,
					&p_pstPPPStat->ulRx_FIFO_Errors,
					&p_pstPPPStat->ulRx_Frame_Errors,
					&p_pstPPPStat->ulRx_Compressed,
					&p_pstPPPStat->ulRx_Multicast,
					&p_pstPPPStat->ullTx_Bytes,
					&p_pstPPPStat->ullTx_Packets,
					&p_pstPPPStat->ulTx_Errors,
					&p_pstPPPStat->ulTx_Dropped,
					&p_pstPPPStat->ulTx_FIFO_Errors,
					&p_pstPPPStat->ulCollisions,
					&p_pstPPPStat->ulTx_Carrier_Errors,
					&p_pstPPPStat->ulTx_Compressed);

				ucRet = OK;
				//(LOG_INFO, "Gprs  got  /proc/dev/net  information    \n"  );
				break;
			}
			
		}
		else	
			ucRet = NO_PPP_DEV_ERR;
			
	}

CleanUp:
	fclose (hFd);

	return ucRet;
}


/*************************************************************************
* Function     : In 30s,if there is little data from PPP connection,so we
* 		 can consider that the PPP connection is idle;
* 
* Return       : 0x00: PPP_IDLE 0x01:PPP_BUSY  other value: failure
*************************************************************************/
int   check_ppp_idle( MOD_PPP_STAT  *p_pstPPPStartStat , unsigned long long *rBytesThreshold , unsigned long long *sBytesThreshlod)
{
	unsigned char ucRet;
				
	MOD_PPP_STAT  	stPPPCurStat;
					
	memset(&stPPPCurStat, 0, sizeof(stPPPCurStat));
	ucRet = check_ppp_stat(&stPPPCurStat);
	if(ucRet != 0x00)
	{
		//(LOG_ERRO, "Check PPP status failure!\n");
		ucRet = -1;
		goto CleanUp;
	}
	// ping  -c3  ; every packet default is 64 bytes ; 
	if(((stPPPCurStat.ullRx_Bytes- p_pstPPPStartStat->ullRx_Bytes- 3*64*3) < *rBytesThreshold) && ((stPPPCurStat.ullTx_Bytes- p_pstPPPStartStat->ullTx_Bytes- 3*64*3) < *sBytesThreshlod) )
	{
		//(LOG_DEBG, "In 60s there is little data in PPP.The PPP is idle now! \n");
		ucRet  = PPP_IDLE;
		goto CleanUp;
	}
	else 
	{
		ucRet = PPP_BUSY;
		//(LOG_DEBG, "In 60s there is little data in PPP.The PPP is busy now! \n");
		goto CleanUp;
	}

CleanUp:
	return ucRet;
}


int	check_ppp_dev( void  )
{

	int		fd;
	char    buf[ 1024 ];
	
	// Judge ppp10  device 
	if((fd=open("/proc/net/dev", O_RDONLY, 0666)) < 0)
    {
            //(LOG_ERRO, "Open file \"/proc/net/dev\" failure.\n");
    		return	FAILURE;
	}			 
	
	while(  0 < read(fd, buf, sizeof(buf)) )
    {
    	if(NULL != strstr(buf, "ppp10"))
    	{
        	//(LOG_INFO, "PPP10  is  exist .\n");
    		break;
    	}
    		else
    	{
	    	//(LOG_DEBG, "PPP10  is not  exist ! \n");
		    close( fd );
    		return  FAILURE;
    	}
	}

	close( fd );
	return OK;

}



/********************************************************************************************
* Function      : int check_ppp_ping( GPRS_ATTR  *gprs);
* Description   : ping  the  IP  of server  to judge  PPP  is  alive or not 
* Parameter     : GPRS_ATTR  *gprs
*
* Return        : int , packetLoss > 80 % , return  FAILURE  ;  packetLoss < 80 %  , return OK 
*
*********************************************************************************************/
int	check_ppp_ping( GPRS_ATTR *gprs )
{
	pid_t	pid;

	int     fd[2];
  //  int     save_fdout = 0;
    char    buf[2048];
	char	acBuf[16];
    int	    packetLoss = 0;
	char	*pacArgv[4] ;
	char  	*ptr_start = NULL,
			*ptr_end = NULL;

			
	SIM		*pSim = NULL;
	if( SIM_1 == gprs->curSimNo)
		pSim = &( gprs->sim1 );
	else	if( SIM_2 == gprs->curSimNo)
		pSim = &( gprs->sim2 );
	else
		return FAILURE;	
	
	pacArgv [0] = "/bin/ping";
	pacArgv [1] = "-c3";  //  send 5  packets   every  1  seconds 
	pacArgv [2] = pSim->pingIp; //pSimCur->pingIp
	pacArgv [3] = NULL;
	
	
    if (pipe(fd) < 0)
    {
        //(LOG_DEBG, "pipe failure");
        return ERR;
    }

//    save_fdout = dup(STDOUT_FILENO);  // save STDOUT_FILENO to  save_fdout

    pid = vfork();

    if(pid < 0)
    {
      //(LOG_DEBG , "Vfork error!\n");
       return ERR;
    }
	else if(0 == pid)  /*Child process  ;   only for  write  --  dup2(fd[1], STDOUT_FILENO)*/
    {

		close(fd[0]); 	// close read 
		dup2(fd[1], STDOUT_FILENO); //  STDOUT_FILENO = 1 --> fd[1]
		execv(pacArgv[0], pacArgv);
    //      msleep(500);
		exit(0);
    }
    else if (0 < pid)
    {

		wait(NULL); //Wait child exit
		close(fd[1]);//close write endpoint
		//dup2(save_fdout, STDOUT_FILENO);
		memset(buf, 0, sizeof(buf));
		read(fd[0], buf, sizeof(buf));
		if( NULL != (ptr_start = strstr(buf, "errors")))
		{
			ptr_start = strstr (buf, "received"); // ...received, 0% packet loss...
			ptr_start += 7;
		}	
		else		if( NULL != (ptr_start = strstr(buf, "received")))
		{
			ptr_start += 9;  // ... errors, 100% packet loss ...
		}
		else
		{
			return -1;
		}
		
		if (0 != ptr_start)
		{
			ptr_end = strchr (ptr_start, '%');

			if (0 != ptr_end && 0 < (ptr_end - ptr_start) && sizeof (buf) > (ptr_end - ptr_start))
			{
				memset (acBuf, 0x00, sizeof (acBuf));
				strncpy (acBuf, ptr_start, ptr_end - ptr_start);
				packetLoss = atoi (acBuf);
				//(LOG_INFO, " PING server IP packetLoss = %d%% \n", packetLoss );
				if( 80 > packetLoss )
					return OK;
			}
		
		}
				
    }

	//(LOG_DEBG, "PING Server IP  error !\n");
    return FAILURE;

}

/*
	Function:	check ppp10  and  ppp ping is alive
	Return:	ppp10 is  exist  and  ping is alive  ,return OK; else  , return  FAILURE
*/
int  check_ppp_alive( GPRS_ATTR  *gprs )
{
	
// check PPP10 is exist 
	if(FAILURE == check_ppp_dev(  ) )
	{
		return  NO_PPP_DEV_ERR;
	}	
// check PPP ping  is alive	
	if( FAILURE == check_ppp_ping(  gprs ))
	{
		//(LOG_DEBG, "PPP  ping  is  failure .   Net  is not alive . \n");
		return PPP_PING_ERR;
	}

	gprs->gprsInfo.pppAlive = PPP_ALIVE;
	return OK;
	
}

/*
	start  ppp  . return  SCCESS or  FAILURE 
*/
int	ppp_start( GPRS_ATTR   *gprs )
{
	pid_t   pid = -1;
    int		ucRet;	
	
	char	acRate [10]		= { 0 },
			acIfIndex [10]	= { 0 },
			acUID [256]		= { 0 },
			acPWD [256]		= { 0 },
//			acIP [64]		= { 0 },
			*pacArgv [7];
					
	SIM		*pSim = NULL;
	if( SIM_1 == gprs->curSimNo)
		pSim = &( gprs->sim1 );
	else	if( SIM_2 == gprs->curSimNo)
		pSim = &( gprs->sim2 );
	else
		return FAILURE;	
	
	ucRet = check_ppp_alive(gprs);
    if(0x00 == ucRet)
    {
        //(LOG_INFO, "PPP connection is connected.\n"); 
        return 0x00;
    }
	else
	{
		ucRet = ppp_term( gprs );
		if(OK != ucRet)
			return FAILURE;
	}
	
	// /apps/etc/ppp/gprs-start-pap /dev/ttyUC864E0 115200 10
	pacArgv [0] = "/etc/ppp/gprs-start-pap"; 	// /etc/ppp/gprs-start-chap
	//uc864e: "/dev/ttyUC864E0"		
	//GTM900B: L200 /dev/ttyS1  L300 /dev/ttyS2
	pacArgv [1] = gprs->data_port.dev_name; 

	sprintf (acRate, "%d", 115200);
	pacArgv [2] = acRate;
	
	sprintf (acIfIndex, "%d", 10);
	pacArgv [3] = acIfIndex;

	if( APN_HISTORY == pSim->apnFlag)
	{
		sprintf (acUID, "user %s", pSim->userHistory);
		pacArgv [4] = acUID;
		sprintf (acPWD, "password %s", pSim->passwordHistory);
		pacArgv [5] = acPWD;
	}
	else if( APN_GPRS_CONF == pSim->apnFlag )
	{
		sprintf (acUID, "user %s", pSim->userGprsConf);
		pacArgv [4] = acUID;
		sprintf (acPWD, "password %s", pSim->passwordGprsConf);
		pacArgv [5] = acPWD;
	}
	else if( APN_APNS_CONF == pSim->apnFlag)
	{
		sprintf (acUID, "user %s", pSim->userApnsConf);
		pacArgv [4] = acUID;
		sprintf (acPWD, "password %s", pSim->passwordApnsConf);
		pacArgv [5] = acPWD;
	}
	
	pacArgv [6] = NULL;
	
    pid = vfork();
    if(pid < 0) /* vfork Error  */
    {
        //(LOG_CRIT, "Vfork error!\n");
        return ERR;
    }
    else if(0 == pid)  /*Child process*/
    {
        //(LOG_DEBG, "Child Process start ppp connection now.\n");
	 execv(pacArgv[0], pacArgv);
        exit(0);
    }
    else if (0 < pid)
    {   
        wait(NULL); //Wait child exit
        sleep(5);  /* Make sure ppp start shell script excute over*/

        ucRet = check_ppp_alive( gprs );
        if(0x00 != ucRet )
        {
            //(LOG_CRIT, "Child proecss start pppd failure, return code: 0x%0x.\n", ucRet); 
            return FAILURE;
        }
        else
        {
            //(LOG_DEBG, "Child process start pppd successfully.\n");        
            return SUCCESS;
        }
    }

    return FAILURE;

}


int	ppp_term( GPRS_ATTR *gprs )
{

    unsigned char    ucRet;
    pid_t            pid = -1;
	
	char	*pacArgv[3] ;
	pacArgv [0] = "/etc/ppp/gprs-close";
	pacArgv [1] = "10";
	pacArgv [2] = NULL;
	
//PPP_DEV_ERR means there is no PPP10 dev , so we don't neet to close it 
    ucRet = check_ppp_alive( gprs );
    if( NO_PPP_DEV_ERR == ucRet )
    {
        //(LOG_INFO, "PPP has disconnected.\n"); 
        return OK;
    }

//PPP is  alive , we neet  to close it .
    pid = vfork();
    if(pid < 0) /* vfork Error  */
    {
        //(LOG_CRIT, "Vfork error!\n");
        return ERR;
    }
    else if (0 == pid) 
    {        
		//(LOG_DEBG, "Child process stop pppd now.\n", pid);
		execv( pacArgv [0]  ,  pacArgv ) ;
 //       msleep(10000);  
        exit(0);
    }
    else
    {
        wait(NULL); //Wait child exit

        ucRet = check_ppp_alive( gprs );
        msleep(4000); /* Make sure pppd has exit. */
        if(0x00 != ucRet )
        {
            //(LOG_DEBG, "Child process stop pppd successfully.\n");        
            return OK;
        }
        else
        {
            //(LOG_CRIT, "Child proecss stop pppd failure, return code: 0x%0x.\n", ucRet); 
            return ERR;
        }
    }

    return FAILURE;


}


void  *PPP_Thread( void  * arg)
{
	GPRS_ATTR   *gprs =  ( GPRS_ATTR * )arg ;
	int	        ucRet = 0 , 
                iStatusFlag1 = 0 , 
                iStatusFlag2 = 0 ;
	unsigned long	
//		ulCurTime1 = 0,   
//			ulCurTime2 = 0, 
//                    ulCurTime3 = 0, 
//                    ulStartTime1 = 0,
                    ulStartTime2 = 0, 
                    ulStartTime3 = 0;
	SIM		    *pSim = NULL;
	MOD_PPP_STAT    stPPPStat;

//	printf("come into PPP pthread now !\n");
	nc_log (&logger, NC_MOBILED_NAME, NA_LOG_WARNING, "@%04d Start PPP thread now!", __LINE__);
   	
while(na_proc_token (0, 1))
{ 
	if(PPP_START == g_ucWorkMode)		// PPP == g_ucWorkMode 
	{	
	//	//(LOG_INFO, "The main thread  ask  PPP  to work .  \n");
		if( SYS_IDLE == g_stGprsStatus.ucGprsStatus )// PPP = g_ucWorkMode  && GPRS  is idle , we will use it ,and lock it .
		{
			//(LOG_INFO, "The main thread ask PPP to work .PPP is  IDLE  now , and  go  to  busy  from now on  . \n");
			nc_mobiled_set_state( PPP_BUSY ); //  lock  gprs
		}
		else if( PPP_BUSY == g_stGprsStatus.ucGprsStatus || PPP_IDLE == g_stGprsStatus.ucGprsStatus)  // PPP = g_ucWorkMode && GPRS is used  by ourself	
		{	

			check_gprs_sim_door( gprs );		// check  the sim_door_1  sim_door_2  status  always.
			ucRet = check_ppp_alive( gprs );
			if( OK != ucRet  )  //  ppp  is  not OK
			{		
				//(LOG_DEBG, "PPP is  not  OK ! \n");
				if( PPP_PING_ERR == ucRet )
				{
					//(LOG_DEBG, " PPP  ping is  not  alive .  So we  term PPP  and switch   SIM now . \n");
					ppp_term( gprs ) ;
				}
SELECT_SIM:	ucRet = select_sim(gprs) ;     // no sim_door  is closed  ; or ,  when we  change  from sim2  to  sim1 ,but  sim1 .
				if( FAILURE == ucRet )      
				{		
					//(LOG_DEBG, " There is no sim can be used  Now ! \n");
					goto  SLEEP;
				}	
				else  /* if select_sim return OK, we will  select_apn   */
				{
SELECT_APN:			ucRet = select_apn( gprs);	  
					if( SUCCESS == ucRet)     /* select_apn  return SUCCESS ,we will  start ppp */                
					{
						//(LOG_DEBG, "Gprs select APN success . \n");
						ppp_start(gprs);
						ucRet = check_ppp_alive(gprs );
						if( OK == ucRet )	//If PPP is OK, record  curSim'S apn / user / password 
						{	
							//(LOG_DEBG, "PPP start  OK . \n");
						 //	pSim( gprs->curSimNo == SIM_1 )? ( pSim = &(gprs->sim1) ): (pSim = &(gprs->sim2) );
                        
						pSim = ( gprs->curSimNo == SIM_1 )? ( &(gprs->sim1) ):(&(gprs->sim2) );
							//(LOG_DEBG, " Gprs  current  SIM  is %d \n" ,  gprs->curSimNo);
							switch(pSim->apnFlag)
							{
								case( APN_GPRS_CONF ):
									strncpy(pSim->apnHistory,  pSim->apnGprsConf, APN_LEN);
									strncpy(pSim->userHistory,  pSim->userGprsConf, USER_LEN );
									strncpy(pSim->passwordHistory,  pSim->passwordGprsConf, PASSWD_LEN);
									pSim->historyApnFlag = APN_GPRS_CONF;
									break;
								case( APN_APNS_CONF  ):
									strncpy(pSim->apnHistory, pSim->apnApnsConf, APN_LEN); 
									strncpy(pSim->userHistory, pSim->userApnsConf, USER_LEN);
									strncpy(pSim->passwordHistory, pSim->passwordApnsConf, PASSWD_LEN);
									pSim->historyApnFlag = APN_APNS_CONF;
									break;
								default:
									break;
							}
							//(LOG_INFO, "After select APN , Gprs  current  SIM %d apnHistory = %s  ;\n", gprs->curSimNo, pSim->apnHistory);
							//(LOG_INFO, "After select APN , Gprs  current  SIM %d userHistory = %s  ;\n", gprs->curSimNo,  pSim->userHistory);
							//(LOG_INFO, "After select APN , Gprs  current  SIM %d passwordHistory = %s ; \n", gprs->curSimNo, pSim->passwordHistory);
							pSim->apnFlag = APN_NULL;
							goto  SLEEP;
						}	
						else	// PPP is not  OK , we  term PPP now  and  reselect apn for a try 
						{
							//(LOG_DEBG, "PPP  start failure . \n");
							ppp_term( gprs );
							//(LOG_DEBG, "Term PPP NOW .\n");
							goto  SELECT_APN;	
						}	
					}
					else      /* select_apn  return FAILURE ,we will  try other  sim again*/
					{
						//(LOG_DEBG, "Gprs select  APN  failure . Now go to try switch another SIM . \n");
						if( gprs->curSimNo== SIM_1 )  // select_apn failure , if  cur_sim  is  SIM1 , we  set  SIM_1_FAILURE  flag 1 .
						{
							gprs->sim_1_Failure = 1;
						}
						goto  SELECT_SIM;
					}
				}	
			}	
			else    /* PPP is OK, check PPP is IDLE, and  if  now  use SIM2, we  need switch to SIM1 if  the time arrived FallBackTime */
			{
				//(LOG_DEBG, "PPP is  OK  . \n");

                // Check PPP is IDLE or  BUSY 
                if( 0 == iStatusFlag1 )
				{
				 	ulStartTime2 = na_time_now( );
      					memset(&stPPPStat, 0 , sizeof( stPPPStat ));
					check_ppp_stat(  &stPPPStat ) ;
					iStatusFlag1 = 1;
				}

                if(  ( na_time_elapsed(ulStartTime2) ) > gprs->idleTime)     //  checK  PPP idle time 
                {
                    ucRet = check_ppp_idle(&stPPPStat ,&gprs->receiveBytesThreshold, &gprs->sendBytesThreshold);
    				if( -1 == ucRet )
                    {        
	            		goto SLEEP;
                    }    
				    else    if( PPP_BUSY == ucRet)		
				    {
                        //(LOG_DEBG, "PPP is  BUSY Now. \n");
                        iStatusFlag1 = 0;
                        g_stGprsStatus.ucGprsStatus = PPP_BUSY;
                    }
                    else    if( PPP_IDLE == ucRet)
                    {
                        //(LOG_DEBG, "PPP is  IDLE Now. \n");
                        iStatusFlag1 = 0;
                        g_stGprsStatus.ucGprsStatus = PPP_IDLE;
                    }
                }        

                /* CHec  SIM,  if sim2 is used, switch to SIM1  */
				if( gprs->curSimNo == SIM_2 && gprs->simDoor1.simDoorState == 1  )  // PPP is OK .current sim is sim2 , fallbacktime  if set  in xml file.
				{	
					if( iStatusFlag2 == 0 )
					{
		//				GET_TICKCOUNT(ulCurTime3); 
						ulStartTime3 = na_time_now( );
			//			ulStartTime3 = ulCurTime3;
						iStatusFlag2 = 1;
					}	
			//		GET_TICKCOUNT(ulCurTime3); 
					if ( ( na_time_elapsed(ulStartTime3)) < (gprs->sim2.fallBackTime) )
					{
					    //(LOG_DEBG, "PPP is OK. And current Sim is SIM2. But time isn't arrive  fallbaketime .\n");
                    	goto SLEEP;
					}
					else
					{
	            		//(LOG_DEBG, "PPP is OK. And current Sim is SIM2 .  And time is arrive fallbacktime , try to switch to SIM1  now .\n");
                        iStatusFlag2 =0;
					    ppp_term(gprs);
                        goto SELECT_SIM;
					}
				}				
				else 	if( gprs->curSimNo == SIM_1 )// PPP is OK . current sim is SIM_1  , goto sleep
				{
					//(LOG_DEBG, "PPP is  OK.  And  current  Sim is SIM_1. \n");
					goto SLEEP;
				}
			}
		}			
		else	 // PPP = g_ucWorkMode && GPRS is used  by other  thread ,wait .
		{	
			//(LOG_DEBG, "Main thread ask PPP to start , But GPRS is used by other thread NOW. \n");
			goto SLEEP;         
		}
		
	}	/* if(PPP == g_ucWorkMode) */
	else if (PPP_START != g_ucWorkMode) 
	{
		if( PPP_IDLE == g_stGprsStatus.ucGprsStatus)	//PPP != g_ucWorkMode , PPP == g_ucHold.ucHold	
		{
			//(LOG_DEBG, "Main thread ask  PPP  to stop, and PPP  is idle  NOW . \n");
			ppp_term( gprs ) ;			
			//(LOG_DEBG, "PPP is termed . \n");
			nc_mobiled_set_state( SYS_IDLE );   //unlock GPRS
		}
		else
		{
			//(LOG_DEBG, "Main thread ask PPP to stop , but PPP is busy NOW . \n");
			goto SLEEP;
		}
	}

SLEEP:
	sleep(10);  //  sleep  _   ms  ,then  continure loop 

} 

//CleanUp:
//    printf("Stop pppd dial up now.\n");
    nc_log (&logger, NC_MOBILED_NAME, NA_LOG_WARNING, "@%04d Stop pppd dial up now!", __LINE__);
    ppp_term( gprs );
	
    pthread_exit((void *)1);

  //  return 0;

}

