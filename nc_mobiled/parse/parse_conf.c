#include <common/common.h>


int parse_gprs_conf( GPRS_ATTR *gprs)
{
#if 0
	   gprs->gprsInfo.module= MODULE_GTM900B;
           
	//    gprs->module = MODULE_UC864E;
	gprs->gprsInfo.type = MODE_3G; 
 	    gprs->idleTime = 5000;
	    gprs->pingTime = 5000;
	    strncpy(gprs->primarySimCarrier, "cmnet" , CARRIER_LEN); 	// Primary sim carrier is set by in gprs.conf
	    strncpy(gprs->secondarySimCarrier, "uninet", CARRIER_LEN);	// Secondary sim carrier is set alsl in gprs.conf
		
	//sim1
//	    gprs->sim1.netWorkType = MODE_GPRS;
	    strncpy(gprs->sim1.userGprsConf , "GHL", USER_LEN);
	    strncpy(gprs->sim1.passwordGprsConf, "ghl", PASSWD_LEN);
	    strncpy(gprs->sim1.pingIp, "10.78.28.15", IP_LEN);
	    strncpy(gprs->sim1.apnGprsConf , "cmnet", APN_LEN);
	    
	//sim2	
//	    gprs->sim2.netWorkType = MODE_GPRS;
   	    gprs->sim2.fallBackTime = 21600000;
	    strncpy(gprs->sim2.userGprsConf , "GHL", USER_LEN);
	    strncpy(gprs->sim2.passwordGprsConf, "ghl", PASSWD_LEN);
	    strncpy(gprs->sim2.pingIp, "10.78.28.15", IP_LEN);
	    strncpy(gprs->sim2.apnGprsConf , "cmnet", APN_LEN);

    	    return 0;

#endif 

	int rv;
	char *ptr  = NULL;

	NA_CONFIG *config = 0;
	NA_CONFIG_ITEM *item_conf = NULL , *item_sim = NULL , *item = NULL;
	NA_ARRAY_ITEM *array_item;

	NA_MEM_ALLOC(config, sizeof(NA_CONFIG), (NA_CONFIG *));
	if (!config)
	{

		nc_log(&logger, NC_MOBILED_NAME, NA_LOG_ERROR,  \
					 "na_mem_alloc failure \n", __LINE__);
		rv = -1;
		goto CleanUp;
	}
	
//  if ((rv = nc_config (& config.proxy, config, NC_MOBILED_NAME, 0))) {
	if ((rv = nc_config(&gprs->config.proxy, config, NC_MOBILED_NAME, 0)))
	{
		nc_log(&logger, NC_MOBILED_NAME, NA_LOG_ERROR,  \
					 "@%04d The interface configuration failure, rv=%d", __LINE__, rv);
		rv = -2;
		goto CleanUp;
	}

	nc_log(&logger, NC_MOBILED_NAME, NA_LOG_NORMAL,  \
					 "@%04d The interface configuration success , rv=%d", __LINE__, rv);
	  
	if (!(item_conf = na_config_find(config, 0, NC_MOBILED_CONF)))
	{
		nc_log(&logger, NC_MOBILED_NAME, NA_LOG_ERROR,
					 "@%04d The %s configuration is missing", __LINE__, NC_MOBILED_CONF);
		rv = -3;
		goto CleanUp;
	}

	nc_log(&logger, NC_MOBILED_NAME, NA_LOG_ERROR,
					 "@%04d The %s configuration is finded", __LINE__, NC_MOBILED_CONF);

//	if(!(item_conf = na_config_find(config, item_conf, NC_MOBILED_CONF)))
	// modem type
	if (!(item = na_config_find(config, item_conf, NC_MOBILED_CONF_MODEM)) ||
			!(ptr = na_config_get_string(item)) ||
			!(!strcmp(ptr, NC_MOBILED_MODEM_GTM900B_NAME)
				|| !strcmp(ptr, NC_MOBILED_MODEM_GTM900C_NAME)
				|| !strcmp(ptr, NC_MOBILED_MODEM_G600_NAME)
				|| !strcmp(ptr, NC_MOBILED_MODEM_UC864E_NAME)))
	{
		rv = -4;
		nc_log(&logger, NC_MOBILED_NAME, NA_LOG_ERROR,
				 "@%04d The modem type is not configure correctly at %d line , rv = %d",
					 __LINE__, na_config_line(item), rv);
		goto CleanUp;
	}

	nc_log(&logger, NC_MOBILED_NAME, NA_LOG_NORMAL,
					 "@%04d The modem type is finded  correctly at %d line , type is %s",
					 __LINE__, na_config_line(item), ptr);
	

	if (!strncasecmp(ptr, NC_MOBILED_MODEM_GTM900B_NAME,7 ))
	{
		 gprs->module = NC_MOBILED_MODEM_GTM900B;
		 gprs->type = MODE_GPRS;
	 	nc_log(&logger, NC_MOBILED_NAME, NA_LOG_DETAIL,
					 "@%04D Modem is %s",
					 __LINE__, NC_MOBILED_MODEM_GTM900B_NAME);
		 
	}
	else if (!strcmp(ptr, NC_MOBILED_MODEM_UC864E_NAME))
	{
		 gprs->module = NC_MOBILED_MODEM_UC864E;
		 gprs->type = MODE_3G;
    	 	 nc_log(&logger, NC_MOBILED_NAME, NA_LOG_DETAIL,
					 "@%04D Modem is %s",
					 __LINE__, NC_MOBILED_MODEM_UC864E_NAME);
	}
	else if (!strcmp(ptr, NC_MOBILED_MODEM_GTM900C_NAME))
	{
		 gprs->module = NC_MOBILED_MODEM_GTM900C;
		 gprs->type = MODE_GPRS;
		 nc_log(&logger, NC_MOBILED_NAME, NA_LOG_DETAIL,
					 "@%04D Modem is %s",
					 __LINE__, NC_MOBILED_MODEM_GTM900C_NAME);
		  
	}
	else if (!strcmp(ptr, NC_MOBILED_MODEM_G600_NAME))
	{
		 gprs->module = NC_MOBILED_MODEM_G600;
		 gprs->type = MODE_GPRS;
		 nc_log(&logger, NC_MOBILED_NAME, NA_LOG_DETAIL,
					 "@%04D Modem is %s",
					 __LINE__, NC_MOBILED_MODEM_G600_NAME);
	}

	//optional unit: ms
	if ( (item = na_config_find(config, item_conf, "idleTime"))) 
			 gprs->idleTime  = na_config_get_int(item);
	else
			 gprs->idleTime = IDLETIME_DEFAULT;

	//optional unit: ms
	if ((item = na_config_find(config, item_conf, "pingTime")) )
			 gprs->pingTime  = na_config_get_int(item);
	else
			 gprs->pingTime = PINGTIME_DEFAULT;

	NC_GPRS_SIM	 *config_sim = NULL;
	int size = 0;
	if (!(item_sim = na_config_find(config, item_conf, NC_MOBILED_CONF_SIM)))
	{
		rv = -4;
		nc_log(&logger, NC_MOBILED_NAME, NA_LOG_ERROR,
					 "@%04d The %s configuration is missing, rv = %d", __LINE__, NC_MOBILED_CONF_SIM, rv);
		goto CleanUp;
	}
	nc_log(&logger, NC_MOBILED_NAME, NA_LOG_NORMAL,
					 "@%04d SIM confiture is finded ", __LINE__);
	

	if( NULL != item_sim )
		size = na_config_size(config, item_sim);
	else
		size = 0;


	 rv = na_array_init( &gprs->config.array_sim, 256, NA_ARRAY_INDEX, \
			 NA_ARRAY_FLAG_CHK_DUP | NA_ARRAY_FLAG_NO_LOCK, 0);
    if( 0 != rv )
	{
		rv = -5;
		goto CleanUp;
	}
	
//get first sim elements
	item_sim = na_config_chld (config, item_sim);
//Add to queue
  if (item_sim && size) {
    do {      
	NA_MEM_ALLOC (config_sim, sizeof (NC_GPRS_SIM), ( NC_GPRS_SIM *));
       if (!config_sim) {
        rv = -6;
        goto CleanUp;      
	}
      // sim carrier
      if ((item = na_config_find (config, item_sim, "apn")) &&
          (ptr = na_config_get_string (item)))        
          strncpy (config_sim->apnGprsConf, ptr, sizeof (config_sim->apnGprsConf) - 1);

	// sim apn 
	if ((item = na_config_find (config, item_sim, "user")) &&
          (ptr = na_config_get_string (item)))        
          strncpy (config_sim->userGprsConf, ptr, sizeof (config_sim->userGprsConf) - 1);

        
      if ((item = na_config_find (config, item_sim, "passWord")) &&
          (ptr = na_config_get_string (item)))        
          strncpy (config_sim->passwordGprsConf, ptr, sizeof (config_sim->passwordGprsConf) - 1);

      if ((item = na_config_find (config, item_sim, "pingIp")) &&
          (ptr = na_config_get_string (item)))        
          strncpy (config_sim->pingIp, ptr, sizeof (config_sim->pingIp) - 1);

    if ( (item = na_config_find (config, item_sim, "fallBackTime"))  )
          config_sim->fallBackTime =  na_config_get_int(item);
          
	//Add to _array and build index -- carrier for search 
      if ((rv = na_array_add ( &gprs->config.array_sim, config_sim, 0, 1, 0))) {
        nc_log (&logger, "mobiled", NA_LOG_ERROR, "@%04d The %s config_sim object insertion failure, rv (%d)", __LINE__, config_sim->apnGprsConf, rv);
        rv = -7;        
	goto CleanUp;      
	}      
	  
	config_sim = 0;
	
    }    while ((item_sim = na_config_next (config, item_sim)));
  }
  
   item = 0;

	if ( !( na_array_get_by_index(&(gprs->config.array_sim), 0 , &array_item) ) )
	{
	config_sim = (NC_GPRS_SIM *) array_item->data;

 	strncpy( gprs->sim1.userGprsConf , config_sim->userGprsConf, USER_LEN - 1);
	strncpy( gprs->sim1.passwordGprsConf , config_sim->passwordGprsConf, PASSWD_LEN - 1);
	strncpy( gprs->sim1.pingIp , config_sim->pingIp,  IP_LEN - 1);
	strncpy( gprs->sim1.apnGprsConf , config_sim->apnGprsConf,  APN_LEN - 1);
	
    	}

	if (!( na_array_get_by_index(&(gprs->config.array_sim), 1 , &array_item) ))
	{
	config_sim = (NC_GPRS_SIM *) array_item->data;

 	strncpy( gprs->sim2.userGprsConf , config_sim->userGprsConf, USER_LEN - 1);
	strncpy( gprs->sim2.passwordGprsConf , config_sim->passwordGprsConf, PASSWD_LEN - 1);
	strncpy( gprs->sim2.pingIp , config_sim->pingIp,  IP_LEN - 1);
	strncpy( gprs->sim2.apnGprsConf , config_sim->apnGprsConf,  APN_LEN - 1);
	 gprs->sim2.fallBackTime = config_sim->fallBackTime;
	
    	}
	

	rv = 0;

CleanUp :
	
	na_config_term(config);
	NA_MEM_CLEAN(config);
	NA_MEM_CLEAN(item_sim);

	return rv;

}

