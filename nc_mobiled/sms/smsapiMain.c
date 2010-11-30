
/**************************************************************************************
 *
 *    FileName:  smsapi_main.c
 * Description:  The SMS_API programe entry point, main() function defined here.
 *
 *      Author:  zengxiangru  (zengxiangru@ghlsystems.com)
 *   CopyRight:  GHL Systems Berhad @ 2009
 *
 *   ChangeLog: 
 *               Release Draft on 25 June. 2010 by zengxiangru
 *
 *
 *************************************************************************************/


#include <common/common.h>



int                 domainSockfd = 0;
int 		    g_iMsgNum = 0;

struct APILIST
{
	GW_API_PARAM Api2Gw;
	struct APILIST *previous;
	struct APILIST *next;
};

COM_PORT   *sms_comport;

struct APILIST *recv_list_head = NULL;  /*SMS thread receive message from gprs module and store it*/
struct APILIST *send_list_head = NULL; /*SMS thread store the messages from clients which need send*/

int createlist(struct APILIST *head)
{
	struct APILIST *p1;

	p1 = (struct APILIST *)malloc(sizeof(struct APILIST));
	if(!p1)
	{
	//	logs(LOG_ERRO, "Alloc memory failure.\n");
  		nc_log (&logger, NC_MOBILED_NAME, NA_LOG_ERROR, "@%04d Mobiled SMS thread alloc memory failure!", __LINE__);
		return -1;
	}
	
	head = p1;
	p1->previous = NULL;
	p1->next = NULL;

	return 0;
}

int adddata2list(struct APILIST *head, struct APILIST *pSrc)
{
	struct APILIST *p1,*p2;

	p1 = (struct APILIST *)malloc(sizeof(struct APILIST));
	if(p1 == NULL)
	{
  		nc_log (&logger, NC_MOBILED_NAME, NA_LOG_ERROR, "@%04d Mobiled SMS thread alloc memory failure!", __LINE__);
		return -1;
	}
	memcpy(&(p1->Api2Gw),&(pSrc->Api2Gw),sizeof(GW_API_PARAM));
	p2 = head->next;

	while(1)
	{
  		nc_log (&logger, NC_MOBILED_NAME, NA_LOG_DETAIL, "@%04d Mobiled SMS thread:before add,head next value is:%p", __LINE__, head->next);
		if(head->next != NULL)
		{
			p1 ->next = head->next;
			head->next = p1;
			p2->previous = p1;
			
			break;
		}
		else
		{
			p1->previous = NULL;
			p1->next = NULL;
			head->next = p1;

			break;
		}
	}
//	logs(LOG_INFO, "after add Head next value is:%p\n",head->next);
  	nc_log (&logger, NC_MOBILED_NAME, NA_LOG_DETAIL, "@%04d Mobiled SMS thread:after add,head next value is:%p", __LINE__, head->next);
	g_iMsgNum++;
//	logs(LOG_INFO, "iMsgNum is :%d\n",g_iMsgNum);
  	nc_log (&logger, NC_MOBILED_NAME, NA_LOG_DETAIL, "@%04d Mobiled SMS thread:iMsgNum is:%d!", __LINE__, g_iMsgNum);
	return 0;
}

int getlistvalue(struct APILIST *head, GW_API_PARAM *pmsg)
{
	struct APILIST *p1;
	p1 = head;

	while(1)
	{
		if(p1->next != NULL)
		{
			p1 = p1->next;
			if(NULL!=p1->next)
				continue;
			else
			{
				while((((p1->Api2Gw.ack_nak.uiStatus >>16) %2) == 1 ) && (p1->previous !=NULL))
				{
					p1 = p1->previous;
				}

				if(((p1->Api2Gw.ack_nak.uiStatus >> 16) %2) ==1)
					return -1; 
				else
				{
					memcpy(pmsg,&(p1->Api2Gw),sizeof(GW_API_PARAM));
				//	logs(LOG_INFO,"Process indexGw is:%d\n",p1->Api2Gw.ack_nak.indexGW);
					p1->Api2Gw.ack_nak.uiStatus |= APIHANDLED;
					break;
				}
			}
		}
		else
		{
		//	logs(LOG_INFO, "Now No message to send!\n");
  			nc_log (&logger, NC_MOBILED_NAME, NA_LOG_DETAIL, "@%04d Mobiled SMS thread:Now no message to send!", __LINE__);
			return -1;
		}
	}
	
	return 0;
}

