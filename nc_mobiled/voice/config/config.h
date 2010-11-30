#ifndef CONFIG_H
#define CONFIG_H

#include "../common/CArrayMgr.h"
#include "../common/CXmlParser.h"
#include "../common/CLogger.h"
#include "../common/CUtility.h"
#include "../common/CISO8583.h"

extern LOGGER_VOICE	g_stVoiceLog;

extern unsigned char cor_getkey (char *p_pcKey, char *p_pcOut);

// PPP Auth Type Definition
#define	CFG_PPP_AUTH_TYPE_PAP															0x01
#define	CFG_PPP_AUTH_TYPE_CHAP															0x02

// PORT Type Definition
#define CFG_PORT_TYPE_TTY																0x01
#define CFG_PORT_TYPE_SC16																0x02

// PORT Mode Definition
#define CFG_PORT_MODE_RAW																0x01
#define CFG_PORT_MODE_ESLP_MASTER														0x02
#define CFG_PORT_MODE_ESLP_BACKUP														0x03
#define CFG_PORT_MODE_ESLP_NODE															0x04
#define CFG_PORT_MODE_GSM																0x05

// Parser Type Definition
#define CFG_PARSER_TYPE_NONE															0x00
#define CFG_PARSER_TYPE_TPDU															0x01
#define CFG_PARSER_TYPE_ISO8583															0x02
#define CFG_PARSER_TYPE_APACS															0x03

// Pattern Destination Definition
#define CFG_PATT_DEST_PRIMARY															0x01
#define CFG_PATT_DEST_SECONDARY															0x02

// Crypto source definition
#define CFG_CRYPTO_SRC_RAW																0x01
#define CFG_CRYPTO_SRC_FILE																0x02
#define CFG_CRYPTO_SRC_SAM                                                              0x03

// Crypto type definition
#define CFG_CRYPTO_TYPE_SSL																0x01
#define CFG_CRYPTO_TYPE_ENC																0x02

// Crypto padding type definition
#define CFG_CRYPTO_PADDING_TYPE_NONE                                                    0x00
#define CFG_CRYPTO_PADDING_TYPE_PKCS													0x01
#define CFG_CRYPTO_PADDING_TYPE_NULL_NUM_BYTES											0x02

// Connection Mode Definition
#define CFG_CONN_MODE_CONNECT															0x01
#define CFG_CONN_MODE_LISTEN															0x02

// Connection Type Definition
#define CFG_CONN_TYPE_PERMANENT															0x01
#define CFG_CONN_TYPE_ONDEMAND															0x02
#define CFG_CONN_TYPE_ONDEMAND_RR														0x03

// Connection Flag Definition
#define CFG_CONN_FLAG_DROP_LINE															0x80
#define CFG_CONN_FLAG_TUNNEL															0x40

// PSTN protocol type definition
#define CFG_PSTN_PROTOCOL_TYPE_FC_V22_NAME												"V22 FastConnect"
#define CFG_PSTN_PROTOCOL_TYPE_FC_V22B_NAME												"V22 bis FastConnect"
#define CFG_PSTN_PROTOCOL_TYPE_FC_V22B_2400_NAME										"V22 bis FastConnect (2400)"
#define CFG_PSTN_PROTOCOL_TYPE_V34_NAME													"V34"
#define CFG_PSTN_PROTOCOL_TYPE_V90_NAME													"V90"

#define CFG_PSTN_PROTOCOL_TYPE_FC_V22													0x01
#define CFG_PSTN_PROTOCOL_TYPE_FC_V22B													0x02
#define CFG_PSTN_PROTOCOL_TYPE_FC_V22B_2400												0x03
#define CFG_PSTN_PROTOCOL_TYPE_V34														0x04
#define CFG_PSTN_PROTOCOL_TYPE_V90														0x05

// Pattern Protocol Format Definition
#define CFG_PATTERN_PROTOCOL_FMT_NONE													0x00
#define CFG_PATTERN_PROTOCOL_FMT_HL_HEX													0x01
#define CFG_PATTERN_PROTOCOL_FMT_HL_BCD													0x02
#define CFG_PATTERN_PROTOCOL_FMT_IHL_HEX												0x03
#define CFG_PATTERN_PROTOCOL_FMT_IHL_BCD												0x04
#define CFG_PATTERN_PROTOCOL_FMT_HLHL_HEX												0x05
#define CFG_PATTERN_PROTOCOL_FMT_HLHL_BCD												0x06
#define CFG_PATTERN_PROTOCOL_FMT_LRC_HEX												0x07
#define CFG_PATTERN_PROTOCOL_FMT_LRC_BCD												0x08

// Depend Comparison Boolen
#define CFG_DEPEND_CMP_AND																0x01
#define CFG_DEPEND_CMP_OR																0x02

// Depend Type
#define CFG_DEPEND_TYPE_NORMAL															0x01
#define CFG_DEPEND_TYPE_IF																0x02
#define CFG_DEPEND_TYPE_HW																0x03

// Modem ID type
#define CFG_MODEM_ID_GTM900B_NAME														"GTM900B"
#define CFG_MODEM_ID_UC864E_NAME														"UC864E"
#define CFG_MODEM_ID_G600_NAME														  "G600"
#define CFG_MODEM_ID_CX930XX_NAME														"CX930XX"

