#ifndef CISO8583_H
#define CISO8583_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "GTLV.h"
#include "CUtility.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ISO_8583			"ISO8583"
#define ISO_B24				"ISOB24"

#define I_ISO_8583			1
#define I_ISO_B24			2

// ISO 8583 Field Types
#define TYPE_A				0
#define TYPE_AN				1
#define TYPE_ANS			2
#define TYPE_N				3
#define TYPE_B				4
#define TYPE_Z				5
#define TYPE_XN				6
#define TYPE_UNKNOWN		-1

#define TYPE_A_NAME			"a"
#define TYPE_AN_NAME		"an"
#define TYPE_ANS_NAME		"ans"
#define TYPE_N_NAME			"n"
#define TYPE_B_NAME			"b"
#define TYPE_Z_NAME			"z"
#define TYPE_XN_NAME		"xn"

#define DEFAULT_BITMAP_SZ	8


// XML Tags
#define ISO_END				"</ISO8583>"

#define ISO_STANDARD		"standard"
#define ISO_HEADER			"header"
#define ISO_ID				"bit"
#define ISO_TYPE			"type"
#define ISO_SIZE			"size"
#define ISO_FIXED			"fixed"
#define ISO_LEN				"len"
#define ISO_PAD_POS			"ppos"
#define ISO_PAD_CHAR		"pchar"


// Error Codes
#define	E_FAIL_OPEN_FILE	81	// Failed to Open File
#define	E_EMPTY_MESSAGE		82	// Empty Message
#define	E_LEN_TOO_SHORT		83	// Length Too Short
#define	E_EMPTY_CONTAINER	84	// Empty Data Container
#define	E_INVALID_ISO_FIELD	85	// Invalid ISO Field
#define	E_UNUSED_DATA_FOUND	86	// Unused Data Found
#define	E_INSUF_DATA		87	// Insufficient Data
#define	E_MISSING_DATA		88	// Missing Data
#define	E_MISSING_FIELD_DEF	89	// Missing Field Definition
#define	E_INVALID_PARAM		90	// Invalid Parameter
#define	E_OUT_MEMORY		91	// Out Of Memory


// Constant Definitions
#define MAX_TEMP_BUFFER_SZ			1024
#define MAX_XML_BUFFER_SZ			40960
#define MAX_ISO_MESG_SZ				2048
#define MAX_ISO_FIELDS				128
#define MAX_FIELD_LEN				4