int IsTimeOut(time_t ComeInTime)
{
	time_t  CurrentTime;

	CurrentTime = time(NULL);

	if((CurrentTime - ComeInTime) > TimeOut)
	{
	//	logs(LOG_INFO, "This is timeout!\n");
  		nc_log (&logger, NC_MOBILED_NAME, NA_LOG_DETAIL, "@%04d Mobiled SMS thread: This is timeout!", __LINE__);
		return 0;
	}
	else
		return 1;
}

int deletedata(struct APILIST *head, int index)
{
	struct APILIST *p1,*p2;
	p1 = head;

	while(1)
	{	
		if(p1->next != NULL)
		{
			p2 = p1;
			p1 = p1->next;
			if(p1->Api2Gw.ack_nak.index == index) 
			{
				if(p1->next != NULL)
				{
					p2->next = p1->next;
					p1->next->previous = p1->previous;
					free(p1);
					g_iMsgNum--;
					return 0;
				}
				else
				{
					p2->next = NULL;
					free(p1);
					g_iMsgNum--;
					return 0;
				}
			}
			else 
			{
				if(p1->next != NULL)
					continue;
				else
				{
			//		logs(LOG_INFO,"Don't find the node!\n");
  					nc_log (&logger, NC_MOBILED_NAME, NA_LOG_DETAIL, "@%04d Mobiled SMS thread:Don't find the node!", __LINE__);
					return -1;
				}
			}
		}
		else
		{
		//	logs(LOG_INFO, "Don't find the node!\n");
  			nc_log (&logger, NC_MOBILED_NAME, NA_LOG_DETAIL, "@%04d Mobiled SMS thread:Don't find the node!", __LINE__);
			return -1;
		}
	}
//	logs(LOG_INFO,"iMsgNum is:%d\n",g_iMsgNum);
	
	return 0;
}

struct APILIST * findinlist(struct APILIST *head, int index)
{
	struct APILIST *p1;
	p1 = head;

	while(1)
	{	
		if(p1->next != NULL)
		{
			p1 = p1->next;
			if(p1->Api2Gw.ack_nak.index == index) 
			{
				return p1;
			}
			else 
			{
				if(p1->next != NULL)
					continue;
				else
				{
				//	logs(LOG_INFO,"Don't find the node!\n");
  					nc_log (&logger, NC_MOBILED_NAME, NA_LOG_DETAIL, "@%04d Mobiled SMS thread:Don't find the node!", __LINE__);
					return NULL;
				}
			}
		}
		else
		{
		//	logs(LOG_INFO,"Don't find the node!\n");
  			nc_log (&logger, NC_MOBILED_NAME, NA_LOG_DETAIL, "@%04d Mobiled SMS thread:Don't find the node!", __LINE__);
			return NULL;
		}
	}
	
	return NULL;
}


/***********************************************************************
 *Description : continually query the list,if there is a timeout msg,so
                delete it and write some information to log
 *Parameter   : void
 *  
 *********************************************************************/
void querylist(struct APILIST *head)
{
	struct APILIST *p1;
	int iRet = 0;
	p1 = head;

	while(TRUE)
	{
//		p1 = head;
		if(p1->next != NULL)
		{
			p1 = p1->next;
		//	if(((p1->Api2Gw.ack_nak.uiStatus >> 16) %2) == 1)
		//	{
				iRet = IsTimeOut(p1->Api2Gw.ack_nak.ComeInTime);
				if(iRet == 0)
				{
				//	logs(LOG_INFO, "This message is timeout!\n");  //write to log
  					nc_log (&logger, NC_MOBILED_NAME, NA_LOG_DETAIL, "@%04d Mobiled SMS thread:This message is timeout!", __LINE__);
				//	write2log(p1->Api2Gw.ack_nak.index);
					deletedata(head, p1->Api2Gw.ack_nak.index);

					if(head->next != NULL)
					{
						p1 = head;
						continue;
					}
					else
						break;
				}
				else
					continue;
		//	}
		//	else
		//		continue;
		}
		else
		{
		//	logs(LOG_INFO, "The list is empty,Don't need query!\n");
			break;
		}
	}
}


