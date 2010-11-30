/*******************************************************************************
* This file is licensed under the terms of your license agreement(s) with
* GHL covering this file. Redistribution, except as permitted by the terms of
* your license agreement(s) with GHL, is strictly prohibited.
********************************************************************************/

#include <common/common.h>

/* ==========================================================================================
 * Description: this function is the structure initializer, everytime before use
 *              structure must be initialize using this function.
 *              this function will initialize the communication library.
 *
 * Parameter  : [IN  ] ctx - structure context
 *
 * Return     : 0 - success
 *              else - failure
 * ==========================================================================================
 */
int nc_mobile_init (NC_MOBILE *ctx)
{
  nc_mobile_term (ctx);
  ctx->init = 1;

  if (na_sock_init (&ctx->sock, NA_SOCK_TYPE_TCP, 0, 0, 60, 2))
    return -1;


  if (na_time_retry_init (&ctx->retry, 5000, 1000, 3, NA_TIME_RETRY_FLAG_IMMEDIETELY))
    return -2;

  return 0;
}

/* ==========================================================================================
 * Description: this function is the config structure destructor, everytime after use
 *              config structure must be destroy using this function.
 *              this function will destroy the communication library, after that it will set
 *              the magic number to it. the magic number is use to indicate the structure has
 *              been properly initialized, so that any termination can be done correctly.
 *
 * Parameter  : [IN  ] ctx - structure context
 *
 * Return     : 0 - success
 *              else - failure
 * ==========================================================================================
 */
 
 // close  socket ; clean buf
void nc_mobile_term (NC_MOBILE *ctx)
{
  if (!ctx ||
      (NA_MAGIC == ctx->magic && !ctx->init))
    return;

  if (NA_MAGIC == ctx->magic && ctx->init) {
    na_sock_term (&ctx->sock);

    NA_MEM_CLEAN (ctx->buf);
  }

  memset (ctx, 0, sizeof (NC_MOBILE));
  ctx->magic = NA_MAGIC;
}

/* ==========================================================================================
 * Description: this function is use to retrieve configuration from the daemon, the daemon will
 *              retrieve configuration according to the provided application name or configuration
 *              path. the configuration will be store into the config parameter if the function
 *              returned success. the config parameter must be destroy after use.
 *
 * Parameter  : [IN  ] ctx - structure context
 *              [OUT ] config - retrieved configuration
 *              [IN  ] app_name - desired application name
 *              [IN  ] config_path - configuration path (optional)
 *
 * Return     : 0 - success
 *              else - failure
 * ==========================================================================================
 */
int nc_mobile ( NC_MOBILE *ctx , NC_MOBILE_DATA  *dat, int flag)
{
  int rv= 0;

  unsigned long start;

  unsigned int  rlen;

  if (NA_MAGIC != ctx->magic ||
      !ctx->init)
    nc_mobile_init (ctx);

  if (!na_sock_connected (&ctx->sock)) {
    rv = 1;
    if (na_sock_connecting (&ctx->sock) ||
        na_time_retry (&ctx->retry)) {
	    if(flag == 0)
	    {
      		if (!(rv = na_sock_connect (&ctx->sock, NC_MOBILE_SERVER_ADDR, 0, NC_MOBILE_SERVER_PORT, 0)))
        	rv = 0;
	    }
	    else if(flag == 1)
	    {
		    printf("This socket is to SMS server!\n");
      		if (!(rv = na_sock_connect (&ctx->sock, NC_MOBILE_SMS_SERVER_ADDR, 0, NC_MOBILE_SMS_SERVER_PORT, 0)))
       		rv = 0;
	    }
    }

    if (rv) {
      if (0 > rv)
        rv -= 100;
      return rv;
    }
  }


  if (!ctx->buf) {
    NA_MEM_ALLOC (ctx->buf, NC_MOBILE_MAX_CLIENT_BUF, (unsigned char *));
    if (!ctx->buf) {
      rv = -2;
      goto cleanup;
    }
  }

#if 0
  // prepare buf 
 strncpy((char *)ctx->buf, "Hello!", strlen("Hello!")  );
    ctx->len= strlen((char *)ctx->buf) + 1;
	//else ctx->len = 0;
#endif 
  if( 0 == nc_mobile_data_build( ctx, dat))
  	ctx->len = sizeof(NC_MOBILE_DATA);
  else ctx->len = 0;

  // prepare frame
  ctx->event.frame.dst_app = 0;
  ctx->event.frame.src_app = 0;  
  ctx->event.frame.ops = NC_MOBILE_OPS_REQ;
  ctx->event.frame.flags = 0;
  ctx->event.frame.dat = ctx->buf;
  ctx->event.frame.dat_len = ctx->len;

  ctx->event.frame.raw_frm = ctx->buf;
  ctx->event.frame.raw_end = ctx->buf + NC_MOBILE_MAX_CLIENT_BUF;
  ctx->event.frame.raw_len = ctx->len;  // message length 

//  int i=0;
  // dat-->content   raw_len = header_len + message_len
  if (nc_mobile_frame_build (&ctx->event) ||
      na_sock_send (&ctx->sock, ctx->event.frame.raw_frm, ctx->event.frame.raw_len)) {
    rv = -3;
    goto cleanup;
  }
  printf("send to server OK!\n");


 
  ctx->len  = 0;         //clear 

  start = na_time_now ();

  while (na_proc_token (0, 1)) {
    // check timeout
    if (NC_MOBILE_TIMEOUT < na_time_elapsed (start)) {
      rv = -4;
      goto cleanup;
    }

    if ((rv = na_sock_recv (&ctx->sock, ctx->buf + ctx->len, NC_MOBILE_MAX_CLIENT_BUF - ctx->len, &rlen, 1))) {
	printf("na_socke_recv error!\n");
	  rv = -5;
      goto cleanup;
    }
	
    if (rlen) {
      ctx->len += rlen;
      ctx->lact = na_time_now ();
    }

    if (ctx->len) {
      if (!nc_mobile_frame_parse (&ctx->event, ctx->buf, ctx->len, 0)) {

       if(0)
       {
		// process response
//        if (na_config_init (config, NA_CONFIG_MODE_STRING) ||
  //          na_config_load (config, (char *) ctx->event.config))
          rv = -6;
      	}	
        else
       {
       rv = 0;
       goto cleanup;
       }  
      }
    }
	  sleep(5);
  }

  rv = -6;

cleanup:
  na_sock_close (&ctx->sock, 0);

  return rv;

  
}



