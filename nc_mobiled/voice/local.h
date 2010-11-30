#ifndef LOCAL_H
#define LOCAL_H

#include <time.h>

#include "common/GMaster.h"
#include "common/CArrayMgr.h"
#include "common/CLogger.h"
#include "common/CSocket.h"
#include "common/CUtility.h"
#include "common/CWorker.h"
#include "common/CComPort.h"
#include "common/CISO8583.h"
#include "common/GTLV.h"

#include "config/config.h"
//#include "modules/mod_crypto.h"

#if defined (WIN32)

	#define SYS_LOG_DEF_PATH		"C:\\netrouter.log"
	#define SYS_CFG_DEF_PATH		"C:\\netrouter_cfg.xml"
	#define SYS_STAT_DEF_PATH		"C:\\netrouter.info"

#elif defined (LINUX) || defined (UCLINUX)

	#define SYS_LOG_DEF_PATH		"/tmp/mobiled_voice.log"
	#define SYS_CFG_DEF_PATH		"/apps/etc/mobiled_voice_cfg.xml"
	#define SYS_STAT_DEF_PATH		"/stat/netrouter.info"

#endif

// ===================
// ** SYSTEM STATUS **
// ===================

#define SYS_STATUS_ONLINE			0x01
#define SYS_STATUS_IDLE				0x02
#define SYS_STATUS_PENDING			0x03
#define SYS_STATUS_CRITICAL			0x04
#define SYS_STATUS_CLOSING			0x05
#define SYS_STATUS_OFFLINE			0x06
#define SYS_STATUS_FAILURE			0x07
#define SYS_STATUS_STAND_BY			0x08
#define SYS_STATUS_ACTIVATE			0x09
#define SYS_STATUS_HARDWARE_FAILURE 0x0A
#define SYS_STATUS_SERVICE_FAILURE  0x0B


// ==================================
// ** SYSTEM CHECK DEPENDENCY MODE **
// ==================================

#define SYS_CHECK_MODE_FULL			0x01
#define SYS_CHECK_MODE_PARTIAL		0x02


// ============================
// ** SYSTEM CONNECTION FLAG **
// ============================

#define SYS_CONN_FLAG_DROP_LINE		0x80
#define SYS_CONN_FLAG_TUNNEL		0x40


// =======================
// ** SYSTEM LOG DEFINE **
// =======================

#define SYS_LOG_MIN					30
#define SYS_LOG_MIN_HALF			15
#define SYS_LOG_MAX					1024
#define SYS_LOG_MAX_HALF			512


// ===============================
// ** SYSTEM CO-RELATION DEFINE **
// ===============================

#define SYS_COREL_MAX				1024


// =====================================
// ** SYSTEM CONNECTION LIMIT CONTROL **
// =====================================

#define SYS_CONN_CAP_CHN_TCP		0x01
#define SYS_CONN_CAP_CHN_PSTN		0x02
#define SYS_CONN_CAP_CHN_RS232		0x03
#define SYS_CONN_CAP_CHN_RS485		0x04


// ==========================
// ** PARSER GLOBAL DEFINE **
// ==========================

#define SYS_PARSER_PSTN_CALL_ID			50	// PSTN incoming call ID

#define SYS_PARSER_TPDU_DST				281 // TPDU DST
#define SYS_PARSER_TPDU_SRC				282 // TPDU SRC
#define SYS_PARSER_TPDU_ORG_SRC			283 // TPDU ORG SRC

#define SYS_PARSER_APACS_IND			300	// APACS INDICATOR
#define SYS_PARSER_APACS_TID			301	// APACS TID
#define SYS_PARSER_APACS_MSGID			302	// APACS MSG ID


typedef struct COREL_ITEM_T
{
	unsigned long	ulPackTime,
					ulStartTime,
					ulTimeout;

	unsigned char	aucTPDUOrigSrc [3],
					ucOrigProtocolFmt,
					ucOrigProtocolLen;

	char			acOrigComID [MAX_TINY_BUFFER_SZ],
					acOrigConnID [MAX_TINY_BUFFER_SZ],
					acDestComID [MAX_TINY_BUFFER_SZ];

	CFG_ROUTE		*pstRoute;

	CFG_CRYPTO		*pstOrigCryptoCfg;

	CFG_DEST		*pstDest;
}
COREL_ITEM;