int receive_long_msg(GW_API_PARAM   *pstApiGwInfo, SM_PARAM *pstMsgStruct, int *piNum, COM_PORT   *sms_comport)
{
	int iRet = 0;
	while(TRUE)
	{
		(*piNum)++;
	//	logs(LOG_INFO, "Long message number is:%d\n",pk_rcv_total);
  		nc_log (&logger, NC_MOBILED_NAME, NA_LOG_DETAIL, "@%04d Mobiled SMS thread:Long message number is:%d!", __LINE__, pk_rcv_total);
											
		iRet = gsmRecvSMS(pstMsgStruct, *piNum, sms_comport);
		if(iRet > 0 && pk_rcv_total > 0 )
		{
			strcat((char *)(pstApiGwInfo->msgstruct.TP_UD), (char *)(pstMsgStruct->TP_UD));
			gsmDeleteSMS(*piNum, sms_comport);
			if(pk_rcv_num < pk_rcv_total)
					continue;
			else
			{
				pk_rcv_num = 0;
				pk_rcv_total = 0;
				iRet = 0;
				goto CleanUp;
			}
		}
		else if(iRet < 0) 
		{
			iRet = -1;
			goto CleanUp;
		}
		else
		{
			iRet = -2;
			goto CleanUp;
		}
	}
CleanUp:
	return iRet;

}

