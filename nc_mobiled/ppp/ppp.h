#ifndef  _PPP_H
#define  _PPP_H

/*
 * =====================================================================================
 *
 *       Filename:  ppp.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/22/2010 10:42:48 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Guo Wenxue (kernel), guowenxue@ghlsystems.com
 *        Company:  GHL System Berhad.
 *
 * =====================================================================================
 */

#define MOD_PPP_PROC_NET_DEV		"/proc/net/dev"
#define NO_PPP_DEV_ERR  0x1
#define PPP_PING_ERR 0x2
#define TIME_NOT_ENOUGH	0x3
#define PPP_ERROR                       0x06



int	check_ppp_ping( GPRS_ATTR * gprs );
int	check_ppp_alive( GPRS_ATTR  *gprs );
int 	check_ppp_stat ( MOD_PPP_STAT *p_pstPPPStat );
int	ppp_start( GPRS_ATTR   *gprs );
int	ppp_term( GPRS_ATTR *gprs );
void  *PPP_Thread( void  * arg);
int	check_ppp_idle(MOD_PPP_STAT * p_pstPPPStartStat, unsigned long long * rBytesThreshold, unsigned long long * sBytesThreshlod);

#endif 