#define CFG_MODEM_ID_GTM900B_CODE														0x01
#define CFG_MODEM_ID_UC864E_CODE														0x02
#define CFG_MODEM_ID_G600_CODE														  0x03
#define CFG_MODEM_ID_CX930XX_CODE														0x04

// Network type
#define CFG_PPP_NETWORK_TYPE_GPRS														0x01
#define CFG_PPP_NETWORK_TYPE_EDGE														0x02
#define CFG_PPP_NETWORK_TYPE_HSDPA														0x03

// PPP type
#define CFG_PPP_TYPE_MOBILE																0x01
#define CFG_PPP_TYPE_DIALUP																0x02

// Volume
#define CFG_OPTM_VOLUME_OFF_NAME														"O"
#define CFG_OPTM_VOLUME_LOW_NAME														"L"
#define CFG_OPTM_VOLUME_MIDDLE_NAME														"M"
#define CFG_OPTM_VOLUME_HIGH_NAME														"H"

#define CFG_OPTM_VOLUME_OFF_CODE														0x00
#define CFG_OPTM_VOLUME_LOW_CODE														0x01
#define CFG_OPTM_VOLUME_MIDDLE_CODE														0x02
#define CFG_OPTM_VOLUME_HIGH_CODE														0x03

// Common
#define CFG_COMMON_YES																	0x01
#define CFG_COMMON_NO																	0x00

#define MOD_CFG_NAME																	"CONFIG    "

//Config File Elements Definition
#define CFG_XML_CFGS_NAME																"Cfgs"
#define    CFG_XML_CORE_ENGINE_NAME														"CoreEngine"

#define       CFG_XML_LOGGER_VOICE_NAME														"Logger"
#define         CFG_XML_LOGGER_VOICE_DEST_NAME												"Dest"
#define         CFG_XML_LOGGER_VOICE_LEVEL_NAME												"Level"
#define         CFG_XML_LOGGER_VOICE_FILE_SIZE_NAME											"FileSize"
#define         CFG_XML_LOGGER_VOICE_DUPLICATE_NAME											"Duplicate"

#define       CFG_XML_MODULE_NAME														"Modules"
#define         CFG_XML_MODULE_BUZZER_NAME												"Buzzer"
#define         CFG_XML_MODULE_MOBILE_MODEM_ID_NAME										"MobileModemID"				// For backward compatibility

#define          CFG_XML_MODULE_OPTM_NAME												"OPTM"
#define             CFG_XML_MODULE_OPTM_ID_NAME											"ID"
#define             CFG_XML_MODULE_OPTM_MODEM_ID_NAME									"ModemID"
#define             CFG_XML_MODULE_OPTM_VOLUME_NAME										"Volume"

#define          CFG_XML_MODULE_GSM_NAME												"GSM"
#define             CFG_XML_MODULE_GSM_ID_NAME											"ID"
#define             CFG_XML_MODULE_GSM_MODEM_ID_NAME									"ModemID"
#define             CFG_XML_MODULE_GSM_CTRLDRV_NAME										"CtrlDrv"
#define             CFG_XML_MODULE_GSM_DATADRV_NAME										"DataDrv"

#define          CFG_XML_MODULE_PPPS_NAME												"PPPs"
#define             CFG_XML_MODULE_PPPS_PPP_NAME										"PPP"
#define               CFG_XML_MODULE_PPPS_PPP_ID										"ID"
#define               CFG_XML_MODULE_PPPS_PPP_QUERY										"Query"
#define               CFG_XML_MODULE_PPPS_PPP_MODEM_ID									"ModemID"

#define                CFG_XML_MODULE_PPPS_PPP_PSTN_NAME								"PSTN"
#define                  CFG_XML_MODULE_PPPS_PPP_PSTN_ID								"ID"
#define                  CFG_XML_MODULE_PPPS_PPP_PSTN_DIAL								"Dial"
#define                  CFG_XML_MODULE_PPPS_PPP_PSTN_UID								"Username"
#define                  CFG_XML_MODULE_PPPS_PPP_PSTN_PWD								"Password"
#define                  CFG_XML_MODULE_PPPS_PPP_PSTN_AUTH								"Auth"
#define                  CFG_XML_MODULE_PPPS_PPP_PSTN_IP								"IP"
#define                  CFG_XML_MODULE_PPPS_PPP_PSTN_HANDSHAKE							"Handshake"
#define                  CFG_XML_MODULE_PPPS_PPP_PSTN_CONN_TIMEOUT						"ConnTimeout"
#define                  CFG_XML_MODULE_PPPS_PPP_PSTN_IDLE_TIMEOUT						"IdleTimeout"
#define                  CFG_XML_MODULE_PPPS_PPP_PSTN_FALLBACK							"Fallback"
#define                  CFG_XML_MODULE_PPPS_PPP_PSTN_FALLBACK_THRESHOLD				"FallbackThreshold"