int receive_msg(COM_PORT   *sms_comport, GW_API_PARAM *pApiGwInfo)
{
	int		num = 0, 
			iRetVal = 0,
			ilength,
			iRet;
	static int	recv_index = 1;

	GW_API_PARAM    stApiGwInfo;
	SM_PARAM        stMsgStruct;
	struct APILIST  stApiList;

	ilength = sizeof(stApiGwInfo);
	
	memset(&stApiGwInfo, 0, ilength);

	num = 1;
	iRetVal = gsmRecvSMS(&(stApiGwInfo.msgstruct), num, sms_comport);
	if(iRetVal < 0)
	{
  		nc_log (&logger, NC_MOBILED_NAME, NA_LOG_DETAIL, "@%04d Mobiled SMS thread: Send AT command failure!", __LINE__);
		
	}
	else if(iRetVal > 0)
	{
		gsmDeleteSMS(num, sms_comport);

		if(pk_rcv_total > 0)
		{
		//	logs(LOG_INFO, "This message is a long message!Continue receiving message !\n");
  			nc_log (&logger, NC_MOBILED_NAME, NA_LOG_DETAIL, "@%04d Mobiled SMS thread:This is a long message!Now continue receiving message!", __LINE__);
			
			memset(&stMsgStruct,0,sizeof(stMsgStruct));
			iRetVal = receive_long_msg(&stApiGwInfo, &stMsgStruct, &num, sms_comport);
			if(iRetVal < 0)
			{
				iRetVal = -1;
				goto CleanUp;
			}
		}
		
		memset(&stApiList, 0, sizeof(stApiList));
		memcpy(&(stApiList.Api2Gw.msgstruct),&(stApiGwInfo.msgstruct),sizeof(SM_PARAM));
		memcpy(&(stApiList.Api2Gw.ack_nak),&(pApiGwInfo->ack_nak),sizeof(NAK_PARAM));
		stApiList.Api2Gw.ack_nak.index = recv_index;
		recv_index++;
	//	logs(LOG_INFO, "Before add to list index value is:%d\n",stApiList.Api2Gw.ack_nak.index);
  		nc_log (&logger, NC_MOBILED_NAME, NA_LOG_DETAIL, "@%04d Mobiled SMS thread:Before add to list index value is:%d!", __LINE__, stApiList.Api2Gw.ack_nak.index);

		stApiList.Api2Gw.ack_nak.ComeInTime = time(NULL);
	//	logs(LOG_INFO, "Come in time is:%d\n",stApiList.Api2Gw.ack_nak.ComeInTime);
  		nc_log (&logger, NC_MOBILED_NAME, NA_LOG_DETAIL, "@%04d Mobiled SMS thread:Come in time is:%d!", __LINE__, stApiList.Api2Gw.ack_nak.ComeInTime);
		
		if(recv_list_head == NULL)
		{
			if(createlist(recv_list_head))
			{
  				nc_log (&logger, NC_MOBILED_NAME, NA_LOG_WARNING, "@%04d Mobiled SMS thread:create list for recving failure!", __LINE__);
				return -1;
			}
		}

		adddata2list(recv_list_head, &stApiList);

		while(TRUE)
		{
			num++;
			if(recv_index >= 200)
				recv_index = 1;
			memset(&stApiGwInfo,0,ilength);
			memset(&stApiList,0,sizeof(stApiList));
			iRet = gsmRecvSMS(&(stApiGwInfo.msgstruct),num, sms_comport);
			if((g_iMsgNum < TotalMsgNum) && (iRet > 0))
			{
				gsmDeleteSMS(num, sms_comport);

				if(pk_rcv_total > 0)
				{
				//	logs(LOG_INFO, "This message is a long message!Go on receiving message !\n");
  					nc_log (&logger, NC_MOBILED_NAME, NA_LOG_DETAIL, "@%04d Mobiled SMS thread:This message is a long message!Continue receiving message!", __LINE__);
			
					memset(&stMsgStruct,0,sizeof(stMsgStruct));
					iRetVal = receive_long_msg(&stApiGwInfo, &stMsgStruct, &num, sms_comport);
					if(iRetVal < 0)
					{
						iRetVal = -1;
						goto CleanUp;
					}
				}

				memcpy(&(stApiList.Api2Gw.msgstruct), &(stApiGwInfo.msgstruct), sizeof(SM_PARAM));
				stApiList.Api2Gw.ack_nak.ComeInTime = time(NULL);
			//	logs(LOG_INFO, "Come in time is:%d\n",stApiList.Api2Gw.ack_nak.ComeInTime);
  				nc_log (&logger, NC_MOBILED_NAME, NA_LOG_DETAIL, "@%04d Mobiled SMS thread:Come in time is:%d!", __LINE__, stApiList.Api2Gw.ack_nak.ComeInTime);
				
				stApiList.Api2Gw.ack_nak.index = recv_index;
				recv_index++;

												
				if(recv_list_head == NULL)
				{
					if(createlist(recv_list_head))
					{
						nc_log (&logger, NC_MOBILED_NAME, NA_LOG_WARNING, "@%04d Mobiled SMS thread:create list for recving failure!", __LINE__);
						return -1;
					}
				}

				adddata2list(recv_list_head, &stApiList);

			}
			else if(iRet < 0)
			{
				iRetVal = -1;
				goto CleanUp;
			}
		 }/*while (TURE)*/
	
		}/*else if(iRetVal > 0)*/
CleanUp:
	return iRetVal;
}

int SHMProcessCommonChannelRequest(int *pdwClientID)
{
	socklen_t clnt_len;
	int new_sock;
	struct sockaddr_un clnt_addr;

	clnt_len = sizeof(clnt_addr);

	if ((new_sock = accept(domainSockfd, (struct sockaddr *) &clnt_addr,&clnt_len)) < 0)
	{
	//	logs(LOG_ERRO, "Can't accept:%s\n",strerror(errno));
		return -1;
	}

	*pdwClientID = new_sock;
//	logs(LOG_INFO, "NEW_SOCK is:%d\n",new_sock);

	return 0;
}

int SHMProcessEventsServer(int *pdwClientID)
{
	fd_set read_fd;
	int selret;

	FD_ZERO(&read_fd);

	FD_SET(domainSockfd, &read_fd);

	selret = select(domainSockfd + 1, &read_fd, (fd_set *) NULL,(fd_set *) NULL,NULL);
	
	if(selret<0)
	{
	//	logs(LOG_ERRO, "Select returns with failure:%s!\n",strerror(errno));
		return -1;
	}
	if(selret==0)
	{
		return 2;
	}

	if (FD_ISSET(domainSockfd, &read_fd))
	{
	//	logs(LOG_INFO, "Common chanel packet arrival\n");
		if (SHMProcessCommonChannelRequest(pdwClientID) == -1)
		{
	//		logs(LOG_ERRO, "error in SHMProcessCommonChannelRequest: %d\n", *pdwClientID);
			return -1;
		}
		else
		{
	//		logs(LOG_INFO, "SHMProcessCommonChannelRequest detects: %d\n", *pdwClientID);
			return 0;
		}
	}
	return -1;
}

