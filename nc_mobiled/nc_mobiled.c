/*******************************************************************************
* This file is licensed under the terms of your license agreement(s) with
* GHL covering this file. Redistribution, except as permitted by the terms of
* your license agreement(s) with GHL, is strictly prohibited.
********************************************************************************/

#include <common/common.h>

/* ==========================================================================================
 * Description: this function display command description
 *
 * Parameter  : void
 *
 * Return     : void
 * ==========================================================================================
 */
void nc_mobiled_usage()
{
	printf("%s usage:\n", NC_MOBILED_NAME);
	printf("\n");
	printf("    %s [options]\n", NC_MOBILED_NAME);
	printf("\n");
	printf("Options:\n");
	printf("\n");
	printf("-stay      stay foreground\n");
	printf("--help     print this message\n");
	printf("\n");
}

void nc_mobiled_set_state(unsigned char holder)
{
	pthread_mutex_lock(&g_stGprsStatus.f_lock);
	g_stGprsStatus.ucGprsStatus = holder;
	pthread_mutex_unlock(&g_stGprsStatus.f_lock);
}

int nc_mobiled_init(NC_MOBILED * ctx)
{
	nc_mobiled_term(ctx);

	ctx->init = 1;

	if (na_sock_init(&ctx->server.sock, NA_SOCK_TYPE_TCP, 0, 0, 60, 2))
		return -1;

	if (na_array_init
			(&ctx->server.clients, NC_MOBILED_MAX_CONF, NA_ARRAY_INDEX,
			 NA_ARRAY_FLAG_CHK_DUP | NA_ARRAY_FLAG_NO_LOCK, 0))
		return -2;
	
//	close_gprs( &( ctx->gprs ) );

	return 0;
}

/* ==========================================================================================
 * Description: this function is the main entry for the daemon, it will daemonize itself and
 *              process in background to serve client connection.
 *
 * Parameter  : [IN  ] argc - the number of argument
 *              [IN  ] argv - argument array
 * Return     : 0 - success
 *              else - failure
 * ==========================================================================================
 */

unsigned char g_ucWorkMode;			/*The flag which shows the CTRL module decides which pthread to work */
MutexGprsStatus g_stGprsStatus;

int nc_mobiled_main(int argc, char *argv[])
{
	int rv, i, daemonize = 1;
	int iRet = 0;
	unsigned long last = 0;				// default is houtai
	NC_MOBILED ctx;

	pthread_t thread_ppp_id;
//    pthread_t             thread_sms_id;
 //	pthread_t             thread_voice_id;
	pthread_attr_t thread_attr;


	// get options
	for (i = 1; i < argc; i++)
	{
		if (!strcmp(argv[i], "-stay"))
		{
			daemonize = 0;
		}
		else
		{
			nc_mobiled_usage();
			return 0;
		}
	}

	if (daemonize)
	{
		if (0 > (rv = na_proc_daemonize(0, NC_SELF_FILE, argv)))
			return -2;
		else if (!rv)
		{
			_exit(0);									// parent dont continue
		}
	}

	if (nc_mobiled_init(&ctx))
		return -1;

//config
	if ((rv = nc_mobiled_config(&ctx)))
	{
		nc_log(&logger, NC_MOBILED_NAME, NA_LOG_WARNING,
					 "@%04d Retrieve configuration failure, rv=%d", __LINE__, rv);
		rv = -1;
	}
	nc_log(&logger, NC_MOBILED_NAME, NA_LOG_WARNING,
				 "@%04d Retrieve configuration success", __LINE__);

	ctx.config.init = 1;					// proceed to setup proc structure

A:
	iRet = initial_gprs(&ctx.gprs);	/* Open the serial port and ctrl port. */
	if (OK != iRet)
	{
	nc_log(&logger, NC_MOBILED_NAME, NA_LOG_WARNING,
					 "@%04 Nc_mobiled init_gprs failure ! rv = %d " , iRet);
		msleep(5000);
		goto A;
	}
	nc_log(&logger, NC_MOBILED_NAME, NA_LOG_NORMAL,
					 "@%04 Nc_mobiled init_gprs success ! rv = %d ", iRet);
	

SELECT_SIM:
	iRet = select_sim(&ctx.gprs);	/* Select a  SIM to work */
	if (OK != iRet)
	{
		nc_log(&logger, NC_MOBILED_NAME, NA_LOG_WARNING,
					 "@%04 Nc_mobiled init_gprs failure ! rv = %d " , iRet);
		msleep(5000);

		printf("Select sim error!\n");
		goto SELECT_SIM;
	}
	//(LOG_INFO, "Gprs  select  sim success ! \n");

	printf("Select sim OK!\n");

	/*Init the mutex value */
	memset(&g_stGprsStatus, 0, sizeof(g_stGprsStatus));
	pthread_mutex_init(&(g_stGprsStatus.f_lock), NULL);

	//   g_ucWorkMode = IDLE;
	g_ucWorkMode = PPP_START;
	nc_mobiled_set_state(SYS_IDLE);

/*=======================================================================*/
	/* creaddte thread */
	/*Initialize the thread attribute */

	pthread_attr_init(&thread_attr);

	/*Set the stack size of the thread */
	pthread_attr_setstacksize(&thread_attr, 120 * 1024);

	/*Set thread to detached state:Don`t need pthread_join */
	pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);

	pthread_create(&thread_ppp_id, &thread_attr, PPP_Thread, (void *)&(ctx.gprs));

