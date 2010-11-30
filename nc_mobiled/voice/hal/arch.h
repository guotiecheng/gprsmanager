#ifndef ARCH_H
#define ARCH_H

#include "../local.h"

#include "hal_gtm900b.h"
#include "hal_g600.h"
#include "hal_uc864e.h"
#include "hal_cx930xx.h"
#include "hal_led.h"
#include "hal_buzzer.h"
#include "hal_eth.h"
//#include "arch_diagnose.h"

#define HAL_ARCH_NAME	"ARCH      "

// Alert
#define ARCH_ALERT_ONLINE         0x00
#define ARCH_ALERT_INTERRUPT      0x01
#define ARCH_ALERT_BACKUP         0x02
#define ARCH_ALERT_CRITICAL       0x03
#define ARCH_ALERT_DIAGNOSE       0x04
#define ARCH_ALERT_FAILURE        0x05

// State
#define ARCH_STATE_IDLE           0x01
#define ARCH_STATE_PAUSE          0x02
#define ARCH_STATE_GAP            0x03
#define ARCH_STATE_REST           0x04
#define ARCH_STATE_ON             0x05
#define ARCH_STATE_OFF            0x06

// Diagnose Status
#define ARCH_DIAGNOSE_IDLE        0x00
#define ARCH_DIAGNOSE_HARDWARE    0x10
#define ARCH_DIAGNOSE_LAN         0x20
#define ARCH_DIAGNOSE_WAN         0x30
#define ARCH_DIAGNOSE_HOST        0x40
#define ARCH_DIAGNOSE_RECOVERED   0x50
#define ARCH_DIAGNOSE_CABLE       0x60
#define ARCH_DIAGNOSE_BLINK       0x70

#define ARCH_DIAGNOSE_HARDWARE_MOBILE_FAIL    (ARCH_DIAGNOSE_HARDWARE | 0x01)
#define ARCH_DIAGNOSE_HARDWARE_SIM_FAIL       (ARCH_DIAGNOSE_HARDWARE | 0x02)
#define ARCH_DIAGNOSE_HARDWARE_PSTN_FAIL      (ARCH_DIAGNOSE_HARDWARE | 0x03)
#define ARCH_DIAGNOSE_HARDWARE_ETH_UNPLUG     (ARCH_DIAGNOSE_HARDWARE | 0x04)
#define ARCH_DIAGNOSE_HARDWARE_RS232_FAIL     (ARCH_DIAGNOSE_HARDWARE | 0x05)
#define ARCH_DIAGNOSE_HARDWARE_RS485_FAIL     (ARCH_DIAGNOSE_HARDWARE | 0x06)

#define ARCH_DIAGNOSE_LAN_GW_EMPTY            (ARCH_DIAGNOSE_LAN | 0x01)
#define ARCH_DIAGNOSE_LAN_GW_PING_FAIL        (ARCH_DIAGNOSE_LAN | 0x02)
#define ARCH_DIAGNOSE_LAN_INVALID_IP          (ARCH_DIAGNOSE_LAN | 0x03)
#define ARCH_DIAGNOSE_LAN_RESOLVE_FAIL        (ARCH_DIAGNOSE_LAN | 0x04)

#define ARCH_DIAGNOSE_WAN_MOBILE_PPP_FAIL     (ARCH_DIAGNOSE_WAN | 0x01)
#define ARCH_DIAGNOSE_WAN_INSPECTION_FAIL     (ARCH_DIAGNOSE_WAN | 0x02)

#define ARCH_DIAGNOSE_HOST_CONNECT_FAIL       (ARCH_DIAGNOSE_HOST | 0x01)
#define ARCH_DIAGNOSE_HOST_SSL_FAIL           (ARCH_DIAGNOSE_HOST | 0x02)

#define ARCH_DIAGNOSE_PSTN_CABLE_CONN         (ARCH_DIAGNOSE_CABLE | 0x01)
#define ARCH_DIAGNOSE_PSTN_CABLE_FAIL         (ARCH_DIAGNOSE_CABLE | 0x02)

#define ARCH_DIAGNOSE_PSTN_BLINK_ON         (ARCH_DIAGNOSE_BLINK | 0x01)
#define ARCH_DIAGNOSE_PSTN_BLINK_OFF         (ARCH_DIAGNOSE_BLINK | 0x02)

#define ARCH_SSL_HANDSHAKE_CONN_FAIL          0x01
#define ARCH_SSL_HANDSHAKE_RECOVER            0x02
#define ARCH_SSL_HANDSHAKE_FAIL               0x03


typedef struct ARCH_HI_T
{
	HAL_LED			stLED;

	HAL_BUZZER		stBuzzer;

	HAL_ETH			stEth;
}
ARCH_HI;	// Human interface

typedef struct ARCH_MOBILE_T
{
	HAL_GTM900B		stGTM900B;

	HAL_G600		  stG600;

	HAL_UC864E		stUC864E;
}
ARCH_MOBILE;

typedef struct ARCH_MODEM_T
{
	HAL_CX930XX		stCX930XX;
}
ARCH_MODEM;

typedef struct ARCH_T
{
	unsigned char	ucInit,
					ucAlert,
					ucDiagnoseAlert,
					ucDiagnoseLastAlert;

	char			acTrgID [MAX_TINY_BUFFER_SZ];

	ARCH_HI			stHI;

	ARCH_MOBILE		stMobile;

	ARCH_MODEM		stModem;

 // 	ARCH_DIAGNOSE stDiagnose;

	WRK_THREAD		stThread;
}
ARCH;

extern ARCH				g_stArch;

// == Arch functions ==
unsigned char arch_init ();
void arch_term ();

unsigned char arch_start ();
void arch_close (unsigned char p_ucWait);

#endif