int SHMProcessEventsContext(int dwClientID)
{
	fd_set read_fd,write_fd;
	int iRet;

#ifdef DO_TIMEOUT
	struct timeval tv;
	
	tv.tv_sec = 2;
	tv.tv_usec = 0;
#endif

	FD_ZERO(&read_fd);
	FD_ZERO(&write_fd);
	
	FD_SET(dwClientID, &read_fd);
	FD_SET(dwClientID, &write_fd);

	iRet = select(dwClientID + 1, &read_fd,&write_fd,NULL,
#ifdef DO_TIMEOUT	
			&tv
#else
			NULL
#endif
			);

	if(iRet<0)
	{
	//	logs(LOG_ERRO, "select returns with failure: %s",strerror(errno));
  		nc_log (&logger, NC_MOBILED_NAME, NA_LOG_DETAIL, "@%04d Mobiled SMS thread:select returns with failure: %s!", __LINE__, strerror(errno));
		return -1;
	}

	if (iRet == 0)
	{
		/*timeout*/
		return 0;
	}
#if 0
	if(iRet == 2)
	{
		return 3;
	}
#endif
	if(FD_ISSET(dwClientID, &read_fd))
	{
		return 1;                        /*now can read and return*/
	}
	if(FD_ISSET(dwClientID, &write_fd))
	{
		return 2;			/*now can write and return*/
	}
	return -1;
}

void nc_mobiled_sms_term (NC_MOBILED_SMS *ctx)
{
  if (!ctx ||
      (NA_MAGIC == ctx->magic && !ctx->init))
    return;

 #if 0
  if (NA_MAGIC == ctx->magic && ctx->init) {
    NA_MEM_CLEAN (ctx->module);
  }
  #endif

  memset (ctx, 0, sizeof (NC_MOBILED_SMS));
  ctx->magic = NA_MAGIC;
}

int nc_mobiled_sms_init (NC_MOBILED_SMS *ctx)
{
  nc_mobiled_sms_term (ctx);
  ctx->init = 1;

  if (na_sock_init (&ctx->server.sock, NA_SOCK_TYPE_TCP, 0, 0, 60, 2))
    return -1;
  
  if (na_array_init (&ctx->server.clients, NC_MOBILE_MAX_CLIENT, NA_ARRAY_INDEX, NA_ARRAY_FLAG_CHK_DUP | NA_ARRAY_FLAG_NO_LOCK, 0))
    return -2;

  return 0;
}