#define                CFG_XML_MODULE_PPPS_PPP_SIM_NAME									"SIM"
#define                  CFG_XML_MODULE_PPPS_PPP_SIM_APN								"APN"
#define                  CFG_XML_MODULE_PPPS_PPP_SIM_OPE								"OPE"
#define                  CFG_XML_MODULE_PPPS_PPP_SIM_UID								"Username"
#define                  CFG_XML_MODULE_PPPS_PPP_SIM_PWD								"Password"
#define                  CFG_XML_MODULE_PPPS_PPP_SIM_AUTH								"Auth"
#define                  CFG_XML_MODULE_PPPS_PPP_SIM_IP									"IP"
#define                  CFG_XML_MODULE_PPPS_PPP_SIM_NETWORK_TYPE						"NetworkType"
#define                  CFG_XML_MODULE_PPPS_PPP_SIM_IDLE_TIMEOUT						"IdleTimeout"
#define                  CFG_XML_MODULE_PPPS_PPP_SIM_FALLBACK							"Fallback"
#define                  CFG_XML_MODULE_PPPS_PPP_SIM_FALLBACK_THRESHOLD					"FallbackThreshold"

#define                     CFG_XML_MODULE_PPPS_PPP_PING								"Ping"
#define                     CFG_XML_MODULE_PPPS_PPP_PING_IP								"IP"
#define                     CFG_XML_MODULE_PPPS_PPP_PING_INTERVAL						"Interval"
#define                     CFG_XML_MODULE_PPPS_PPP_PING_THRESHOLD						"Threshold"

#define          CFG_XML_MODULE_PSTNS_NAME												"PSTNs"
#define             CFG_XML_MODULE_PSTNS_PSTN_NAME										"PSTN"
#define               CFG_XML_MODULE_PSTNS_PSTN_ID										"ID"
#define               CFG_XML_MODULE_PSTNS_PSTN_DATADRV									"DataDrv"
#define               CFG_XML_MODULE_PSTNS_PSTN_CTRLDRV									"CtrlDrv"
#define               CFG_XML_MODULE_PSTNS_PSTN_TYPE									"Type"
#define               CFG_XML_MODULE_PSTNS_PSTN_COUNTRY_REG								"CountryReg"

#define          CFG_XML_MODULE_RS232S_NAME												"RS232s"
#define             CFG_XML_MODULE_RS232S_RS232_NAME									"RS232"
#define               CFG_XML_MODULE_RS232S_RS232_ID									"ID"
#define               CFG_XML_MODULE_RS232S_RS232_DATADRV								"DataDrv"
#define               CFG_XML_MODULE_RS232S_RS232_TYPE									"Type"
#define               CFG_XML_MODULE_RS232S_RS232_BAUDRATE								"BaudRate"
#define               CFG_XML_MODULE_RS232S_RS232_STOPBIT								"StopBit"
#define               CFG_XML_MODULE_RS232S_RS232_DATABIT								"DataBit"
#define               CFG_XML_MODULE_RS232S_RS232_PARITY								"Parity"
#define               CFG_XML_MODULE_RS232S_RS232_FLOWCTRL								"FlowCtrl"

#define          CFG_XML_MODULE_RS485S_NAME												"RS485s"
#define             CFG_XML_MODULE_RS485S_RS485_NAME									"RS485"
#define               CFG_XML_MODULE_RS485S_RS485_ID_NAME								"ID"
#define               CFG_XML_MODULE_RS485S_RS485_DATADRV_NAME							"DataDrv"
#define               CFG_XML_MODULE_RS485S_RS485_CTRLDRV_NAME							"CtrlDrv"
#define               CFG_XML_MODULE_RS485S_RS485_TYPE_NAME								"Type"
#define               CFG_XML_MODULE_RS485S_RS485_BAUDRATE_NAME							"BaudRate"
#define               CFG_XML_MODULE_RS485S_RS485_STOPBIT_NAME							"StopBit"
#define               CFG_XML_MODULE_RS485S_RS485_DATABIT_NAME							"DataBit"
#define               CFG_XML_MODULE_RS485S_RS485_PARITY_NAME							"Parity"
#define               CFG_XML_MODULE_RS485S_RS485_FLOWCTRL_NAME							"FlowCtrl"

#define          CFG_XML_CRYPTOS_NAME													"Cryptos"
#define             CFG_XML_CRYPTO_NAME													"Crypto"
#define               CFG_XML_CRYPTO_ID_NAME											"ID"
#define               CFG_XML_CRYPTO_TYPE_NAME											"Type"
#define               CFG_XML_CRYPTO_INDEX_NAME											"Index"
#define               CFG_XML_CRYPTO_SESSION_TIMEOUT_NAME								"SessionTimeout"
#define               CFG_XML_CRYPTO_CIPHER_NAME										"Cipher"
#define               CFG_XML_CRYPTO_SRC_NAME											"Source"
#define               CFG_XML_CRYPTO_CERT_NAME											"Cert"
#define               CFG_XML_CRYPTO_PRVT_KEY_NAME										"PrvtKey"
#define               CFG_XML_CRYPTO_CA_NAME											"CA"
#define               CFG_XML_CRYPTO_CRL_NAME											"CRL"
#define               CFG_XML_CRYPTO_PASSPHASE_NAME										"PassPhase"
#define               CFG_XML_CRYPTO_CN_NAME											"CN"
#define               CFG_XML_CRYPTO_PADDING_NAME										"Padding"
#define               CFG_XML_CRYPTO_PKCS11_OPS_IP_NAME								    "OPSIP"
#define               CFG_XML_CRYPTO_PKCS11_OPS_PORT_NAME								"OPSPort"
#define               CFG_XML_CRYPTO_PKCS11_OPS_TIMEOUT_NAME						    "OPSTimeout"