// Common Data Fields
#define TLV_BIT_MAP_EXT				1	// Bit Map, Secondary
#define TLV_CARD_PAN				2	// Primary Account Number (PAN)
#define TLV_PROC_CODE				3	// Processing Code
#define TLV_TXN_AMT					4	// Amount, Transaction
#define TLV_SETTLE_AMT				5	// Amount, Settlement
#define TLV_CH_BILL_AMT				6	// Amount, Cardholder Billing
#define TLV_TRANS_DATE_TIME			7	// Transmission Date & Time
#define TLV_CH_BILL_FEE_AMT			8	// Amount, Cardholder Billing Fee
#define TLV_SETTLE_CONV_RATE		9	// Conversion Rate, Settlement
#define TLV_CH_BILL_CONV_RATE		10	// Conversion Rate, Cardholder Billing
#define TLV_STAN					11	// Systems Trace Audit Number
#define TLV_TXN_TIME				12	// Time, Local Transaction (hhmmss)
#define TLV_TXN_DATE				13	// Date, Local Transaction (mmdd)
#define TLV_EXP_DATE				14	// Date, Expiration (yymm)
#define TLV_SETTLE_DATE				15	// Date, Settlement (mmdd)
#define TLV_CONV_DATE				16	// Date, Conversion (mmdd)
#define TLV_CAPTURE_DATE			17	// Date, Capture (mmdd)
#define TLV_MERCH_TYPE				18	// Merchant Type
#define TLV_ACQ_INST_CTRY_CODE		19	// Acquiring Institution, Country Code
#define TLV_PAN_EXT_CTRY_CODE		20	// PAN Extended, Country Code
#define TLV_FWD_INST_CTRY_CODE		21	// Forwarding Institution, Country Code
#define TLV_POS_ENTRY_MODE			22	// Point of Service Entry Mode
#define TLV_APP_PAN_NUM				23	// Application PAN Number
#define TLV_NETWORK_INT_ID			24	// Network International Identifier
#define TLV_POS_COND_CODE			25	// Point of Service Condition Code
#define TLV_POS_CAPTURE_CODE		26	// Point of Service Capture Code
#define TLV_AUTH_ID_RESP_LEN		27	// Authorising Identification Response Length
#define TLV_TXN_FEE_AMT				28	// Amount, Transaction Fee
#define TLV_SETTLE_FEE_AMT			29	// Amount, Settlement Fee
#define TLV_TXN_PROC_FEE_AMT		30	// Amount, Transaction Processing Fee
#define TLV_SETTLE_PROC_FEE_AMT		31	// Amount, Settlement Processing Fee
#define TLV_ACQ_INST_ID_CODE		32	// Acquiring Institution Identification Code
#define TLV_FWD_INST_ID_CODE		33	// Forward Institution ID COde
#define TLV_PAN_EXT					34	// Primary Account Number, Extended
#define TLV_TRACK2_DATA				35	// Track 2 Data
#define TLV_TRACK3_DATA				36	// Track 3 Data
#define TLV_RET_REF_NUM				37	// Retrieval Reference Number
#define TLV_AUTH_ID_RESP			38	// Authorization Identification Response
#define TLV_RESP_CODE				39	// Response Code
#define TLV_SVC_RESTRICT_CODE		40	// Service Restriction Code
#define TLV_CARD_ACC_TERM_ID		41	// Card Acceptor Terminal Identification
#define TLV_CARD_ACC_ID_CODE		42	// Card Acceptor Identification Code
#define TLV_CARD_ACC_NAME_LOC		43	// Card Acceptor Name/Location
#define TLV_ADDL_RESP_DATA			44	// Additional Response Data
#define TLV_TRACK1_DATA				45	// Track 1 Data
#define TLV_ADDL_DATA_ISO			46	// Additional Data - ISO
#define TLV_ADDL_DATA_NATIONAL		47	// Additional Data - National
#define TLV_ADDL_DATA_PRIV			48	// Additional Data - Private
#define TLV_TXN_CURR_CODE			49	// Currency Code, Transaction
#define TLV_SETTLE_CURR_CODE		50	// Currency Code, Settlement
#define TLV_CH_BILL_CURR_CODE		51	// Currency Code, Cardholder Billing
#define TLV_CARDHOLDER_PIN			52	// Personal Identification Number Data
#define TLV_SEC_REL_CTRL_INFO		53	// Security Related Control Information
#define TLV_ADDL_AMT				54	// Additional Amounts
#define TLV_RESERVED_ISO_1			55	// Reserved ISO
#define TLV_RESERVED_ISO_2			56	// Reserved ISO
#define TLV_RESERVED_NATIONAL_1		57	// Reserved National
#define TLV_RESERVED_NATIONAL_2		58	// Reserved National
#define TLV_RESERVED_NATIONAL_3		59	// Reserved for National Use
#define TLV_RESERVED_PRIV_1			60	// Advice/Reason Code (Private Reserved) (Terminal Data)
#define TLV_RESERVED_PRIV_2			61	// Reserved Private (Card Issuer-Category-Response Code Data)
#define TLV_RESERVED_PRIV_3			62	// Reserved Private (Postal Code)
#define TLV_RESERVED_PRIV_4			63	// Reserved Private (Additional Data)
#define TLV_MAC						64	// Message Authentication Code (MAC)