//    pthread_create(&thread_sms_id, &thread_attr, SMS_API_Thread, (void *)&(ctx.gprs));

//	pthread_create(&thread_sms_id, &thread_attr, VOICE_Thread, ( void * ) 0);

	while (na_proc_token(0, 1))
	{
	

		printf("Come in nc_mobiled_main() \n");
		if (last)
		{
			if (5000 >= na_time_elapsed(last))
			{
				na_time_sleep(10);
				continue;
			}
			last = 0;
		}

		if (0 > (rv = nc_mobiled_proc(&ctx)))
		{
			last = na_time_now();
		}
		else if (rv)
		{
			na_time_sleep(10);
		}

		get_gprs_poweron_uptime(&(ctx.gprs));
		printf("GPRS POWER_ON TIME is %d days, %d hours , %d minutes\n",   \
					ctx.gprs.gprsInfo.timer.timeLen.day, \
					ctx.gprs.gprsInfo.timer.timeLen.hour,  \
					ctx.gprs.gprsInfo.timer.timeLen.minute);

		if(  ( PPP_IDLE == g_stGprsStatus.ucGprsStatus) || ( PPP_BUSY == g_stGprsStatus.ucGprsStatus) )
		{	
			if( OK == check_ppp_stat( &( ctx.gprs.gprsInfo.netFlow )))
				printf("Gprs netFlow is Receive  %llu bytes, Send %llu bytes .\n ",  \
								ctx.gprs.gprsInfo.netFlow.ullRx_Bytes, \
								ctx.gprs.gprsInfo.netFlow.ullTx_Bytes);
		}	

		sleep(5);

	}


	pthread_mutex_destroy(&g_stGprsStatus.f_lock);

	int kill_rc = 0;

	do
    {
        kill_rc = pthread_kill(thread_ppp_id, 0);
        sleep(1);
    }while(ESRCH != kill_rc);
//    logs(LOG_DEBG, "PPP pthread exits now!\n");
	// cleanup:
		printf("Nc_mobiled clear !\n");
//	 ppp_term( &( ctx.gprs ) );

	 nc_mobiled_term(&ctx);

	return 0;

}

/* ==========================================================================================
 * Description: this function is the newsd structure destructor, everytime after use
 *              newsd structure must be destroy using this function.
 *              after that it will set the magic number to it. the magic number is use to
 *              indicate the structure has been properly initialized, so that any termination
 *              can be done correctly.
 *
 * Parameter  : [IN  ] ctx - structure context
 *
 * Return     : 0 - success
 *              else - failure
 * ==========================================================================================
 */