#define       CFG_XML_CHANNEL_NAME														"Channels"
#define          CFG_XML_CHANNEL_TCPS_NAME												"TCPs"
#define             CFG_XML_CHANNEL_TCPS_TCP_NAME										"TCP"
#define               CFG_XML_CHANNEL_TCPS_TCP_ID										"ID"
#define               CFG_XML_CHANNEL_TCPS_TCP_BUFSIZE									"BufSize"
#define               CFG_XML_CHANNEL_TCPS_TCP_CONN_MODE_NAME							"ConnMode"
#define               CFG_XML_CHANNEL_TCPS_TCP_CONN_TYPE_NAME							"ConnType"
#define               CFG_XML_CHANNEL_TCPS_TCP_IP									    "IP"
#define               CFG_XML_CHANNEL_TCPS_TCP_LOCPORT									"LocPort"
#define               CFG_XML_CHANNEL_TCPS_TCP_REMPORT									"RemPort"
#define               CFG_XML_CHANNEL_TCPS_TCP_CONN_TIMEOUT_NAME						"ConnTimeout"
#define               CFG_XML_CHANNEL_TCPS_TCP_IDLE_TIMEOUT_NAME						"IdleTimeout"
#define               CFG_XML_CHANNEL_TCPS_TCP_WRK_MIN_NAME								"MinWrk"
#define               CFG_XML_CHANNEL_TCPS_TCP_WRK_MAX_NAME								"MaxWrk"
#define               CFG_XML_CHANNEL_TCPS_TCP_WRK_KILL_IDLE_NAME						"KillIdleWrk"
#define               CFG_XML_CHANNEL_TCPS_TCP_KEEP_ALIVE_INTERVAL_NAME					"KeepAliveInterval"
#define               CFG_XML_CHANNEL_TCPS_TCP_KEEP_ALIVE_RETRY_NAME					"KeepAliveRetry"
#define               CFG_XML_CHANNEL_TCPS_TCP_PCK_TIMEOUT_NAME							"PackTimeout"
#define               CFG_XML_CHANNEL_TCPS_TCP_DROP_LINE_FLAG_NAME						"DropLineFlag"
#define               CFG_XML_CHANNEL_TCPS_TCP_TUNNEL_FLAG_NAME							"TunnelFlag"
#define                  CFG_XML_CHANNEL_TCPS_TCP_TARGETS_NAME							"Targets"
#define                     CFG_XML_CHANNEL_TCPS_TCP_TARGETS_TARGET_NAME				"Target"
#define                       CFG_XML_CHANNEL_TCPS_TCP_TARGETS_TARGET_PATTERN_NAME		"Pattern"
#define                       CFG_XML_CHANNEL_TCPS_TCP_TARGETS_TARGET_PARSER_NAME		"Parser"
#define                  CFG_XML_CHANNEL_TCPS_TCP_CRYPTO_NAME							"Crypto"
#define                    CFG_XML_CHANNEL_TCPS_TCP_CRYPTO_ID_NAME						"ID"

#define          CFG_XML_CHANNEL_PSTNS_NAME												"PSTNs"
#define             CFG_XML_CHANNEL_PSTNS_PSTN_NAME										"PSTN"
#define               CFG_XML_CHANNEL_PSTNS_PSTN_ID										"ID"
#define               CFG_XML_CHANNEL_PSTNS_PSTN_MODEM_NAME								"Modem"
#define               CFG_XML_CHANNEL_PSTNS_PSTN_PROTOCOL_NAME							"Protocol"
#define               CFG_XML_CHANNEL_PSTNS_PSTN_CONN_TYPE_NAME							"ConnType"
#define               CFG_XML_CHANNEL_PSTNS_PSTN_CONN_TIMEOUT_NAME						"ConnTimeout"
#define               CFG_XML_CHANNEL_PSTNS_PSTN_IDLE_TIMEOUT_NAME						"IdleTimeout"
#define               CFG_XML_CHANNEL_PSTNS_PSTN_PCK_TIMEOUT_NAME						"PackTimeout"
#define               CFG_XML_CHANNEL_PSTNS_PSTN_SNRM_TIMEOUT_NAME						"SNRMTimeout"
#define               CFG_XML_CHANNEL_PSTNS_PSTN_RR_TIMEOUT_NAME						"RRTimeout"
#define               CFG_XML_CHANNEL_PSTNS_PSTN_ATTENUATION_NAME           "Attenuation"
#define               CFG_XML_CHANNEL_PSTNS_PSTN_BUFSIZE_NAME							"BufSize"
#define               CFG_XML_CHANNEL_PSTNS_PSTN_DROP_LINE_FLAG_NAME					"DropLineFlag"
#define               CFG_XML_CHANNEL_PSTNS_PSTN_TUNNEL_FLAG_NAME						"TunnelFlag"
#define                CFG_XML_CHANNEL_PSTNS_PSTN_NODES_NAME							"Nodes"
#define                   CFG_XML_CHANNEL_PSTNS_PSTN_NODE_NAME							"Node"
#define                     CFG_XML_CHANNEL_PSTNS_PSTN_NODE_DIAL_NAME					"Dial"
#define                CFG_XML_CHANNEL_PSTNS_PSTN_TARGETS_NAME							"Targets"
#define                CFG_XML_CHANNEL_PSTNS_PSTN_TARGETS_TARGET_NAME					"Target"
#define                CFG_XML_CHANNEL_PSTNS_PSTN_TARGETS_TARGET_PATTERN_NAME			"Pattern"