int parse_apn_conf( GPRS_ATTR *gprs)
{
      SIM		*pSim = NULL;	

	 gprs->curSimNo = 1;
	 
     	if( SIM_1 == gprs->curSimNo)		
		pSim = &( gprs->sim1 );	
	else		if( SIM_2 == gprs->curSimNo )		
		pSim = &( gprs->sim2 );	
	else
		return -1;


#if 0
	strncpy(pSim->apnApnsConf, "cmnet", APN_LEN);
	strncpy(pSim->userApnsConf , "GHL",  USER_LEN );
	strncpy(pSim->passwordApnsConf , "ghl", PASSWD_LEN );
	
    	return 0;

#endif
//	strncpy(pSim->carrier, "China Mobile", CARRIER_LEN);
	int rv;
	char *ptr  = NULL;

	NA_CONFIG *config = 0;
	NA_CONFIG_ITEM *item_conf = NULL , *item_apn = NULL , *item = NULL;
	NA_ARRAY_ITEM *array_item;

	NA_MEM_ALLOC(config, sizeof(NA_CONFIG), (NA_CONFIG *));
	if (!config)
	{

		nc_log(&logger, NC_MOBILED_NAME, NA_LOG_ERROR,  \
					 "na_mem_alloc failure \n", __LINE__);
		rv = -1;
		goto CleanUp;
	}
	
//  if ((rv = nc_config (& config.proxy, config, NC_MOBILED_NAME, 0))) {
	if ((rv = nc_config(&gprs->config.proxy, config, NC_MOBILED_NAME, 0)))
	{
		nc_log(&logger, NC_MOBILED_NAME, NA_LOG_ERROR,  \
					 "@%04d The interface configuration failure, rv=%d", __LINE__, rv);
		rv = -2;
		goto CleanUp;
	}

	nc_log(&logger, NC_MOBILED_NAME, NA_LOG_NORMAL,  \
					 "@%04d The interface configuration success , rv=%d", __LINE__, rv);

	if (!(item_conf = na_config_find(config, 0, NC_MOBILED_CONF)))
	{
		nc_log(&logger, NC_MOBILED_NAME, NA_LOG_ERROR,
					 "@%04d The %s configuration is missing", __LINE__, NC_MOBILED_CONF);
		rv = -3;
		goto CleanUp;
	}

	nc_log(&logger, NC_MOBILED_NAME, NA_LOG_NORMAL,
					 "@%04d The %s configuration is finded", __LINE__, NC_MOBILED_CONF);

	NC_GPRS_APN *config_apn = NULL;
	int size = 0;
	if (!(item_apn = na_config_find(config, item_conf, NC_MOBILED_CONF_APNS)))
	{
		rv = -4;
		nc_log(&logger, NC_MOBILED_NAME, NA_LOG_ERROR,
					 "@%04d The %s configuration is missing, rv = %d", __LINE__, NC_MOBILED_CONF_APNS, rv);
		goto CleanUp;
	}
	nc_log(&logger, NC_MOBILED_NAME, NA_LOG_DETAIL,
					 "@%04d APN confiture is finded ", __LINE__);
	

	if( NULL != item_apn )
		size = na_config_size(config, item_apn);
	else
		size = 0;


	 rv = na_array_init( &gprs->config.array_apns, 256, NA_ARRAY_INDEX, \
			 NA_ARRAY_FLAG_CHK_DUP | NA_ARRAY_FLAG_NO_LOCK, 0);
    if( 0 != rv )
	{
		rv = -5;
		goto CleanUp;
	}
	
//get first sim elements
	item_apn = na_config_chld (config, item_apn);
  if (item_apn && size) {
    do {      
	NA_MEM_ALLOC (config_apn, sizeof (NC_GPRS_APN), ( NC_GPRS_APN *));
       if (!config_apn) {
        rv = -6;
        goto CleanUp;      
	}
      //  carrier
      if ((item = na_config_find (config, item_apn,  NC_MOBILED_CONF_APNS_CARRIER)) &&
          (ptr = na_config_get_string (item)))        
          strncpy (config_apn->carrier, ptr, sizeof (config_apn->carrier) - 1);

	//  apn 
	if ((item = na_config_find (config, item_apn, NC_MOBILED_CONF_APNS_USER)) &&
          (ptr = na_config_get_string (item)))        
          strncpy (config_apn->userApnConf, ptr, sizeof (config_apn->userApnConf) - 1);

        
      if ((item = na_config_find (config, item_apn, NC_MOBILED_CONF_APNS_PASSWORD)) &&
          (ptr = na_config_get_string (item)))        
          strncpy (config_apn->passwordApnConf, ptr, sizeof (config_apn->passwordApnConf) - 1);

      if ((item = na_config_find (config, item_apn, NC_MOBILED_CONF_APNS_APN)) &&
          (ptr = na_config_get_string (item)))        
          strncpy (config_apn->apnApnConf, ptr, sizeof (config_apn->apnApnConf) - 1);
          
	//Add to _array and build index -- carrier for search 
      if ((rv = na_array_add ( &gprs->config.array_apns, config_apn, config_apn->carrier, 1, 0))) {
       nc_log (&logger, "mobiled", NA_LOG_ERROR, "@%04d The %s config_apn object insertion failure, rv (%d)", __LINE__, config_apn->carrier, rv);
        rv = -7;        
	goto CleanUp;      
	}      
	  
	config_apn = 0;
	
    }    while ((item_apn = na_config_next (config, item_apn)));
  }
  
   item = 0;

	if ( !( na_array_get_by_id(&(gprs->config.array_apns), pSim->carrier, &array_item) ) )
	{
	config_apn = (NC_GPRS_APN *) array_item->data;
 	strncpy( pSim->userApnsConf, config_apn->userApnConf, USER_LEN - 1);
	strncpy( pSim->passwordApnsConf, config_apn->passwordApnConf, PASSWD_LEN - 1);
	strncpy( pSim->apnApnsConf, config_apn->apnApnConf,  APN_LEN - 1);
	
    	}

	rv = 0;

CleanUp :
	
	na_config_term(config);
	NA_MEM_CLEAN(config);
	NA_MEM_CLEAN(item_apn);
	return rv;
	
}