#define TLV_BIT_MAP_TERTIARY		65	// Bit Map, Tertiary
#define TLV_SETTLE_CODE				66	// Settlement Code
#define TLV_EXT_PAY_CODE			67	// Extended Payment Code
#define TLV_RECV_INST_CTRY_CODE		68	// Receiving Institution Country Code
#define TLV_SETTLE_INST_CTRY_CODE	69	// Settlement Institution Country Code
#define TLV_NET_MGMT_INFO_CODE		70	// Network Management Information Code
#define TLV_MESG_NUM				71	// Message Number
#define TLV_MESG_NUM_LAST			72	// Message Number, Last
#define TLV_ACTION_DATE				73	// Date, Action (yymmdd)
#define TLV_CREDIT_NUM				74	// Credits, Number
#define TLV_CREDIT_REV_NUM			75	// Credits, Reversal Number
#define TLV_DEBIT_NUM				76	// Debits, Number
#define TLV_DEBIT_REV_NUM			77	// Debits, Reversal Number
#define TLV_TRANSFER_NUM			78	// Transfer, Number
#define TLV_TRANSFER_REV_NUM		79	// Transfer, Reversal Number
#define TLV_INQUIRIES_NUM			80	// Inquiries, Number
#define TLV_AUTH_NUM				81	// Authorisations, Number
#define TLV_CREDIT_PROC_FEE_AMT		82	// Credits, Processing Fee Amount
#define TLV_CREDIT_TXN_FEE_AMT		83	// Credits, Transaction Fee Amount
#define TLV_DEBIT_PROC_FEE_AMT		84	// Debits, Processing Fee Amount
#define TLV_DEBIT_TXN_FEE_AMT		85	// Debits, Transaction Fee Amount
#define TLV_CREDIT_AMT				86	// Credits, Amount
#define TLV_CREDIT_REV_AMT			87	// Credits, Reversal Amount
#define TLV_DEBIT_AMT				88	// Debits, Amount
#define TLV_DEBIT_REV_AMT			89	// Debits, Reversal Amount
#define TLV_ORIGINAL_DATA_ELEMENT	90	// Original Data Elements
#define TLV_FILE_UPDATE_CODE		91	// File Update Code
#define TLV_FILE_SEC_CODE			92	// File Security Code
#define TLV_RESP_INDICATOR			93	// Response Indicator
#define TLV_SVC_INDICATOR			94	// Service Indicator
#define TLV_REPLACEMENT_AMT			95	// Replacement Amounts
#define TLV_MESG_SEC_CODE			96	// Message Security Code
#define TLV_NET_SETTLE_AMT			97	// Amount, Net Settlement
#define TLV_PAYEE					98	// Payee
#define TLV_SETTLE_INST_ID_CODE		99	// Settlement Institution Identification Code
#define TLV_RECV_INST_ID_CODE		100	// Receiving Institution Identification Code
#define TLV_FILE_NAME				101	// File Name
#define TLV_ACCT_ID_1				102	// Account Identification 1
#define TLV_ACCT_ID_2				103	// Account Identification 2
#define TLV_TXN_DESC				104	// Transaction Description
#define TLV_RESERVED_ISO_3			105	// Reserved for ISO Use
#define TLV_RESERVED_ISO_4			106	// Reserved for ISO Use
#define TLV_RESERVED_ISO_5			107	// Reserved for ISO Use
#define TLV_RESERVED_ISO_6			108	// Reserved for ISO Use
#define TLV_RESERVED_ISO_7			109	// Reserved for ISO Use
#define TLV_RESERVED_ISO_8			110	// Reserved for ISO Use
#define TLV_RESERVED_ISO_9			111	// Reserved for ISO Use
#define TLV_RESERVED_NATIONAL_4		112	// Reserved for National Use
#define TLV_AUTH_AGT_INST_ID_CODE	113	// Authorising Agent Institution ID Code
#define TLV_RESERVED_NATIONAL_5		114	// Reserved for National Use
#define TLV_RESERVED_NATIONAL_6		115	// Reserved for National Use
#define TLV_RESERVED_NATIONAL_7		116	// Reserved for National Use
#define TLV_RESERVED_NATIONAL_8		117	// Reserved for National Use
#define TLV_RESERVED_NATIONAL_9		118	// Reserved for National Use
#define TLV_RESERVED_NATIONAL_10	119	// Reserved for National Use
#define TLV_RESERVED_PRIV_5			120	// Reserved for Private Use (Key Management / Terminal Address-Branch)
#define TLV_RESERVED_PRIV_6			121	// Reserved for Private Use (Auth Indicators-CRT Authorization Data)
#define TLV_RESERVED_PRIV_7			122	// Reserved for Private Use (Card Issuer Identification Code)
#define TLV_RESERVED_PRIV_8			123	// Reserved for Private Use (Cryptographic Service Message / Invoice Data)
#define TLV_INFO_TEXT				124	// Info Text / Batch and Shift Data
#define TLV_NET_MGMT_INFO			125	// Network Management Information / Settlement Data
#define TLV_ISSUER_TRACE_ID			126	// Issuer Trace ID / Pre-Auth and Chargeback Data
#define TLV_RESERVED_PRIV_9			127	// Reserved for Private Use (User Data)
#define TLV_MAC_2					128	// Message Authentication Code