void nc_mobiled_term(NC_MOBILED * ctx)
{

	if (!ctx || (NA_MAGIC == ctx->magic && !ctx->init))
		return;

	if (NA_MAGIC == ctx->magic && ctx->init)
	{
		NA_MEM_CLEAN(ctx->module);
	}

	memset(ctx, 0, sizeof(NC_MOBILED));
	ctx->magic = NA_MAGIC;

	nc_config_term( &ctx->gprs.config.proxy);
	 na_array_term (&ctx->gprs.config.array_apns);
	 na_array_term(&ctx->gprs.config.array_sim);

	
}

#if 0
int nc_mobiled_good(NC_MOBILED * ctx)
{
	int rv;

	if (!ctx->config.init)
	{
		ctx->good = 0;

		if ((rv = nc_mobiled_config(ctx)))
		{
			nc_log(&logger, NC_MOBILED_NAME, NA_LOG_WARNING,
						 "@%04d Retrieve configuration failure, rv=%d", __LINE__, rv);
			rv = -1;
			goto cleanup;
		}

		nc_log(&logger, NC_MOBILED_NAME, NA_LOG_WARNING,
					 "@%04d Retrieve configuration successful", __LINE__);
		ctx->config.init = 1;				// proceed to setup proc structure
	}

	// setup proc structure
	if (1 == ctx->config.init)
	{
		ctx->proc.modem = ctx->config.modem;

		strcpy(ctx->proc.apn, ctx->config.apn);
		strcpy(ctx->proc.uid, ctx->config.uid);
		strcpy(ctx->proc.pwd, ctx->config.pwd);

		ctx->config.init = 2;				// proceed to setup module
	}

	// setup module
	if (2 == ctx->config.init)
	{
		NA_MEM_CLEAN(ctx->module);

		switch (ctx->proc.modem)
		{
		case NC_MOBILED_MODEM_GTM900B:
			NA_MEM_ALLOC(ctx->module, sizeof(NC_MOBILED_GTM900B),
									 (NC_MOBILED_GTM900B *));

			ctx->f_init = nc_mobiled_gtm900b_init;
			ctx->f_term = nc_mobiled_gtm900b_term;
			ctx->f_exec = nc_mobiled_gtm900b_exec;	// create a thread  that select sim , power up  

			ctx->f_data = nc_mobiled_gtm900b_ppp;	// create a thread  that start PPP 
			ctx->f_ussd = nc_mobiled_gtm900b_ussd;
			ctx->f_tel = nc_mobiled_gtm900b_voice;
			ctx->f_sms = nc_mobiled_gtm900b_sms;
			break;

		default:
			rv = -2;
			goto cleanup;
		}

		if (!ctx->module)
		{
			rv = -3;
			goto cleanup;
		}

		ctx->config.init = 3;				// finished
	}

	rv = 0;

cleanup:

	return rv;
}

#endif

int nc_mobiled_serve(NC_MOBILED * ctx, NC_MOBILED_CLIENT * client)
{
	int rv = 0;
	unsigned int used;
	// parse  rbuf, frame  header , src app, dest app, ...   ->into   client->revent   
	if (!
			(rv =
			 nc_mobile_frame_parse(&client->revent, client->rbuf, client->rlen,
														 &used)))
	{
		//   printf("Mobiled receive crrent client %s:%d  \n", client->revent.data, client->revent.clen);  
		// process client request
		if (nc_mobiled_resp(ctx, client))	// according  client->revent ,  according the path  which rbuf  give , find  config file and  in the file , 
			//and  find the app  section,  then copy response  content  to sbuf,  then  add frame header , then socket send .
			return -1;
	}

	if (used)
	{
		client->lact = na_time_now();
		client->rlen -= used;				// update client receive buf used total length 

		// CAUTION: after used frame then only can adjust buffer
		if (client->rlen)
			memmove(client->rbuf, client->rbuf + used, client->rlen);
	}

	return 0;
}