typedef struct COREL_T
{
	int				iPackSeqNo;

	CRITICAL_LOCK	stLock;

	ARRAY_LIST		stCoRels;	// COREL_ITEM
}
COREL;

typedef struct SYS_CONN_STAT_T
{
	int						iPSTN,
							iRS232,
							iRS485,
							iTCP,
							iPeak;

	CRITICAL_LOCK			stLock;
}
SYS_CONN_STAT;

typedef struct SYS_STAT_T
{
	unsigned long			ulPacket,
							ulTxn,
							ulTimeout,

							ulConnRetry,
							ulMobileRetry,
							ulDialupRetry;

	char					acBuf [MAX_LARGE_BUFFER_SZ];

	double					dbCurMobileDowntime,
							dbCurMobileUptime,
							dbSumMobileDowntime,
							dbSumMobileUptime,

							dbCurDialupDowntime,
							dbCurDialupUptime,
							dbSumDialupDowntime,
							dbSumDialupUptime;

	#if defined (LINUX) || defined (UCLINUX)

		unsigned long long

	#elif defined (WIN32)
		
		unsigned __int64

	#endif

							ullCurMobileTx,
							ullCurMobileRx,
							ullSumMobileTx,
							ullSumMobileRx,

							ullCurDialupTx,
							ullCurDialupRx,
							ullSumDialupTx,
							ullSumDialupRx,

							ullSumRoundTrip,
							ullAvgRoundTrip;

	struct timeb			stCurMobileDownStamp,
							stCurMobileUpStamp,

							stCurDialupDownStamp,
							stCurDialupUpStamp;

	SYS_CONN_STAT			stConn;

	// Internal use
	unsigned long			ulLastUpdate;

	FILE					*pfdFile;

	CRITICAL_LOCK			stLock;
}
SYS_STAT;

typedef struct SYS_CONN_T
{
	int						iConnID;	// Keep the unique seq connection id

	CRITICAL_LOCK			stLock;
}
SYS_CONN;

typedef struct PATTERN_DAT_T
{
	unsigned long	ulWaitDelay,
					ulRecvTimeout;
	
	// Use to mark the tunnel connection protocol format
	unsigned char	ucProtocolFmt,
					ucProtocolLen;

	// Use to mark the tunnel connection path
	char			acDestComID [MAX_TINY_BUFFER_SZ],
					acDestConnID [MAX_TINY_BUFFER_SZ];

	CFG_CRYPTO		*pstDestCryptoCfg;

	CFG_PATTERN		*pstPtn;
}
PATTERN_DAT;

typedef struct CPACKET_ADDR_T
{
	unsigned char	ucType;

	char			acComID [MAX_TINY_BUFFER_SZ],
					acConnID [MAX_TINY_BUFFER_SZ];
}
CPACKET_ADDR;

typedef struct CPACKET_T
{
	unsigned char	ucFlags;

	// Flag definitions
	// 0 0 0 0 0 0 0 0
	// | | |_|_|_|_|_|__________________________________________________ For furture use
	// | |______________________________________________________________ Tunnel flag - 0: no mark connection path, 1: mark connection path
	// |________________________________________________________________ Drop line flag - 0: dont drop line after failure, 1: drop line after failure

	unsigned long	ulLastRecv,
					ulPackTimeout;

	unsigned char	ucOrigProtocolFmt,
					ucOrigProtocolLen,
					ucDestProtocolFmt,
					ucDestProtocolLen,

					ucRouteState,
					ucStatus,	// Sender status

					ucMoreHex,
					*pucProcToken,
					*pucBuf;

	char			acOrigComID [MAX_TINY_BUFFER_SZ],
					acOrigConnID [MAX_TINY_BUFFER_SZ],
					acDestComID [MAX_TINY_BUFFER_SZ],
					acDestConnID [MAX_TINY_BUFFER_SZ],

					acHex [SYS_LOG_MAX + 1];

	int				iSize,
					iLen,
					iTotalLen;

	CPACKET_ADDR	stOrig,
					stDest;

	PATTERN_DAT		*pstPtnDat;

	ARRAY_LIST		stPtnDats;		// PATTERN_DAT

	ARRAY_LIST		stTunnels;		// CPACKET_ADDR
									// Use to receord each created tunnel, so that ease during destroying

	CFG_CRYPTO		*pstOrigCryptoCfg,
					*pstDestCryptoCfg;

	GTLV_DATA		stTLV;
}
CPACKET;

#endif