#define          CFG_XML_CHANNEL_RS232S_NAME											"RS232s"
#define             CFG_XML_CHANNEL_RS232S_RS232_NAME									"RS232"
#define               CFG_XML_CHANNEL_RS232S_RS232_ID									"ID"
#define               CFG_XML_CHANNEL_RS232S_RS232_MODE_NAME							"Type"
#define               CFG_XML_CHANNEL_RS232S_RS232_BUFSIZE								"BufSize"
#define               CFG_XML_CHANNEL_RS232S_RS232_PCK_TIMEOUT_NAME						"PackTimeout"
#define               CFG_XML_CHANNEL_RS232S_RS232_TUNNEL_FLAG_NAME						"TunnelFlag"
#define                CFG_XML_CHANNEL_RS232S_RS232_TARGETS_NAME						"Targets"
#define                   CFG_XML_CHANNEL_RS232S_RS232_TARGETS_TARGET_NAME				"Target"
#define                     CFG_XML_CHANNEL_RS232S_RS232_TARGETS_TARGET_PATTERN_NAME	"Pattern"

#define          CFG_XML_CHANNEL_RS485S_NAME											"RS485s"
#define             CFG_XML_CHANNEL_RS485S_RS485_NAME									"RS485"
#define               CFG_XML_CHANNEL_RS485S_RS485_ID									"ID"
#define               CFG_XML_CHANNEL_RS485S_RS485_MODE_NAME							"Type"
#define               CFG_XML_CHANNEL_RS485S_RS485_BUFSIZE								"BufSize"
#define               CFG_XML_CHANNEL_RS485S_RS485_RAW_LOG								"RawLog"
#define               CFG_XML_CHANNEL_RS485S_RS485_PCK_TIMEOUT_NAME						"PackTimeout"
#define                CFG_XML_CHANNEL_RS485S_RS485_TARGETS_NAME						"Targets"
#define                   CFG_XML_CHANNEL_RS485S_RS485_TARGETS_TARGET_NAME				"Target"
#define                     CFG_XML_CHANNEL_RS485S_RS485_TARGETS_TARGET_PATTERN_NAME	"Pattern"

#define       CFG_XML_PATTERNS_NAME														"Patterns"
#define          CFG_XML_PATTERNS_PATTERN_NAME											"Pattern"
#define            CFG_XML_PATTERNS_PATTERN_ID_NAME										"ID"
#define            CFG_XML_PATTERNS_PATTERN_FORMAT_NAME									"Format"
#define             CFG_XML_PATTERNS_PATTERN_PARSER_NAME								"Parser"
#define               CFG_XML_PATTERNS_PATTERN_PARSER_ID								"ID"
#define               CFG_XML_PATTERNS_PATTERN_PARSER_STAMPTPDU							"StampTPDU"
#define               CFG_XML_PATTERNS_PATTERN_PARSER_PROTOCOLFMT_NAME					"ProtocolFmt"
#define               CFG_XML_PATTERNS_PATTERN_PARSER_DEF_FILE_NAME						"DefFile"
#define             CFG_XML_PATTERNS_PATTERN_ROUTE_NAME									"Route"
#define               CFG_XML_PATTERNS_PATTERN_ROUTE_KEY_NAME							"Key"
#define               CFG_XML_PATTERNS_PATTERN_ROUTE_VAL_NAME							"Val"
#define               CFG_XML_PATTERNS_PATTERN_ROUTE_COREL_NAME							"CoRel"
#define               CFG_XML_PATTERNS_PATTERN_ROUTE_GENCOREL_NAME						"GenCoRel"
#define               CFG_XML_PATTERNS_PATTERN_ROUTE_SELFROUTE_NAME						"SelfRoute"
#define                CFG_XML_PATTERNS_PATTERN_DEST_NAME								"Dest"
#define                  CFG_XML_PATTERNS_PATTERN_DEST_ID_NAME							"ID"
#define                  CFG_XML_PATTERNS_PATTERN_DEST_TYPE_NAME						"Type"
#define                  CFG_XML_PATTERNS_PATTERN_DEST_WEIGHT_NAME						"Weight"
#define                  CFG_XML_PATTERNS_PATTERN_DEST_CONGESTED_NAME					"Congested"
#define                  CFG_XML_PATTERNS_PATTERN_DEST_THRESHOLD_NAME					"Threshold"
#define                  CFG_XML_PATTERNS_PATTERN_DEST_COLDOWN_NAME						"Coldown"					// For backward compatibility
#define                  CFG_XML_PATTERNS_PATTERN_DEST_COOLDOWN_NAME					"Cooldown"
#define                  CFG_XML_PATTERNS_PATTERN_DEST_PROTOCOLFMT_NAME					"ProtocolFmt"