int nc_mobiled_resp(NC_MOBILED * ctx, NC_MOBILED_CLIENT * client)
{
	int rv;

#if 0
	NA_CONFIG *config = 0;
	NA_CONFIG_ITEM *config_item = 0;
	NA_ARRAY_ITEM *item;
#endif
	printf("Serving %s:%d client connection... \n", client->sock.raddr,
				 client->sock.rport);

	NC_MOBILE_DATA *pstDat;
	pstDat = (NC_MOBILE_DATA *) (client->revent.data);
	printf("Server receiverd CMD = %u\n", pstDat->iCmd);

	NC_MOBILE_CONTENT_SMS *pstSms;
	pstSms = (NC_MOBILE_CONTENT_SMS *) (pstDat->ucContent);

	printf("Server received TelNum = %s\n", pstSms->acTelNum);
	printf("Server received SmsMsg = %s\n", pstSms->acMsg);

	// manage module
	//switch( CMD )

	// Frame  ops  to judge what Server should to do 
	if (NC_MOBILE_OPS_REQ != client->revent.frame.ops)
	{
		printf("ops not valid (%d)\n", client->revent.frame.ops);
		return -1;
	}

	// prepare buf
	NC_MOBILE_RESULT result;
	memset(&result, 0, sizeof(NC_MOBILE_RESULT));
	result.iResult = 1;
	strncpy((char *)(result.ucContent), "I'm fine!", strlen("I'm fine!") + 1);
	memcpy(client->sbuf, &result, sizeof(NC_MOBILE_RESULT));
	client->slen = sizeof(NC_MOBILE_RESULT) + 1;
	//  strncpy((char *)client->sbuf , "I'm fine!", strlen("I'm fine!") );
	//  client->slen = strlen("I'm fine!") + 1;

#if 0
	if ((rv =
			 na_config_outp(config, config_item, (char *)client->sbuf,
											NC_CONFIG_MAX_CLIENT_BUF, &client->slen)))
	{
		printf("output failure (%d)\n", rv);
		return -2;
	}
#endif

	client->sevent.frame.dst_app = client->revent.frame.src_app;
	client->sevent.frame.src_app = NC_MOBILED_NAME;
	client->sevent.frame.ops = NC_MOBILE_OPS_RES;
	client->sevent.frame.flags = client->revent.frame.flags;
	client->sevent.frame.dat = client->sbuf;
	client->sevent.frame.dat_len = client->slen;

	client->sevent.frame.raw_frm = client->sbuf;
	client->sevent.frame.raw_end =
			client->sevent.frame.raw_frm + NC_MOBILE_MAX_CLIENT_BUF;
	client->sevent.frame.raw_len = client->slen;

	// prepare buffer and send out
	if ((rv = nc_mobile_frame_build(&client->sevent)))
	{
		printf("build failure (%d)\n", rv);
		return -3;
	}

	if ((rv =
			 na_sock_send(&client->sock, client->sevent.frame.raw_frm,
										client->sevent.frame.raw_len)))
	{
		printf("send failure (%d)\n", rv);
		return -4;
	}

	printf("OK (%u)\n", client->slen);
	return 0;

}