int nc_mobiled_sms_resp (NC_MOBILED_SMS *ctx, NC_MOBILED_CLIENT *client)
{
  int rv, iRet;
  static int send_index = 1; 

  GW_API_PARAM stApiGwInfo;
  NC_MOBILE_RESULT stResult ;
  NC_MOBILE_DATA *pstDat ;
  NC_MOBILE_CONTENT_SMS  *pstSMS, stSMS;
  struct APILIST  stApiList;
 
  memset(&stResult, 0, sizeof(NC_MOBILE_RESULT));

  printf ("SMS serving%s:%d client connection... \n", client->sock.raddr, client->sock.rport);

 pstDat = (NC_MOBILE_DATA *)( client->revent.data ) ;
 nc_log (&logger, NC_MOBILED_NAME, NA_LOG_NORMAL, "@%04d Mobiled SMS: SMS server receive CMD = %u!", __LINE__, pstDat->iCmd);
  
 pstSMS = ( NC_MOBILE_CONTENT_SMS *)( pstDat->ucContent);

 printf("Server received TelNum = %s\n", pstSMS->acTelNum);
 printf("Server received SmsMsg = %s\n", pstSMS->acMsg);

 
#if 0
  // Frame  ops  to judge what Server should to do 
  if (NC_MOBILE_OPS_REQ != client->revent.frame.ops) {
    printf ("ops not valid (%d)\n", client->revent.frame.ops);
    return -1;
  }
#endif

  switch(pstDat->iCmd)
  {
	  case MOBILED_SMS_SEND:
		  memset(&stApiGwInfo, 0, sizeof(stApiGwInfo));
		  strncpy(stApiGwInfo.msgstruct.TPA, pstSMS->acTelNum, sizeof(pstSMS->acTelNum)-1);
		  strncpy((char *)(stApiGwInfo.msgstruct.TP_UD), pstSMS->acMsg, sizeof(pstSMS->acMsg)-1);
		  stApiGwInfo.msgstruct.TP_TYPE = TP_VPF | TP_MTI;
		  stApiGwInfo.msgstruct.TP_DCS = GSM_UCS2;
		  
		  if(g_ucWorkMode == PPP_BUSY)
		  {
			  if(send_list_head == NULL)
			  {
		  		nc_log (&logger, NC_MOBILED_NAME, NA_LOG_NORMAL, "@%04d Mobiled SMS: Send list is empty,now create it!", __LINE__);
				createlist(send_list_head);
				  
			  }
			  memset(&stApiList, 0, sizeof(stApiList));
			  memcpy(&(stApiList.Api2Gw.msgstruct),&(stApiGwInfo.msgstruct),sizeof(SM_PARAM));
//			  memcpy(&(stApiList.Api2Gw.ack_nak),&(pApiGwInfo->ack_nak),sizeof(NAK_PARAM));
			
			  stApiList.Api2Gw.ack_nak.index = send_index;
			  send_index++;
			  if(send_index >= 500)
				  send_index = 0;
			  stApiList.Api2Gw.ack_nak.ComeInTime = time(NULL);

			  if(adddata2list(send_list_head, &stApiList))
			  {
		  		nc_log (&logger, NC_MOBILED_NAME, NA_LOG_WARNING, "@%04d Mobiled SMS: add new messge to list failure!!", __LINE__);
				stResult.iResult = 2;
				strcpy(( char *)(stResult.ucContent), "The message has been processed failure, please send it again!");
				break;
			  }
			  stResult.iResult = 2;
		          strcpy(( char *)(stResult.ucContent), "The message has been stored,it will be send later!");
			  break;
			  /*store the message*/
		  }
		  else if(g_ucWorkMode == PPP_IDLE)
		  {
		  }
		  else
		  {
			iRet = gsmSendSMS(&(stApiGwInfo.msgstruct), sms_comport);     /*send msg*/
			if(iRet == 0)
			{
				stResult.iResult = 0;
				break;
			}
		  }
		  break;
	  case MOBILED_SMS_QURRY:
		  if(recv_list_head != NULL)
		  {
		  	memset(&stApiGwInfo, 0, sizeof(stApiGwInfo));
			iRet = getlistvalue(recv_list_head, &stApiGwInfo);
			if(iRet == 0)
			{
				memset(&stSMS, 0, sizeof(stSMS));
				strncpy(stSMS.acTelNum, stApiGwInfo.msgstruct.TPA, sizeof(stSMS.acTelNum)-1);
				strncpy(stSMS.acMsg, (char *)(stApiGwInfo.msgstruct.TP_UD), sizeof(stSMS.acMsg)-1);
				memcpy(stResult.ucContent, &stSMS, sizeof(NC_MOBILE_CONTENT_SMS));
				break;
			}
			else
			{
				nc_log (&logger, NC_MOBILED_NAME, NA_LOG_NORMAL, "@%04d Mobiled SMS: There are no new message in SIM card!", __LINE__);
				stResult.iResult = 2;
				strcpy(( char *)(stResult.ucContent), "There are no new message!");
				break;
			}
		  }
		  else
		  {
		  	nc_log (&logger, NC_MOBILED_NAME, NA_LOG_NORMAL, "@%04d Mobiled SMS: There are no new message in SIM card!", __LINE__);
			stResult.iResult = 2;
			strcpy(( char *)(stResult.ucContent), "There are no new message!");
			break;
		  }
		  break;
	  default:
		  nc_log (&logger, NC_MOBILED_NAME, NA_LOG_NORMAL, "@%04d Mobiled SMS: Unknown operation!", __LINE__);
  		  stResult.iResult = 2;
  		  strcpy(( char *)(stResult.ucContent), "Unknown operation,please check your command!");
		  break;
  }

#if 0
  // prepare buf
  NC_MOBILE_RESULT stResult ;
  memset(&stResult, 0, sizeof(NC_MOBILE_RESULT));
  stResult.iResult = 1;
  strncpy(( char *)(stResult.ucContent), "I'm fine!", strlen("I'm fine!")+1);
  memcpy(client->sbuf, &stResult, sizeof(NC_MOBILE_RESULT));
  client->slen = sizeof(NC_MOBILE_RESULT)+1;
#endif
  
  memcpy(client->sbuf, &stResult, sizeof(NC_MOBILE_RESULT));
  client->slen = sizeof(NC_MOBILE_RESULT)+1;

  client->sevent.frame.dst_app = client->revent.frame.src_app;
  client->sevent.frame.src_app = NC_MOBILED_NAME;
  client->sevent.frame.ops = NC_MOBILE_OPS_RES;
  client->sevent.frame.flags = client->revent.frame.flags;
  client->sevent.frame.dat = client->sbuf;
  client->sevent.frame.dat_len = client->slen;

  client->sevent.frame.raw_frm = client->sbuf;
  client->sevent.frame.raw_end = client->sevent.frame.raw_frm + NC_MOBILE_SMS_MAX_CLIENT_BUF;
  client->sevent.frame.raw_len = client->slen;

  // prepare buffer and send out
  if ((rv = nc_mobile_frame_build (&client->sevent))) {
    printf ("build failure (%d)\n", rv);
    return -3;
  }

  if ((rv = na_sock_send (&client->sock, client->sevent.frame.raw_frm, client->sevent.frame.raw_len))) {
    printf ("send failure (%d)\n", rv);
    return -4;
  }

  printf ("OK (%u)\n", client->slen);
  return 0;

}