#define       CFG_XML_DEPENDENCIES_NAME													"Dependencies"
#define          CFG_XML_DEPENDENCIES_DEPENDENCY_NAME									"Dependency"
#define            CFG_XML_DEPENDENCIES_DEPENDENCY_ID_NAME								"ID"
#define            CFG_XML_DEPENDENCIES_DEPENDENCY_DEPEND_NAME							"Depend"
#define            CFG_XML_DEPENDENCIES_DEPENDENCY_CMP_NAME								"Cmp"

#define       CFG_XML_DIAGNOSE_NAME													"Diagnose"
#define       CFG_XML_DIAGNOSE_WAN_PING_IP_NAME						    "WanPingIP"
#define       CFG_XML_DIAGNOSE_RETRY_THRESHOLD_NAME					  "RetryThreshold"
#define       CFG_XML_DIAGNOSE_RETRY_INTERVAL_NAME					  "RetryInterval"

// ====================================
// ** Logger Configuration structure **
// ====================================

typedef struct CFG_LOGGER_VOICE_T
{
	unsigned char		ucLevel,
						ucDuplicate;

	char				acFileName [MAX_SMALL_BUFFER_SZ];

	unsigned long		ulFileSize;
}
CFG_LOGGER_VOICE;



// =====================================
// ** Modules Configuration structure **
// =====================================

// == PPP ==

typedef struct CFG_PPP_PING_T
{
	unsigned char		ucThreshold;

	char				acIP [16];

	unsigned long		ulInterval;
}
CFG_PPP_PING;

typedef struct CFG_SIM_T
{
	unsigned char		ucAuth,
						ucNetworkType;

	unsigned long		ulIdleTimeout,
						ulFallback;

	char				acAPN [MAX_SMALL_BUFFER_SZ],
						acOPE [MAX_SMALL_BUFFER_SZ],
						acUID [MAX_LITTLE_BUFFER_SZ],
						acPWD [MAX_LITTLE_BUFFER_SZ],
						acIP [16];

	int					iFallbackThreshold;

	CFG_PPP_PING		stPing;
}
CFG_SIM;

typedef struct CFG_MOD_PPP_PSTN_T
{
	unsigned char		ucAuth,
						ucHandshake;

	unsigned long		ulConnTimeout,
						ulIdleTimeout,
						ulFallback;

	char				acID [MAX_TINY_BUFFER_SZ],
						acDial [MAX_SMALL_BUFFER_SZ],
						acUID [MAX_LITTLE_BUFFER_SZ],
						acPWD [MAX_LITTLE_BUFFER_SZ],
						acIP [16];

	int					iFallbackThreshold;

	CFG_PPP_PING		stPing;
}
CFG_MOD_PPP_PSTN;

typedef struct CFG_MOD_PPP_T
{
	unsigned char		ucType,
						ucModemID,
						ucQuery,
						ucLinkType;

	char				acID [MAX_TINY_BUFFER_SZ];

	int					iIfIndex;		// PPP interface index (0 to n)

	// MOBILE on PPP
	CFG_SIM				stSIM1,
						stSIM2;

	// PSTN on PPP
	CFG_MOD_PPP_PSTN	stPSTN1,
						stPSTN2;
}
CFG_MOD_PPP;

// == GSM ==

typedef struct CFG_MOD_GSM_T
{
	unsigned char		ucModemID;

	char				acID [MAX_TINY_BUFFER_SZ],
						acCtrlDrv [MAX_SMALL_BUFFER_SZ],
						acDataDrv [MAX_SMALL_BUFFER_SZ];
}
CFG_MOD_GSM;

// == OPTM (Optimus) ==

typedef struct CFG_MOD_OPTM_T
{
	unsigned char		ucModemID,
						ucVolume;

	char				acID [MAX_TINY_BUFFER_SZ];
}
CFG_MOD_OPTM;

// == PSTN ==

typedef struct CFG_MOD_PSTN_T
{
	unsigned char		ucType;


	char				acID [MAX_TINY_BUFFER_SZ],
						acDataDrv [MAX_SMALL_BUFFER_SZ],
						acCtrlDrv [MAX_SMALL_BUFFER_SZ],
						acCountryReg [3];
}
CFG_MOD_PSTN;

// ** RS232

typedef struct CFG_MOD_RS232_T
{
	unsigned char		ucType,
						ucStopBit,
						ucDataBit,
						ucParity,
						ucFlowCtrl;

	char				acID [MAX_TINY_BUFFER_SZ],
						acDataDrv [MAX_SMALL_BUFFER_SZ];

	unsigned long		ulBaudRate;
}
CFG_MOD_RS232;

// ** RS485

typedef struct CFG_MOD_RS485_T
{
	unsigned char		ucType,
						ucStopBit,
						ucDataBit,
						ucParity,
						ucFlowCtrl;

	char				acID [MAX_TINY_BUFFER_SZ],
						acCtrlDrv [MAX_SMALL_BUFFER_SZ],
						acDataDrv [MAX_SMALL_BUFFER_SZ];

	unsigned long		ulBaudRate;
}
CFG_MOD_RS485;

