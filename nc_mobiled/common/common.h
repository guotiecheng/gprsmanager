/*****************************************************************************
 * Copyright(c)  2009, GHL Sysytems Berhad. 
 *
 * Filename:     n300Util.h
 * Version:      1.0
 * Author:       Device Team
 * Date:         06/11/2009
 * Description:  The common defines of the whole project should be defined in  
 *               this file.
 *
 * Modification History
 *     1.        Version:       1.0
 *               Date:          06/11/2009
 *               Author:        Device Team
 *               Modification:  Creation
 *****************************************************************************/
#ifndef __COMMON_H
#define __COMMON_H

#ifdef __cplusplus
extern "C"
{
#endif

#include  <stdio.h>
#include  <stdlib.h>
#include  <unistd.h>
#include  <string.h>
#include  <stdarg.h>
#include  <libgen.h> /*for basename() and dirname() */

#include  <fcntl.h>
#include  <sys/stat.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include  <errno.h>
#include  <sys/select.h>
#include  <getopt.h>
#include  <termios.h>
#include  <time.h>
#include  <sys/time.h>
#include  <sys/timeb.h>

#include  <sys/wait.h>
#include  <sys/types.h>
#include  <sys/stat.h>
#include 	<sys/sysinfo.h>

//#include <version/version.h>
#include  <netcluster.h>
#include <comport/comport.h>
#include <gprs/gprs_ioctl.h>
#include <gprs/gprs.h>
#include <ppp/ppp.h>
#include <sms/smsapi.h>
#include <sms/pdu.h>
#include <parse/parse_conf.h>
#include <nc_mobile.h>
#include <nc_mobiled.h>



/****** Basic Data Type Define *****/
typedef  signed char        CHAR;        /* Signed   8  bit value */
typedef  unsigned char      UCHAR;       /* Unsigned 8  bit value */
typedef  signed short       SHORT;       /* Signed   16 bit value */
typedef  unsigned short     USHORT;      /* Unsigned 16 bit value */
typedef  signed long int    LONG;        /* Signed   32 bit value */
typedef  unsigned long int  ULONG;       /* Unsigned 32 bit value */

/****** Bit Operate Define *****/
#define SET_BIT(data, i)   ((data) |=  (1 << (i)))    /* Set the bit "i" in "data" to 1  */
#define CLR_BIT(data, i)   ((data) &= ~(1 << (i)))    /* Clear the bit "i" in "data" to 0 */
#define NOT_BIT(data, i)   ((data) ^=  (1 << (i)))    /* Inverse the bit "i" in "data"  */
#define GET_BIT(data, i)   ((data) >> (i) & 1)        /* Get the value of bit "i"  in "data" */
#define L_SHIFT(data, i)¡¡ ((data) << (i))            /* Shift "data" left for "i" bit  */
#define R_SHIFT(data, i)¡¡ ((data) >> (i))            /* Shift "data" Right for "i" bit  */

#define BIT0            0x01
#define BIT1            0x02
#define BIT2            0x04
#define BIT3            0x08
#define BIT4            0x10
#define BIT5            0x20
#define BIT6            0x40
#define BIT7            0x80


/****** Buffer Size Define   *****/
#define SIZE_32         32       
#define SIZE_64         64       
#define SIZE_128        128      
#define SIZE_256        256       
#define SIZE_512        512       
#define SIZE_1024       1024       
#define SIZE_2048       2048       
#define SIZE_4096       4096 


/**** Time Macro   ***/

#define GET_TICKCOUNT(x)   {struct timeval now; gettimeofday(&now, 0); x = now.tv_sec * 1000; \
                                        x += now.tv_usec/1000;}


#define SLEEP(m)       {struct timespec cSleep; unsigned long ulTmp; cSleep.tv_sec = m / 1000; \
                                    if(cSleep.tv_sec == 0){ulTmp = m * 10000; cSleep.tv_nsec = ulTmp * 100;} \
                                    else{cSleep.tv_nsec = 0;}nanosleep(&cSleep, 0);}

#define FOPEN(p,q,r)    p = fopen (q, r); printf ("FOPEN,0x%p,%u,%s\n", p, __LINE__, __FILE__)
#define FCLOSE(p)       if (0 < p){fclose (p); printf ("FCLOSE,0x%p,%u,%s\n", p, __LINE__, __FILE__); p = 0;}

/****** MAX/MIN Macro Define *****/
//#define MIN(x, y)       (((x) < (y)) ? (x) : (y))
//#define MAX(x, y)       (((x) > (y)) ? (x) : (y))


#define NSLEEP(m)       {struct timespec cSleep; unsigned long ulTmp; cSleep.tv_sec = m / 1000; \
                        if(cSleep.tv_sec == 0) {ulTmp = m * 10000; cSleep.tv_nsec = ulTmp * 100;} \
                        else{cSleep.tv_nsec = 0;}nanosleep(&cSleep, 0);}
#define msleep(m)       usleep(m*1000);

/****** Error Code Define *****/
#define OK              0
#define ERR            -1
#define FAILURE        -1
#define SUCCESS         0


/******gprs errors when check it***********/
#define SIM_DOOR_ERROR        0x01   
#define SIM_INSERT_ERROR      0x02
#define GPRS_SIGNAL_ERROR     0x03
#define GPRS_REGISTER_ERROR   0x04
#define GPRS_CARRIER_ERROR    0x05
/*******************************************/

#define TRUE    1
#define FALSE   0

#define PRGNAME_LEN       15
#define PIDFILE_LEN       2*PRGNAME_LEN+9  /* "/tmp/xx/xx.pid" */


#define PID_ASCII_SIZE  11
#define RUNNING         0x01
#define NOT_RUNNING     0x00

extern unsigned char     g_ucStop; /*Defined in common.c */

/*  Function Declare here  */
extern void usage(char *name);
extern void print_version (char *name);
extern void daemonforLinux (unsigned char ucDebug);
extern void daemonforUCLinux (unsigned char ucDebug, int argc, char **argv);
extern void signal_handler (int signo);
extern  int is_myself_running(const char *pid_path);
extern  int record_runpid_file(const char *pid_path);
extern void gblSetState(int holder);

#ifdef __cplusplus
}
#endif


#endif /*Undefine __COMMON_H*/

