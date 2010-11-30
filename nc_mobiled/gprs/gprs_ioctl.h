
#ifndef GPRS_IOCTL_H
#define GPRS_IOCTL_H


#if ( defined ARCH_L200 ||defined ARCH_L100 )
/*GPRS  CTRL_PORT   &   DATA_PORT  */
#define GPRS_GTM900B_CTRL_PORT   	"/dev/gprs"
#define GPRS_GTM900B_DATA_PORT 	 "/dev/ttyS1"
#define GPRS_UC864E_CTRL_PORT 		 "/dev/ttyUC864E0"
#define GPRS_UC864E_DATA_PORT 	"/dev/ttyUC864E0"

/*Ioctl cmd both GTM900B/LC6311/UC864E/GOSPELL  GPRS modure*/
#define GPRS_POWERON        1    /*Power Up GPRS */
#define GPRS_POWERDOWN      2    /*Power donw GPRS*/
#define GPRS_GET_SIMSLOT    5    /*Get which SIM slot work now*/
#define GPRS_SET_SIMSLOT    6    /*Set which SIM slot work now*/
#define GPRS_GET_RI         7    /*Get the RI pin status in GPRS module*/
#define GPRS_CHK_SIM        13   /*Check the SIM door close or not*/

#define GPRS_SET_DTR        19   /*Set DTR pin status in GPRS module*/
#define GPRS_SET_RTS        20   /*Set RTS pin status in GPRS module*/

/*Special ioctl cmd for UC864E driver*/
#define GPRS_RESET          18   /*GPRS module reset */
#define GPRS_POWERMON       22   /*Get current GPRS power status(up/down) */
#define SET_UC864_USB_WORK  23   /*Set UC864E work or the extern USB interface work */

/*********************** ARCH L300 ioctl defined **********************************/
#endif /*  ARCH_L200 */
#if (defined ARCH_L300 || defined ARCH_L350) 
/*GPRS  CTRL_PORT   &   DATA_PORT  */
#define GPRS_GTM900B_CTRL_PORT   	"/dev/gprs"
#define GPRS_GTM900B_DATA_PORT 	 "/dev/ttyS2"
#define GPRS_UC864E_CTRL_PORT 		 "/dev/ttyUC864E0"
#define GPRS_UC864E_DATA_PORT 	"/dev/ttyUC864E0"
//#define GPRS_UC864E_CTRL_PORT 		 "/dev/ttyUSB0"
//#define GPRS_UC864E_DATA_PORT 	"/dev/ttyUSB0"

/*Ioctl cmd both GTM900B/LC6311/UC864E/GOSPELL GPRS modure*/
#define GPRS_POWERON        1    /*Power Up GPRS */
#define GPRS_POWERDOWN      2    /*Power donw GPRS*/

#define GPRS_GET_SIMSLOT    5    /*Get which SIM slot work now*/
#define GPRS_SET_SIMSLOT    6    /*Set which SIM slot work now*/
#define GPRS_CHK_SIM        13   /*Check the SIM door close or not*/

#define GPRS_DRV_DEBUG      17   /*Enable GPRS driver debug*/
#define GPRS_RESET          18   /*GPRS module reset */
#define GPRS_SET_DTR        19   /*Set DTR pin status in GPRS module*/

/*Special ioctl cmd for UC864E driver*/
#define GPRS_POWERMON       22   /*Get current GPRS power status(up/down) */

#endif /*Endif ARCH_L300*/

#endif /*Endif GPRS_IOCTL_H*/