typedef struct CFG_MODULES_T
{
	unsigned char		ucBuzzer;

	ARRAY_LIST			stPPPs;		// CFG_MOD_PPP

	CFG_MOD_GSM			stGSM;

	CFG_MOD_OPTM		stOPTM;

	ARRAY_LIST			stPSTNs,	// CFG_MOD_PSTN
						stRS232s,	// CFG_MOD_RS232
						stRS485s;	// CFG_MOD_RS485
}
CFG_MODULES;

// ====================================================


// =====================================
// ** Target configuration structure **
// =====================================

typedef struct CFG_TARGET_T
{
	char				acPattern [MAX_TINY_BUFFER_SZ];
}
CFG_TARGET;


// ====================================
// ** Crypto configuration structure **
// ====================================

typedef struct CFG_CRYPTO_T
{
	unsigned char		ucIndex,
						ucType,
						ucSrc,
						ucPadding;

	char				acID [MAX_TINY_BUFFER_SZ],
						acCN [MAX_NORMAL_BUFFER_SZ],
						acCipher [MAX_NORMAL_BUFFER_SZ];

	unsigned long		ulSessionTimeout,
	                    ulOPSTimeout;

	char				acCert [MAX_LARGE_BUFFER_SZ],
						acPrvtKey [MAX_LARGE_BUFFER_SZ],
						acCA [MAX_LARGE_BUFFER_SZ],
						acCRL [MAX_LARGE_BUFFER_SZ],
						acPassPhase [MAX_BIG_BUFFER_SZ],
						acOPSIP [MAX_SMALL_BUFFER_SZ];

    int                 iOPSPort;
}
CFG_CRYPTO;


// =====================================
// ** Channel configuration structure **
// =====================================

// ** TCP/IP

typedef struct CFG_TCP_T
{
	unsigned char		ucConnMode,
						ucConnType,
						ucFlags;

	// Flag definitions
	// 0 0 0 0 0 0 0 0
	// | | |_|_|_|_|_|__________________________________________________ For furture use
	// | |______________________________________________________________ Tunnel flag - 0: no mark connection path, 1: mark connection path
	// |________________________________________________________________ Drop line flag - 0: dont drop line after failure, 1: drop line after failure

	char				acID [MAX_TINY_BUFFER_SZ],
						acIP [MAX_SMALL_BUFFER_SZ];

	int					iLocPort,
						iRemPort,

						iMinWrk,
						iMaxWrk,

						iKeepAliveInterval,
						iKeepAliveRetry,

						iBufSize;

	unsigned long		ulConnTimeout,
						ulIdleTimeout,
						ulKillIdleWrk,
						ulPackTimeout;

	ARRAY_LIST			stTargets;

	CFG_CRYPTO			*pstCrypto;

	// For routing use only
	int					iTimeoutCnt;

	unsigned long		ulLastFailure;
}
CFG_TCP;

// ** PSTN

typedef struct CFG_PSTN_DIAL_T
{
	char				acDial [MAX_SMALL_BUFFER_SZ];
}
CFG_PSTN_DIAL;

typedef struct CFG_PSTN_T
{
	unsigned char		ucProtocol,
						ucConnType,
						ucFlags;

	// Flag definitions
	// 0 0 0 0 0 0 0 0
	// | | |_|_|_|_|_|__________________________________________________ For furture use
	// | |______________________________________________________________ Tunnel flag - 0: no mark connection path, 1: mark connection path
	// |________________________________________________________________ Drop line flag - 0: dont drop line after failure, 1: drop line after failure

	char				acID [MAX_TINY_BUFFER_SZ],
						acModem [MAX_TINY_BUFFER_SZ];

	unsigned long		ulConnTimeout,
						ulIdleTimeout,
						ulPackTimeout,

						ulSNRM_Timeout,
						ulRR_Timeout;

	int					iBufSize,
              iAttenuation;

	ARRAY_LIST			stDials,	// CFG_PSTN_DIAL
						stTargets;

	// For routing use only
	int					iTimeoutCnt;

	unsigned long		ulLastFailure;
}
CFG_PSTN;

// ** RS232

typedef struct CFG_RS232_T
{
	unsigned char		ucType,
						ucFlags;

	// Flag definitions
	// 0 0 0 0 0 0 0 0
	// | |_|_|_|_|_|_|__________________________________________________ For furture use
	// |________________________________________________________________ Tunnel flag - 0: no mark connection path, 1: mark connection path

	char				acID [MAX_TINY_BUFFER_SZ];

	unsigned long		ulPackTimeout;

	int					iBufSize;

	ARRAY_LIST			stTargets;

	// For routing use only
	int					iTimeoutCnt;

	unsigned long		ulLastFailure;
}
CFG_RS232;

// ** RS485

typedef struct CFG_RS485_T
{
	unsigned char		ucType,
						ucRawLog,
						ucFlags;

	// Flag definitions
	// 0 0 0 0 0 0 0 0
	// |_|_|_|_|_|_|_|__________________________________________________ For furture use

	char				acID [MAX_TINY_BUFFER_SZ];

	unsigned long		ulPackTimeout;

	int					iBufSize;

	ARRAY_LIST			stTargets;

	// For routing use only
	int					iTimeoutCnt;

	unsigned long		ulLastFailure;
}
CFG_RS485;