#define TLV_TPDU					201	// TPDU
#define TLV_MTI						202	// MTI
#define TLV_RESP_MTI				203	// Response MTI
#define TLV_TXN_ID					204	// Transaction ID
#define TLV_PKID					205	// PKID
#define TLV_BATCH_ID				206	// Batch ID
#define TLV_DATA_TYPE				207	// Data Type
#define TLV_DATA_LEN				208	// Data Len
#define TLV_REQ_DATA				209	// Request Data
#define TLV_REQ_DATA_2				210	// Request Data 2
#define TLV_RES_DATA				211	// Response Data
#define TLV_RES_DATA_2				212	// Response Data 2
#define TLV_HOST_ID					213	// Host ID
#define TLV_TERM_MODEL				214	// Terminal Model
#define TLV_CUSTOMER_NAME			215	// Customer Name
#define TLV_MERCHANT_NAME			216	// Merchant Name
#define TLV_TXN_REF_ID				217	// Transaction Reference ID
#define TLV_TXN_STATUS_1			218	// Transaction Status 1
#define TLV_TXN_STATUS_2			219	// Transaction Status 2
#define TLV_TXN_TYPE				220	// Transaction Type
#define TLV_TXN_SUB_TYPE			221	// Transaction Sub Type
#define TLV_PTR_1					222	// Pointer 1
#define TLV_PTR_2					223	// Pointer 2
#define TLV_PTR_3					224	// Pointer 3
#define TLV_DATA_REF_ID_1			225 // Data Reference ID 1
#define TLV_DATA_REF_ID_2			226	// Data Reference ID 2
#define TLV_DATA_REF_ID_3			227	// Data Reference ID 3
#define TLV_DATA_REF_ID_4			228	// Data Reference ID 4
#define TLV_DATA_REF_ID_5			229	// Data Reference ID 5

#define TLV_KEY_VALUE				251	// Key Value
#define TLV_KEY_COUNT				252	// Key Counter
#define TLV_CHECK_DIGIT				253	// Check Digit
#define TLV_MAC_DATA				254	// MAC Data


// Marcos Definitions
#define FILLFIELD(a,p,q,r,s,t,u,v)	a[p - 1].iID = p; \
										a[p - 1].iType = q; \
										a[p - 1].iSize = r; \
										a[p - 1].iLen = s; \
										a[p - 1].iFixed = t; \
										a[p - 1].cPadPos = u; \
										a[p - 1].cPadChar = v;

#define ISOERR(a,b)						{ \
												ucRet = a;	\
												p_pcErrMsg ? strcpy (p_pcErrMsg, b) : 0; \
												goto CleanUp; \
										}


// Structures Definitions
typedef struct ISO_FIELD_T
{
	int			iID,
				iType,
				iSize,
				iLen,
				iFixed;

	char		cPadPos,
				cPadChar;
}
ISO_FIELD;

typedef struct ISO8583_T
{
	ISO_FIELD					astISODef [MAX_ISO_FIELDS];

	int							iStandard,
								iHeader,
								iMTILen;
}
ISO8583;


// == Public functions ==
unsigned char iso8583_init (ISO8583 *p_pstISO, char *p_pcFileName);
void iso8583_term (ISO8583 *p_pstISO8583);
unsigned char iso8583_parse (ISO8583 *p_pstISO, unsigned char *p_pucInput, int p_iLen, int p_iPos, GTLV_DATA *p_pstTLV, char *p_pcErrMsg);
unsigned char iso8583_build (ISO8583 *p_pstISO, unsigned char *p_pucOutput, int p_iSize, int *p_piLen, int p_iPos, GTLV_DATA *p_pstTLV, char *p_pcErrMsg);


// == Private internal use ==
void iso8583_gen_bitmap (ISO8583 *p_pstISO, unsigned char *p_pucBitmap, int *p_piLen, GTLV_DATA *p_pstTLV);
unsigned char iso8583_extract_field (ISO8583 *p_pstISO, int p_iID, unsigned char *p_pucInput, int p_iLen, int *p_piIdx, unsigned char *p_pucOutput,
									 int *p_piLen, char *p_pcErrMsg);
unsigned char iso8583_build_field (ISO8583 *p_pstISO, int p_iID, unsigned char *p_pucInput, int p_iLen, unsigned char *p_pucOutput, int *p_piIdx, char *p_pcErrMsg);

unsigned char iso8583_get_xml_attr_value (char *p_pcBuf, int *p_piIdx, char *p_pcField, char *p_pcEndStr, char *p_pcValue, int *p_piLen);

#ifdef __cplusplus 
}
#endif 

#endif