int nc_mobiled_proc(NC_MOBILED * ctx)
{

	int rv;
	unsigned int rlen;

	NA_ARRAY_ITEM *citem;

	NC_MOBILED_CLIENT *cur_client;

	// ===================
	// ** accept client **
	// ===================

	if (!ctx->server.client)
	{
		NA_MEM_ALLOC(ctx->server.client, sizeof(NC_MOBILED_CLIENT),
								 (NC_MOBILED_CLIENT *));
	}
	if (ctx->server.client)
	{
		if (!ctx->server.client->rbuf)
		{
			NA_MEM_ALLOC(ctx->server.client->rbuf, NC_MOBILE_MAX_CLIENT_BUF,
									 (unsigned char *));
		}
		if (!ctx->server.client->sbuf)
		{
			NA_MEM_ALLOC(ctx->server.client->sbuf, NC_MOBILE_MAX_CLIENT_BUF,
									 (unsigned char *));
		}
	}

	if (!ctx->server.client ||
			!ctx->server.client->rbuf ||
			!ctx->server.client->sbuf ||
			na_sock_listen(&ctx->server.sock, NC_MOBILE_SERVER_ADDR,
										 NC_MOBILE_SERVER_PORT, 10)
			|| na_sock_accept(&ctx->server.sock, &ctx->server.client->sock, 1))
	{
		na_sock_close(&ctx->server.sock, 0);
		return -1;
	}

	if (na_sock_connected(&ctx->server.client->sock))
	{
		if ((rv =
				 na_array_add(&ctx->server.clients, ctx->server.client, 0, 1, &citem)))
		{
			printf("%s:%d client connection insertion failure, rv=%d\n",
						 ctx->server.client->sock.raddr, ctx->server.client->sock.rport,
						 rv);
			na_sock_close(&ctx->server.client->sock, 0);
		}
		else
		{
			printf("%s:%d client is connected\n", ctx->server.client->sock.raddr,
						 ctx->server.client->sock.rport);

			ctx->server.client = 0;
		}
	}

	// ==================
	// ** process data **
	// ==================

	citem = 0;
	while ((citem = na_array_get_next(&ctx->server.clients, citem)))
	{
		cur_client = (NC_MOBILED_CLIENT *) citem->data;

		if ((rv =
				 na_sock_recv(&cur_client->sock, cur_client->rbuf + cur_client->rlen,
											NC_MOBILE_MAX_CLIENT_BUF - cur_client->rlen, &rlen, 0)))
		{
			if (-5 == rv)
				printf("%s:%d client connection closed\n", cur_client->sock.raddr,
							 cur_client->sock.rport);
			else
				printf("%s:%d client connection failure, rv=%d\n",
							 cur_client->sock.raddr, cur_client->sock.rport, rv);

			na_sock_term(&cur_client->sock);

			NA_MEM_CLEAN(cur_client->rbuf);
			NA_MEM_CLEAN(cur_client->sbuf);
			na_array_del_by_index(&ctx->server.clients, citem->index, 0);
		}
		else
		{
			if (rlen)
			{
				// printf("Mobiled receive crrent client %s:%d  \n",  cur_client->sock.raddr, cur_client->sock.rport ); 
				// printf("Mobiled receive current client  %s , length = %d \n", cur_client->rbuf + cur_client->rlen, cur_client->rlen ); 
				cur_client->rlen += rlen;	// update receive total length
				printf("Mobiled receive current client total length = %d \n",
							 cur_client->rlen);
				cur_client->lact = na_time_now();
			}
		}

		if (cur_client->rlen)
		{
			printf("aaa\n");
			if ((rv = nc_mobiled_serve(ctx, cur_client)) ||
					NC_MOBILE_TIMEOUT <= na_time_elapsed(cur_client->lact))
			{
				printf("%s:%d client connection processing failure, rv=%d\n",
							 cur_client->sock.raddr, cur_client->sock.rport, rv);

				na_sock_term(&cur_client->sock);

				NA_MEM_CLEAN(cur_client->rbuf);
				NA_MEM_CLEAN(cur_client->sbuf);
				na_array_del_by_index(&ctx->server.clients, citem->index, 0);
			}
		}

	}

	return 0;

}

int nc_mobiled_config(  NC_MOBILED * ctx  )
{
	GPRS_ATTR *gprs = &(ctx->gprs);
	parse_gprs_conf( gprs );
//	sleep( 5 );
//	parse_apn_conf( gprs);
	return 0;
}