typedef struct CFG_CHANNELS_T
{
	ARRAY_LIST			stTCPs,		// CFG_TCP
						stPSTNs,	// CFG_PSTN
						stRS232s,	// CFG_RS232
						stRS485s;	// CFG_RS485
}
CFG_CHANNELS;

// ====================================================



// =====================================
// ** Pattern configuration structure **
// =====================================

typedef struct CFG_DEST_T
{
	unsigned char		ucType,
						ucWeight,
						ucProtocolFmt,
						ucProtocolLen;

	char				acID [MAX_TINY_BUFFER_SZ];

	int					iCongested,
						iThreshold;

	unsigned long		ulCooldown;

	// For routing use only
	unsigned long		ulPackCnt;

	double				dblRatio;

	CFG_TCP				*pstTCP;

	CFG_PSTN			*pstPSTN;

	CFG_RS232			*pstRS232;

	CFG_RS485			*pstRS485;
}
CFG_DEST;

typedef struct CFG_PARSER_T
{
	unsigned char		ucType,
						ucFlags,
						ucProtocolFmt,
						ucProtocolLen;

	// Flag definitions
	// 0 0 0 0 0 0 0 0
	// | |_|_|_|_|_|_|__________________________________________________ For furture use
	// |________________________________________________________________ Stamp TPDU flag - 0: no modify TPDU, 1: Stamp an unique number in TPDU

	char				acID [MAX_TINY_BUFFER_SZ],
						acDefFile [MAX_SMALL_BUFFER_SZ];

	ISO8583				stISO8583;
}
CFG_PARSER;

typedef struct CFG_ROUTE_T
{
	unsigned char		ucFlags;

	// Flag definitions
	// 0 0 0 0 0 0 0 0
	// | |_|_|_|_|_|_|__________________________________________________ For furture use
	// |________________________________________________________________ Self route - 0: not allow echo packet, 1: allow echo packet

	char				acKey [MAX_SMALL_BUFFER_SZ],
						acVal [MAX_SMALL_BUFFER_SZ],
						acCoRel [MAX_SMALL_BUFFER_SZ],
						acGenCoRel [MAX_SMALL_BUFFER_SZ];

	ARRAY_LIST			stDests;
}
CFG_ROUTE;

typedef struct CFG_PATTERN_T
{
	char				acID [MAX_TINY_BUFFER_SZ];

	unsigned char		aucPtn [MAX_SMALL_BUFFER_SZ];

	int					iPtnLen;

	CFG_PARSER			stParser;

	ARRAY_LIST			stRoutes;
}
CFG_PATTERN;

// ====================================================



// ========================================
// ** Dependency configuration structure **
// ========================================

typedef struct CFG_DEPEND_T
{
	char				acID [MAX_TINY_BUFFER_SZ],
						acDepend [MAX_TINY_BUFFER_SZ];

	unsigned char		ucCmp,
						ucType;
}
CFG_DEPEND;


// ========================================
// **  Diagnose configuration structure  **
// ========================================

typedef struct CFG_DIAGNOSE_T
{
	char			acWanPingIP [MAX_SMALL_BUFFER_SZ];

  unsigned long   ulRetryInterval;

  int             iRetryThreshold;

	unsigned char		ucEnable;
}
CFG_DIAGNOSE;


typedef struct CFG_CORE_ENGINE_T
{
	CFG_LOGGER_VOICE			stLogger;

	CFG_MODULES			stModules;

	CFG_CHANNELS		stChannels;

	ARRAY_LIST			stPatterns,
						stCryptos,
						stDepends;

  CFG_DIAGNOSE    stDiagnose;

}
CFG_CORE_ENGINE;


unsigned char xml_config_init (char *p_pcPath, CFG_CORE_ENGINE *p_pstCfg);
unsigned char xml_config_term (CFG_CORE_ENGINE *p_pstCfg);

unsigned char xml_config_logger (stXMLParserMainData *p_pstXML, CFG_CORE_ENGINE *p_pstCfg);
unsigned char xml_config_module (stXMLParserMainData *p_pstXML, CFG_CORE_ENGINE *p_pstCfg);
unsigned char xml_config_channel (stXMLParserMainData *p_pstXML, CFG_CORE_ENGINE *p_pstCfg);
unsigned char xml_config_pattern (stXMLParserMainData *p_pstXML, CFG_CORE_ENGINE *p_pstCfg);
unsigned char xml_config_crypto (stXMLParserMainData *p_pstXML, CFG_CORE_ENGINE *p_pstCfg);
unsigned char xml_config_depend (stXMLParserMainData *p_pstXML, CFG_CORE_ENGINE *p_pstCfg);
unsigned char xml_config_diagnose (stXMLParserMainData *p_pstXML, CFG_CORE_ENGINE *p_pstCfg);


void xml_get_channel (CFG_CORE_ENGINE *p_pstCfg, char *p_pcID, CFG_TCP **p_ppstTCP, CFG_PSTN **p_ppstPSTN, CFG_RS232 **p_ppstRS232, CFG_RS485 **p_ppstRS485);

#endif