int nc_mobiled_sms_serve (NC_MOBILED_SMS *ctx, NC_MOBILED_CLIENT *client)
{
  int rv = 0;
  unsigned int used;
	  
  if (!(rv = nc_mobile_frame_parse (&client->revent, client->rbuf, client->rlen, &used))) {
 
    // process client request
    if (nc_mobiled_sms_resp (ctx, client))   
      return -1;
  }

  if (used) {
    client->lact = na_time_now ();
    client->rlen -= used;       // update client receive buf used total length 

    // CAUTION: after used frame then only can adjust buffer
    if (client->rlen)
      memmove (client->rbuf, client->rbuf + used, client->rlen);
  }

  return 0;
}

int nc_mobiled_sms_proc (NC_MOBILED_SMS *ctx)
{
  	 int rv;
	 unsigned int      rlen;
	 NA_ARRAY_ITEM     *citem;
	 NC_MOBILED_CLIENT *cur_client;

  // ===================
  // ** accept client **
  // ===================
	
  nc_log (&logger, NC_MOBILED_NAME, NA_LOG_WARNING, "@%04d Mobiled SMS server is waiting for clients!", __LINE__);

  if (!ctx->server.client) {
    NA_MEM_ALLOC (ctx->server.client, sizeof (NC_MOBILED_SMS_CLIENT), (NC_MOBILED_SMS_CLIENT *));
  }
  if (ctx->server.client) {
    if (!ctx->server.client->rbuf) {
      NA_MEM_ALLOC (ctx->server.client->rbuf, NC_MOBILE_SMS_MAX_CLIENT_BUF, (unsigned char *));
    }
    if (!ctx->server.client->sbuf) {
      NA_MEM_ALLOC (ctx->server.client->sbuf, NC_MOBILE_SMS_MAX_CLIENT_BUF, (unsigned char *));
    }
  }

    if (!ctx->server.client ||
      !ctx->server.client->rbuf ||
      !ctx->server.client->sbuf ||
      na_sock_listen (&ctx->server.sock, NC_MOBILE_SMS_SERVER_ADDR, NC_MOBILE_SMS_SERVER_PORT, 10) ||
      na_sock_accept (&ctx->server.sock, &ctx->server.client->sock, 1)) {
    na_sock_close (&ctx->server.sock, 0);
    return -1;
  }

  if (na_sock_connected (&ctx->server.client->sock)) {
    if ((rv = na_array_add (&ctx->server.clients, ctx->server.client, 0, 1, &citem))) {
      printf ("%s:%d client connection insertion failure, rv=%d\n", ctx->server.client->sock.raddr, ctx->server.client->sock.rport, rv);
      na_sock_close (&ctx->server.client->sock, 0);
    }
    else {
      printf ("%s:%d client is connected\n", ctx->server.client->sock.raddr, ctx->server.client->sock.rport);

      ctx->server.client = 0;
    }
  }

    // ==================
  // ** process data **
  // ==================
    citem = 0;
  while ((citem = na_array_get_next (&ctx->server.clients, citem))) {
    cur_client = (NC_MOBILED_CLIENT *) citem->data;
  
    if ((rv = na_sock_recv (&cur_client->sock, cur_client->rbuf + cur_client->rlen, NC_MOBILE_SMS_MAX_CLIENT_BUF - cur_client->rlen, &rlen, 0))) 
   {
      if (-5 == rv)
        printf ("%s:%d client connection closed\n", cur_client->sock.raddr, cur_client->sock.rport);
      else
        printf ("%s:%d client connection failure, rv=%d\n", cur_client->sock.raddr, cur_client->sock.rport, rv);

      na_sock_term (&cur_client->sock);

      NA_MEM_CLEAN (cur_client->rbuf);
      NA_MEM_CLEAN (cur_client->sbuf);
      na_array_del_by_index (&ctx->server.clients, citem->index, 0);
    }
    else {	
	 if (rlen) {
        cur_client->rlen += rlen;  // update receive total length
	 printf("Mobiled receive current client total length = %d \n", cur_client->rlen);
        cur_client->lact = na_time_now ();
      }
    }

    if (cur_client->rlen) {
      if ((rv = nc_mobiled_sms_serve (ctx, cur_client)) ||
          NC_MOBILE_TIMEOUT <= na_time_elapsed (cur_client->lact)) {
        printf ("%s:%d client connection processing failure, rv=%d\n", cur_client->sock.raddr, cur_client->sock.rport, rv);

        na_sock_term (&cur_client->sock);

        NA_MEM_CLEAN (cur_client->rbuf);
        NA_MEM_CLEAN (cur_client->sbuf);
        na_array_del_by_index (&ctx->server.clients, citem->index, 0);
      }
    }
  }
  return 0;
}