/* ==========================================================================================
 * Description: this function use to construct frame on request and response frame. the event
 *              structure must be initialize before call this function, this function will use
 *              the frame structure to construct frame buffer.
 *
 * Parameter  : [OUT ] event - structure context
 *
 * Return     : 0 - success
 *              else - failure
 * ==========================================================================================
 */
int nc_mobile_frame_build (NC_MOBILE_EVENT *event)
{
  unsigned int  hlen;

  unsigned char hbuf [NC_FRAME_LEN];

  if (event->frame.raw_end - event->frame.raw_frm < NC_FRAME_LEN + event->frame.raw_len)
    return -1;

#if 0

 printf("Server receiverd CMD = %d\n",pstDat->iCmd );


 pstSms = ( NC_MOBILE_CONTENT_SMS *)( pstDat->ucContent);

 printf("Server received TelNum = %s\n", pstSms->acTelNum);
 printf("Server received SmsMsg = %s\n", pstSms->acMsg);
#endif 

  if (nc_frame_build (&event->frame, hbuf, sizeof (hbuf), &hlen))
    return -2;

  // move existing message forward so that header can be insert infront
  memmove (event->frame.raw_frm + hlen,  event->frame.raw_frm, event->frame.raw_len);
  event->frame.dat += hlen;

#if 0
 printf("===========b4 memcpy=================\n");

 pstDat = (NC_MOBILE_DATA *)( event->frame.raw_frm+hlen);
 printf("Server receiverd CMD = %d\n",pstDat->iCmd );
 printf("Data area start address: %p\n", pstDat);

 pstSms = ( NC_MOBILE_CONTENT_SMS *)( pstDat->ucContent);

 printf("Server received TelNum = %s\n", pstSms->acTelNum);
 printf("Server received SmsMsg = %s\n", pstSms->acMsg);
  printf("Frame head address %p\n", (event->frame.raw_frm));
 printf("Frame data  address %p\n", (event->frame.raw_frm + hlen));;

#endif

  // insert header
  memcpy (event->frame.raw_frm, hbuf, hlen);
  event->frame.raw_len += hlen;

 #if 0
  for(i=0; i< hlen+12; i++)
 {
     printf("0x%02x ", *(event->frame.raw_frm+i));  
  }
  printf("\n");



printf("===========After memcpy=================\n");
 printf("Frame head address %p\n", (event->frame.raw_frm));
 printf("Frame data  address %p\n", (event->frame.raw_frm + hlen));;
   printf("Frame hlen = %u\n", hlen);
 pstDat = (NC_MOBILE_DATA *)( event->frame.raw_frm+hlen);
 printf("Data area start address: %p\n", pstDat);
 #endif

 #if 0
 NC_MOBILE_RESULT*pstDat ;
 pstDat = (NC_MOBILE_RESULT*)( event->frame.raw_frm+hlen);
 // NC_MOBILE_CONTENT_SMS  *pstSms;
 printf("Result CMD = %d\n",pstDat->iResult );
 printf("Result SmsMsg = %s\n", pstDat->ucContent);
#endif 

  return 0;
}

/* ==========================================================================================
 * Description: this function use to parse frame on received buffer into structure.
 *
 * Parameter  : [OUT ] event - structure context
 *              [OUT ] used - the used buffer length for the parsed data
 *
 * Return     : 0 - success
 *              else - failure
 * ==========================================================================================
 */
int nc_mobile_frame_parse (NC_MOBILE_EVENT *event, unsigned char *rbuf, unsigned int rlen, unsigned int *used)
{
  if (nc_frame_parse (&event->frame, rbuf, rlen, used))
	  return -1;

  event->data = (event->frame.dat_len) ? event->frame.dat : 0;

  event->clen = event->frame.dat_len;

  return 0;
}

int   nc_mobile_data_build(NC_MOBILE *ctx , NC_MOBILE_DATA *dat )
{
	if( ( NC_MOBILE_MAX_CLIENT_BUF < sizeof( NC_MOBILE_DATA ) ) || NULL == dat )
		return -1;
	else
		memcpy(ctx->buf, dat, sizeof(NC_MOBILE_DATA) + 1);

	return 0;
}