void * SMS_API_Thread(void *arg)
{
	NC_MOBILED_SMS  ctx;
	GW_API_PARAM   stApiGwInfo;
	unsigned long last = 0; 
	GPRS_ATTR *pstGprsAttr = NULL;
   	
	int 		  rv,
			  iRetVal;
	
	if (nc_mobiled_sms_init ( &ctx ) )
		return ((void *)-1);

	nc_log (&logger, NC_MOBILED_NAME, NA_LOG_WARNING, "@%04d Now come into SMS pthread!", __LINE__);

	pstGprsAttr = (GPRS_ATTR *)arg;   
	sms_comport = (COM_PORT *)(&(pstGprsAttr->data_port));

	while (na_proc_token (0, 1)) 
	{
		if(g_ucWorkMode == SMS_CTRL)
		{
			memset(&stApiGwInfo, 0, sizeof(stApiGwInfo));
			iRetVal = receive_msg(sms_comport, &stApiGwInfo);
			if(iRetVal < 0)
			{
			    nc_log (&logger, NC_MOBILED_NAME, NA_LOG_ERROR, "@%04d Mobiled SMS: Send AT command failure!", __LINE__);
			}

		}

	  if (last) {
	   if (5000 >= na_time_elapsed (last)) {
        na_time_sleep (10);
        continue;
      }
      last = 0;
    }

	if (0 > (rv = nc_mobiled_sms_proc (&ctx)))
    	{
	  last = na_time_now ();
    	} 
	else if (rv)
    	{
		na_time_sleep (10);
    	}    
	
	sleep(5);
	}
	
	nc_mobiled_sms_term (&ctx);
	return ((void *) 0);
}

