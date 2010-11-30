#include "CISO8583.h"

unsigned char iso8583_init (ISO8583 *p_pstISO, char *p_pcFileName)
{
	unsigned char	ucRet,
					ucFRet;	

	unsigned long	ulRead,
					ulSize;

	char			acVal [512],
					*pcBuf			= 0;

	int				iIdx			= 0,
					iID,
					iType,
					iLen,
					iBufLen			= sizeof (acVal);

	FILE			*pfdConfig		= 0;

	memset (p_pstISO, 0x00, sizeof (ISO8583));

	if (0 != p_pcFileName)
	{
		if (0 != *p_pcFileName)
		{
			// Open the XML file passed in from the input parameter.
			// Return false if the file is not found or corrupted.
			// Read all the file content and copy to a local buffer.
			// Close the file and free the handle.

			FOPEN (pfdConfig, p_pcFileName, "rb");
			
			if (0 == pfdConfig)
			{
				ucRet = E_FAIL_OPEN_FILE;
				goto CleanUp;
			}

			fseek (pfdConfig, 0, SEEK_END);
			ulSize = ftell (pfdConfig);
		
			MEM_ALLOC (pcBuf, ulSize + 10, (char *));
		
			if (0 == pcBuf)
			{
				ucRet = E_OUT_MEMORY;
				goto CleanUp;
			}
			else
			{
				rewind (pfdConfig);
				ulRead = fread (pcBuf, sizeof (char), ulSize, pfdConfig);
				*(pcBuf + ulSize) = 0;
			}

			FCLOSE (pfdConfig);
			
			iIdx = 0;

			// Parse the buffer and get the length of each part in the ISO 8583 message.
			p_pstISO->iStandard = I_ISO_8583;
			p_pstISO->iHeader = 5;
			p_pstISO->iMTILen = 2;
			
			iLen = iBufLen;
			ucFRet = iso8583_get_xml_attr_value (pcBuf, &iIdx, ISO_STANDARD, NULL, acVal, &iLen);
			
			if (0 == ucFRet)
			{
				if (0 == strcmp (ISO_8583, acVal))
				{
					p_pstISO->iStandard = I_ISO_8583;
				}
				else if (0 == strcmp (ISO_B24, acVal))
				{
					p_pstISO->iStandard = I_ISO_B24;
				}
			}

			iLen = iBufLen;
			ucFRet = iso8583_get_xml_attr_value (pcBuf, &iIdx, ISO_HEADER, NULL, acVal, &iLen);
			p_pstISO->iHeader = (0 == ucFRet) ? atoi (acVal) : 0;

			iIdx = 0;

			// Read all the available ISO 8583 Fields information from the buffer.
			// Add these field information into the Object.
			while (1) 
			{
				iLen = iBufLen;
				ucFRet = iso8583_get_xml_attr_value (pcBuf, &iIdx, ISO_ID, NULL, acVal, &iLen);
				
				if (0 == ucFRet) 
				{
					iID = atoi (acVal) - 1;
					p_pstISO->astISODef [iID].iID = iID + 1;
				}
				else
				{
					break;
				}

				iLen = iBufLen;
				ucFRet = iso8583_get_xml_attr_value (pcBuf, &iIdx, ISO_TYPE, NULL, acVal, &iLen);
				
				if (0 == ucFRet) 
				{
					if (0 == strcmp (TYPE_A_NAME, acVal))
						iType = TYPE_A;
					else if (0 == strcmp (TYPE_AN_NAME, acVal))
						iType = TYPE_AN;
					else if (0 == strcmp (TYPE_ANS_NAME, acVal))
						iType = TYPE_ANS;
					else if (0 == strcmp (TYPE_N_NAME, acVal))
						iType = TYPE_N;
					else if (0 == strcmp (TYPE_Z_NAME, acVal))
						iType = TYPE_Z;
					else if (0 == strcmp (TYPE_B_NAME, acVal))
						iType = TYPE_B;
					else if (0 == strcmp (TYPE_XN_NAME, acVal))
						iType = TYPE_XN;
					else
						iType = TYPE_UNKNOWN;
		
					p_pstISO->astISODef [iID].iType = iType;
				}
		
				iLen = iBufLen;
				ucFRet = iso8583_get_xml_attr_value (pcBuf, &iIdx, ISO_SIZE, NULL, acVal, &iLen);

				if (TYPE_A == p_pstISO->astISODef [iID].iType || TYPE_AN == p_pstISO->astISODef [iID].iType || TYPE_ANS == p_pstISO->astISODef [iID].iType)
				{
					p_pstISO->astISODef [iID].iSize = (0 == ucFRet) ? atoi (acVal) : 0;
				}
				else
				{
					p_pstISO->astISODef [iID].iSize = (0 == ucFRet) ? atoi (acVal) : 0;

					if (0 == p_pstISO->astISODef [iID].iSize % 2)
					{
						p_pstISO->astISODef [iID].iSize = p_pstISO->astISODef [iID].iSize / 2;
					}
					else
					{
						p_pstISO->astISODef [iID].iSize = (p_pstISO->astISODef [iID].iSize / 2) + 1;
					}
				}

				iLen = iBufLen;
				ucFRet = iso8583_get_xml_attr_value (pcBuf, &iIdx, ISO_FIXED, NULL, acVal, &iLen);
				p_pstISO->astISODef [iID].iFixed = (0 == ucFRet) ? ((0 == strcmp ("1", acVal)) ? 1 : 0) : 1;
		
				iLen = iBufLen;
				ucFRet = iso8583_get_xml_attr_value (pcBuf, &iIdx, ISO_LEN, NULL, acVal, &iLen);
				p_pstISO->astISODef [iID].iLen = (0 == ucFRet) ? atoi (acVal) : 0;
		
				iLen = iBufLen;
				ucFRet = iso8583_get_xml_attr_value (pcBuf, &iIdx, ISO_PAD_POS, NULL, acVal, &iLen);
				p_pstISO->astISODef [iID].cPadPos = (0 == ucFRet) ? acVal[0] : 0;
		
				iLen = iBufLen;
				ucFRet = iso8583_get_xml_attr_value (pcBuf, &iIdx, ISO_PAD_CHAR, NULL, acVal, &iLen);
				p_pstISO->astISODef [iID].cPadChar = (0 == ucFRet) ? acVal[0] : 0;
			}
		}
	}

	if (I_ISO_8583 != p_pstISO->iStandard && I_ISO_B24 != p_pstISO->iStandard)
	{
		p_pstISO->iStandard = I_ISO_8583;
		p_pstISO->iHeader = 5;
		p_pstISO->iMTILen = 2;

		FILLFIELD (p_pstISO->astISODef, TLV_BIT_MAP_EXT, TYPE_B, 32, 0, 1, 'L', '0');					// Bit Map Extended
		FILLFIELD (p_pstISO->astISODef, TLV_CARD_PAN, TYPE_N, 10, 1, 0, 'R', '0');					// Primary Account Number (PAN)
		FILLFIELD (p_pstISO->astISODef, TLV_PROC_CODE, TYPE_N, 3, 0, 1, 'R', '0');					// Processing Code
		FILLFIELD (p_pstISO->astISODef, TLV_TXN_AMT, TYPE_N, 6, 0, 1, 'L', '0');						// Amount, Transaction
		FILLFIELD (p_pstISO->astISODef, TLV_SETTLE_AMT, TYPE_N, 6, 0, 1, 'L', '0');					// Amount, Settlement
		FILLFIELD (p_pstISO->astISODef, TLV_CH_BILL_AMT, TYPE_N, 6, 0, 1, 'L', '0');					// Amount, Cardholder Billing
		FILLFIELD (p_pstISO->astISODef, TLV_TRANS_DATE_TIME, TYPE_N, 5, 0, 1, 'L', '0');				// Transmission Date & Time
		FILLFIELD (p_pstISO->astISODef, TLV_CH_BILL_FEE_AMT, TYPE_N, 4, 0, 1, 'L', '0');				// Amount, Cardholder Billing Fee
		FILLFIELD (p_pstISO->astISODef, TLV_SETTLE_CONV_RATE, TYPE_N, 4, 0, 1, 'L', '0');				// Conversion Rate, Settlement
		FILLFIELD (p_pstISO->astISODef, TLV_CH_BILL_CONV_RATE, TYPE_N, 4, 0, 1, 'L', '0');			// Conversion Rate, Cardholder Billing

		FILLFIELD (p_pstISO->astISODef, TLV_STAN, TYPE_N, 3, 0, 1, 'L', '0');							// Systems Trace Audit Number
		FILLFIELD (p_pstISO->astISODef, TLV_TXN_TIME, TYPE_N, 3, 0, 1, 'L', '0');						// Time, Local Transaction (hhmmss)
		FILLFIELD (p_pstISO->astISODef, TLV_TXN_DATE, TYPE_N, 2, 0, 1, 'L', '0');						// Date, Local Transaction (mmdd)
		FILLFIELD (p_pstISO->astISODef, TLV_EXP_DATE, TYPE_N, 2, 0, 1, 'L', '0');						// Date, Expiration (yymm)
		FILLFIELD (p_pstISO->astISODef, TLV_SETTLE_DATE, TYPE_N, 2, 0, 1, 'L', '0');					// Date, Settlement (mmdd)
		FILLFIELD (p_pstISO->astISODef, TLV_CONV_DATE, TYPE_N, 2, 0, 1, 'L', '0');					// Date, Conversion (mmdd)
		FILLFIELD (p_pstISO->astISODef, TLV_CAPTURE_DATE, TYPE_N, 2, 0, 1, 'L', '0');					// Date, Capture (mmdd)
		FILLFIELD (p_pstISO->astISODef, TLV_MERCH_TYPE, TYPE_N, 2, 0, 1, 'L', '0');					// Merchant Type
		FILLFIELD (p_pstISO->astISODef, TLV_ACQ_INST_CTRY_CODE, TYPE_N, 2, 0, 1, 'L', '0');			// Acquiring Institution Country Code
		FILLFIELD (p_pstISO->astISODef, TLV_PAN_EXT_CTRY_CODE, TYPE_N, 2, 0, 1, 'L', '0');			// PAN Extended, Country Code

		FILLFIELD (p_pstISO->astISODef, TLV_FWD_INST_CTRY_CODE, TYPE_N, 2, 0, 1, 'L', '0');			// Forwarding Institution, Country Code
		FILLFIELD (p_pstISO->astISODef, TLV_POS_ENTRY_MODE, TYPE_N, 2, 0, 1, 'L', '0');				// Point of Service Entry Mode
		FILLFIELD (p_pstISO->astISODef, TLV_APP_PAN_NUM, TYPE_N, 2, 0, 1, 'L', '0');					// Application PAN Number
		FILLFIELD (p_pstISO->astISODef, TLV_NETWORK_INT_ID, TYPE_N, 2, 0, 1, 'L', '0');				// Network International Identifier
		FILLFIELD (p_pstISO->astISODef, TLV_POS_COND_CODE, TYPE_N, 1, 0, 1, 'L', '0');				// Point of Service Condition Code
		FILLFIELD (p_pstISO->astISODef, TLV_POS_CAPTURE_CODE, TYPE_N, 1, 0, 1, 'L', '0');				// Point of Service Capture Code
		FILLFIELD (p_pstISO->astISODef, TLV_AUTH_ID_RESP_LEN, TYPE_N, 1, 0, 1, 'L', '0');				// Authorising Identification Response Length
		FILLFIELD (p_pstISO->astISODef, TLV_TXN_FEE_AMT, TYPE_AN, 9, 0, 1, 'L', '0');					// Amount, Transaction Fee
		FILLFIELD (p_pstISO->astISODef, TLV_SETTLE_FEE_AMT, TYPE_AN, 9, 0, 1, 'L', '0');				// Amount, Settlement Fee
		FILLFIELD (p_pstISO->astISODef, TLV_TXN_PROC_FEE_AMT, TYPE_AN, 9, 0, 1, 'L', '0');			// Amount, Transaction Processing Fee

		FILLFIELD (p_pstISO->astISODef, TLV_SETTLE_PROC_FEE_AMT, TYPE_AN, 9, 0, 1, 'L', '0');			// Amount, Settlement Processing Fee
		FILLFIELD (p_pstISO->astISODef, TLV_ACQ_INST_ID_CODE, TYPE_N, 6, 1, 0, 'L', '0');			// Acquiring Institution Identification Code
		FILLFIELD (p_pstISO->astISODef, TLV_FWD_INST_ID_CODE, TYPE_N, 6, 1, 0, 'L', '0');			// Forwarding Institution Identification Code
		FILLFIELD (p_pstISO->astISODef, TLV_PAN_EXT, TYPE_ANS, 28, 1, 0, 'L', '0');					// Primary Account Number, Extended
		FILLFIELD (p_pstISO->astISODef, TLV_TRACK2_DATA, TYPE_Z, 19, 1, 0, 'R', '0');					// Track 2 Data
		FILLFIELD (p_pstISO->astISODef, TLV_TRACK3_DATA, TYPE_ANS, 104, 2, 0, 'R', '0');				// Track 3 Data
		FILLFIELD (p_pstISO->astISODef, TLV_RET_REF_NUM, TYPE_AN, 12, 0, 1, 'L', '0');				// Retrieval Reference Number
		FILLFIELD (p_pstISO->astISODef, TLV_AUTH_ID_RESP, TYPE_AN, 6, 0, 1, 'L', '0');				// Authorization Identification Response
		FILLFIELD (p_pstISO->astISODef, TLV_RESP_CODE, TYPE_AN, 2, 0, 1, 'L', '0');					// Response Code
		FILLFIELD (p_pstISO->astISODef, TLV_SVC_RESTRICT_CODE, TYPE_AN, 3, 0, 1, 'L', '0');			// Service Restriction Code

		FILLFIELD (p_pstISO->astISODef, TLV_CARD_ACC_TERM_ID, TYPE_ANS, 8, 0, 1, 'L', '0');			// Card Acceptor Terminal Identification
		FILLFIELD (p_pstISO->astISODef, TLV_CARD_ACC_ID_CODE, TYPE_ANS, 15, 0, 1, 'L', '0');			// Card Acceptor Identification Code
		FILLFIELD (p_pstISO->astISODef, TLV_CARD_ACC_NAME_LOC, TYPE_ANS, 40, 0, 1, 'L', '0');			// Card Acceptor Name/Location
		FILLFIELD (p_pstISO->astISODef, TLV_ADDL_RESP_DATA, TYPE_ANS, 25, 1, 0, 'L', '0');			// Additional Response Data
		FILLFIELD (p_pstISO->astISODef, TLV_TRACK1_DATA, TYPE_ANS, 76, 1, 0, 'L', '0');				// Track 1 Data
		FILLFIELD (p_pstISO->astISODef, TLV_ADDL_DATA_ISO, TYPE_ANS, 999, 2, 0, 'L', '0');			// Additional Data - ISO
		FILLFIELD (p_pstISO->astISODef, TLV_ADDL_DATA_NATIONAL, TYPE_ANS, 999, 2, 0, 'L', '0');		// Additional Data - National
		FILLFIELD (p_pstISO->astISODef, TLV_ADDL_DATA_PRIV, TYPE_ANS, 999, 2, 0, 'L', '0');			// Additional Data - Private
		FILLFIELD (p_pstISO->astISODef, TLV_TXN_CURR_CODE, TYPE_N, 2, 0, 1, 'L', '0');				// Currency Code, Transaction
		FILLFIELD (p_pstISO->astISODef, TLV_SETTLE_CURR_CODE, TYPE_N, 2, 0, 1, 'L', '0');				// Currency Code, Settlement

		FILLFIELD (p_pstISO->astISODef, TLV_CH_BILL_CURR_CODE, TYPE_N, 2, 0, 1, 'L', '0');			// Currency Code, Cardholder Billing
		FILLFIELD (p_pstISO->astISODef, TLV_CARDHOLDER_PIN, TYPE_B, 32, 0, 1, 'L', '0');				// Personal Identification Number Data
		FILLFIELD (p_pstISO->astISODef, TLV_SEC_REL_CTRL_INFO, TYPE_N, 8, 0, 1, 'L', '0');			// Security Related Control Information
		FILLFIELD (p_pstISO->astISODef, TLV_ADDL_AMT, TYPE_AN, 120, 2, 0, 'L', '0');					// Additional Amounts
		FILLFIELD (p_pstISO->astISODef, TLV_RESERVED_ISO_1, TYPE_B, 128, 2, 0, 'L', '0');				// Reserved ISO
		FILLFIELD (p_pstISO->astISODef, TLV_RESERVED_ISO_2, TYPE_ANS, 999, 2, 0, 'L', '0');			// Reserved ISO
		FILLFIELD (p_pstISO->astISODef, TLV_RESERVED_NATIONAL_1, TYPE_ANS, 999, 2, 0, 'L', '0');		// Reserved National
		FILLFIELD (p_pstISO->astISODef, TLV_RESERVED_NATIONAL_2, TYPE_ANS, 999, 2, 0, 'L', '0');		// Reserved National
		FILLFIELD (p_pstISO->astISODef, TLV_RESERVED_NATIONAL_3, TYPE_ANS, 999, 2, 0, 'L', '0');		// Reserved for National Use
		FILLFIELD (p_pstISO->astISODef, TLV_RESERVED_PRIV_1, TYPE_ANS, 999, 2, 0, 'L', '0');			// Advice/Reason Code (Private Reserved) (Terminal Data)

		FILLFIELD (p_pstISO->astISODef, TLV_RESERVED_PRIV_2, TYPE_ANS, 999, 2, 0, 'L', '0');			// Reserved Private (Card Issuer-Category-Response Code Data)
		FILLFIELD (p_pstISO->astISODef, TLV_RESERVED_PRIV_3, TYPE_ANS, 999, 2, 0, 'L', '0');			// Reserved Private (Postal Code)
		FILLFIELD (p_pstISO->astISODef, TLV_RESERVED_PRIV_4, TYPE_ANS, 999, 2, 0, 'L', '0');			// Reserved Private (Additional Data)
		FILLFIELD (p_pstISO->astISODef, TLV_MAC, TYPE_B, 32, 0, 1, 'L', '0');							// Message Authentication Code (MAC)
		FILLFIELD (p_pstISO->astISODef, TLV_BIT_MAP_TERTIARY, TYPE_B, 32, 0, 1, 'L', '0');			// Bit Map, Tertiary
		FILLFIELD (p_pstISO->astISODef, TLV_SETTLE_CODE, TYPE_N, 1, 0, 1, 'L', '0');					// Settlement Code
		FILLFIELD (p_pstISO->astISODef, TLV_EXT_PAY_CODE, TYPE_N, 1, 0, 1, 'L', '0');					// Extended Payment Code
		FILLFIELD (p_pstISO->astISODef, TLV_RECV_INST_CTRY_CODE, TYPE_N, 2, 0, 1, 'L', '0');			// Receiving Institution Country Code
		FILLFIELD (p_pstISO->astISODef, TLV_SETTLE_INST_CTRY_CODE, TYPE_N, 2, 0, 1, 'L', '0');		// Settlement Institution Country Code
		FILLFIELD (p_pstISO->astISODef, TLV_NET_MGMT_INFO_CODE, TYPE_N, 2, 0, 1, 'L', '0');			// Network Management Information Code

		FILLFIELD (p_pstISO->astISODef, TLV_MESG_NUM, TYPE_N, 2, 0, 1, 'L', '0');						// Message Number
		FILLFIELD (p_pstISO->astISODef, TLV_MESG_NUM_LAST, TYPE_N, 2, 0, 1, 'L', '0');				// Message Number, Last
		FILLFIELD (p_pstISO->astISODef, TLV_ACTION_DATE, TYPE_N, 3, 0, 1, 'L', '0');					// Date, Action (yymmdd)
		FILLFIELD (p_pstISO->astISODef, TLV_CREDIT_NUM, TYPE_N, 5, 0, 1, 'L', '0');					// Credits, Number
		FILLFIELD (p_pstISO->astISODef, TLV_CREDIT_REV_NUM, TYPE_N, 5, 0, 1, 'L', '0');				// Credits, Reversal Number
		FILLFIELD (p_pstISO->astISODef, TLV_DEBIT_NUM, TYPE_N, 5, 0, 1, 'L', '0');					// Debits, Number
		FILLFIELD (p_pstISO->astISODef, TLV_DEBIT_REV_NUM, TYPE_N, 5, 0, 1, 'L', '0');				// Debits, Reversal Number
		FILLFIELD (p_pstISO->astISODef, TLV_TRANSFER_NUM, TYPE_N, 5, 0, 1, 'L', '0');				// Transfer, Number
		FILLFIELD (p_pstISO->astISODef, TLV_TRANSFER_REV_NUM, TYPE_N, 5, 0, 1, 'L', '0');			// Transfer, Reversal Number
		FILLFIELD (p_pstISO->astISODef, TLV_INQUIRIES_NUM, TYPE_N, 5, 0, 1, 'L', '0');				// Inquiries, Number

		FILLFIELD (p_pstISO->astISODef, TLV_AUTH_NUM, TYPE_N, 5, 0, 1, 'L', '0');					// Authorisations, Number
		FILLFIELD (p_pstISO->astISODef, TLV_CREDIT_PROC_FEE_AMT, TYPE_N, 6, 0, 1, 'L', '0');			// Credits, Processing Fee Amount
		FILLFIELD (p_pstISO->astISODef, TLV_CREDIT_TXN_FEE_AMT, TYPE_N, 6, 0, 1, 'L', '0');			// Credits, Transaction Fee Amount
		FILLFIELD (p_pstISO->astISODef, TLV_DEBIT_PROC_FEE_AMT, TYPE_N, 6, 0, 1, 'L', '0');			// Debits, Processing Fee Amount
		FILLFIELD (p_pstISO->astISODef, TLV_DEBIT_TXN_FEE_AMT, TYPE_N, 6, 0, 1, 'L', '0');			// Debits, Transaction Fee Amount
		FILLFIELD (p_pstISO->astISODef, TLV_CREDIT_AMT, TYPE_N, 8, 0, 1, 'L', '0');					// Credits, Amount
		FILLFIELD (p_pstISO->astISODef, TLV_CREDIT_REV_AMT, TYPE_N, 8, 0, 1, 'L', '0');				// Credits, Reversal Amount
		FILLFIELD (p_pstISO->astISODef, TLV_DEBIT_AMT, TYPE_N, 8, 0, 1, 'L', '0');					// Debits, Amount
		FILLFIELD (p_pstISO->astISODef, TLV_DEBIT_REV_AMT, TYPE_N, 8, 0, 1, 'L', '0');				// Debits, Reversal Amount
		FILLFIELD (p_pstISO->astISODef, TLV_ORIGINAL_DATA_ELEMENT, TYPE_N, 21, 0, 1, 'L', '0');		// Original Data Elements

		FILLFIELD (p_pstISO->astISODef, TLV_FILE_UPDATE_CODE, TYPE_AN, 1, 0, 1, 'L', '0');			// File Update Code
		FILLFIELD (p_pstISO->astISODef, TLV_FILE_SEC_CODE, TYPE_AN, 2, 0, 1, 'L', '0');				// File Security Code
		FILLFIELD (p_pstISO->astISODef, TLV_RESP_INDICATOR, TYPE_AN, 5, 0, 1, 'L', '0');				// Response Indicator
		FILLFIELD (p_pstISO->astISODef, TLV_SVC_INDICATOR, TYPE_AN, 7, 0, 1, 'L', '0');				// Service Indicator
		FILLFIELD (p_pstISO->astISODef, TLV_REPLACEMENT_AMT, TYPE_N, 21, 0, 1, 'L', '0');				// Replacement Amounts
		FILLFIELD (p_pstISO->astISODef, TLV_MESG_SEC_CODE, TYPE_B, 32, 0, 1, 'L', '0');				// Message Security Code
		FILLFIELD (p_pstISO->astISODef, TLV_NET_SETTLE_AMT, TYPE_AN, 17, 0, 1, 'L', '0');				// Amount, Net Settlement
		FILLFIELD (p_pstISO->astISODef, TLV_PAYEE, TYPE_ANS, 25, 0, 1, 'L', '0');						// Payee
		FILLFIELD (p_pstISO->astISODef, TLV_SETTLE_INST_ID_CODE, TYPE_N, 6, 1, 0, 'L', '0');			// Settlement Institution Identification Code
		FILLFIELD (p_pstISO->astISODef, TLV_RECV_INST_ID_CODE, TYPE_N, 6, 1, 0, 'L', '0');			// Receiving Institution Identification Code

		FILLFIELD (p_pstISO->astISODef, TLV_FILE_NAME, TYPE_ANS, 17, 1, 0, 'L', '0');					// File Name
		FILLFIELD (p_pstISO->astISODef, TLV_ACCT_ID_1, TYPE_ANS, 28, 1, 0, 'L', '0');					// Account Identification 1
		FILLFIELD (p_pstISO->astISODef, TLV_ACCT_ID_2, TYPE_ANS, 28, 1, 0, 'L', '0');					// Account Identification 2
		FILLFIELD (p_pstISO->astISODef, TLV_TXN_DESC, TYPE_ANS, 100, 2, 0, 'L', '0');					// Transaction Description
		FILLFIELD (p_pstISO->astISODef, TLV_RESERVED_ISO_3, TYPE_ANS, 999, 2, 0, 'L', '0');			// Reserved for ISO Use
		FILLFIELD (p_pstISO->astISODef, TLV_RESERVED_ISO_4, TYPE_ANS, 999, 2, 0, 'L', '0');			// Reserved for ISO Use
		FILLFIELD (p_pstISO->astISODef, TLV_RESERVED_ISO_5, TYPE_ANS, 999, 2, 0, 'L', '0');			// Reserved for ISO Use
		FILLFIELD (p_pstISO->astISODef, TLV_RESERVED_ISO_6, TYPE_ANS, 999, 2, 0, 'L', '0');			// Reserved for ISO Use
		FILLFIELD (p_pstISO->astISODef, TLV_RESERVED_ISO_7, TYPE_ANS, 999, 2, 0, 'L', '0');			// Reserved for ISO Use
		FILLFIELD (p_pstISO->astISODef, TLV_RESERVED_ISO_8, TYPE_ANS, 999, 2, 0, 'L', '0');			// Reserved for ISO Use

		FILLFIELD (p_pstISO->astISODef, TLV_RESERVED_ISO_9, TYPE_ANS, 999, 2, 0, 'L', '0');			// Reserved for ISO Use
		FILLFIELD (p_pstISO->astISODef, TLV_RESERVED_NATIONAL_4, TYPE_ANS, 100, 2, 0, 'L', '0');		// Reserved for National Use
		FILLFIELD (p_pstISO->astISODef, TLV_AUTH_AGT_INST_ID_CODE, TYPE_ANS, 999, 2, 0, 'L', '0');	// Authorising Agent Institution ID Code
		FILLFIELD (p_pstISO->astISODef, TLV_RESERVED_NATIONAL_5, TYPE_ANS, 999, 2, 0, 'L', '0');		// Reserved for National Use
		FILLFIELD (p_pstISO->astISODef, TLV_RESERVED_NATIONAL_6, TYPE_ANS, 999, 2, 0, 'L', '0');		// Reserved for National Use
		FILLFIELD (p_pstISO->astISODef, TLV_RESERVED_NATIONAL_7, TYPE_ANS, 999, 2, 0, 'L', '0');		// Reserved for National Use
		FILLFIELD (p_pstISO->astISODef, TLV_RESERVED_NATIONAL_8, TYPE_ANS, 999, 2, 0, 'L', '0');		// Reserved for National Use
		FILLFIELD (p_pstISO->astISODef, TLV_RESERVED_NATIONAL_9, TYPE_ANS, 999, 2, 0, 'L', '0');		// Reserved for National Use
		FILLFIELD (p_pstISO->astISODef, TLV_RESERVED_NATIONAL_10, TYPE_ANS, 999, 2, 0, 'L', '0');		// Reserved for National Use
		FILLFIELD (p_pstISO->astISODef, TLV_RESERVED_PRIV_5, TYPE_ANS, 999, 2, 0, 'L', '0');			// Reserved Private (Key Management / Terminal Address-Branch)

		FILLFIELD (p_pstISO->astISODef, TLV_RESERVED_PRIV_6, TYPE_N, 500, 2, 0, 'L', '0');			// Reserved Private (Auth Indicators-CRT Authorization Data)
		FILLFIELD (p_pstISO->astISODef, TLV_RESERVED_PRIV_7, TYPE_ANS, 999, 2, 0, 'L', '0');			// Reserved Private (Card Issuer Identification Code)
		FILLFIELD (p_pstISO->astISODef, TLV_RESERVED_PRIV_8, TYPE_ANS, 999, 2, 0, 'L', '0');			// Reserved Private (Cryptographic Service Message / Invoice Data)
		FILLFIELD (p_pstISO->astISODef, TLV_INFO_TEXT, TYPE_ANS, 999, 2, 0, 'L', '0');				// Info Text / Batch and Shift Data
		FILLFIELD (p_pstISO->astISODef, TLV_NET_MGMT_INFO, TYPE_ANS, 999, 2, 0, 'L', '0');			// Network Management Information / Settlement Data
		FILLFIELD (p_pstISO->astISODef, TLV_ISSUER_TRACE_ID, TYPE_ANS, 999, 2, 0, 'L', '0');			// Issuer Trace ID / Pre-Auth and Chargeback Data
		FILLFIELD (p_pstISO->astISODef, TLV_RESERVED_PRIV_9, TYPE_ANS, 999, 2, 0, 'L', '0');			// Reserved for Private Use (User Data)
		FILLFIELD (p_pstISO->astISODef, TLV_MAC_2, TYPE_B, 32, 0, 1, 'L', '0');						// Message Authentication Code (MAC)
	}

	ucRet = 0x00;

CleanUp:
	FCLOSE (pfdConfig);
	MEM_CLEAN (pcBuf);

	return ucRet;
}

void iso8583_term (ISO8583 *p_pstISO)
{
	memset (p_pstISO, 0x00, sizeof (ISO8583));
}

unsigned char iso8583_parse (ISO8583 *p_pstISO, unsigned char *p_pucInput, int p_iLen, int p_iPos, GTLV_DATA *p_pstTLV, char *p_pcErrMsg)
{
	unsigned char	ucRet								= 0x01,
					aucTemp [MAX_TEMP_BUFFER_SZ],
					aucBitmapFlag [MAX_ISO_FIELDS],
					aucBitmap [DEFAULT_BITMAP_SZ * 3];

	char			acTemp [MAX_TEMP_BUFFER_SZ];

	int				iIdx								= 0,
					iSize								= 0,
					iTemp,
					iChar,
					iLen,
					iStopPos,
					iLoop,
					iLoop2;

	if (0 == p_pucInput)
	{
		ucRet = E_EMPTY_MESSAGE;
		p_pcErrMsg ? strcpy (p_pcErrMsg, "Empty ISO Message") : 0;
		goto CleanUp;
	}

	// Parse the byte stream to get the TPDU and MTI.
	// Move the index of the byte stream along the parsing.

	if (p_iLen < iIdx + p_pstISO->iHeader)
	{
		ucRet = E_LEN_TOO_SHORT;
		p_pcErrMsg ? strcpy (p_pcErrMsg, "ISO message too short (header)") : 0;
		goto CleanUp;
	}

	switch (p_pstISO->iStandard)
	{
	case I_ISO_8583:
		// == TPDU ==
		ucRet = gtlv_add (p_pstTLV, TLV_TPDU, p_pucInput + iIdx, p_pstISO->iHeader, 0x00);

		if (0x00 != ucRet)
		{
			goto CleanUp;
		}

		iIdx += p_pstISO->iHeader;
		
		// MTI		
		if (p_iLen < iIdx + p_pstISO->iMTILen) 
		{
			ucRet = E_LEN_TOO_SHORT;
			p_pcErrMsg ? strcpy (p_pcErrMsg, "ISO message too short (MTI)") : 0;
			goto CleanUp;
		}

		ucRet = gtlv_add (p_pstTLV, TLV_MTI, p_pucInput + iIdx, p_pstISO->iMTILen, 0x00);
		
		if (0x00 != ucRet)
		{
			goto CleanUp;
		}

		iIdx += p_pstISO->iMTILen;
		
		// Read the bitmap
		// If the first bit of the bitmap is 1 then the bitmap size is 128 bits.
		// If the first bit of the bitmap is 0 then the bitmap size is 64 bits.

		if (p_iLen < iIdx + DEFAULT_BITMAP_SZ) 
		{
			ucRet = E_LEN_TOO_SHORT;
			p_pcErrMsg ? strcpy (p_pcErrMsg, "ISO message too short (bitmap)") : 0;
			goto CleanUp;
		}
		
		iSize = (127 < *(p_pucInput + iIdx)) ? 128 : 64;

		if (p_iLen < iIdx + (iSize / DEFAULT_BITMAP_SZ))
		{
			ucRet = E_LEN_TOO_SHORT;
			p_pcErrMsg ? strcpy (p_pcErrMsg, "ISO message too short (bitmap)") : 0;
			goto CleanUp;
		}

		memcpy (aucBitmap, p_pucInput + iIdx, iSize / DEFAULT_BITMAP_SZ);
		aucBitmap [iSize / DEFAULT_BITMAP_SZ] = 0;

		ucRet = gtlv_add (p_pstTLV, TLV_BIT_MAP_EXT, aucBitmap, iSize / DEFAULT_BITMAP_SZ, 0x00);

		if (0x00 != ucRet)
		{
			goto CleanUp;
		}

		iIdx += iSize / DEFAULT_BITMAP_SZ;
		break;

	case I_ISO_B24:
		memcpy (acTemp, p_pucInput + iIdx, p_pstISO->iHeader);
		acTemp [p_pstISO->iHeader] = 0;
		ucHexToByte (acTemp, aucTemp, &iLen);

		ucRet = gtlv_add (p_pstTLV, TLV_TPDU, aucTemp, iLen, 0x00);
		
		if (0x00 != ucRet)
		{
			goto CleanUp;
		}

		iIdx += p_pstISO->iHeader;

		// MTI
		if (p_iLen < iIdx + p_pstISO->iMTILen * 2)
		{
			ucRet = E_LEN_TOO_SHORT;
			p_pcErrMsg ? strcpy (p_pcErrMsg, "ISO message too short (MTI)") : 0;
			goto CleanUp;
		}

		strncpy (acTemp, (char *) (p_pucInput + iIdx), p_pstISO->iMTILen * 2);
		acTemp [p_pstISO->iMTILen * 2] = 0;
		ucHexToByte (acTemp, aucTemp, &iLen);

		ucRet = gtlv_add (p_pstTLV, TLV_MTI, aucTemp, iLen, 0x00);
		
		if (0x00 != ucRet)
		{
			goto CleanUp;
		}

		iIdx += p_pstISO->iMTILen * 2;

		// Read the bitmap
		// If the first bit of the bitmap is 1 then the bitmap size is 128 bits.
		// If the first bit of the bitmap is 0 then the bitmap size is 64 bits.

		if (p_iLen < iIdx + DEFAULT_BITMAP_SZ)
		{
			ucRet = E_LEN_TOO_SHORT;
			p_pcErrMsg ? strcpy (p_pcErrMsg, "ISO message too short (bitmap)") : 0;
			goto CleanUp;
		}

		memcpy (acTemp, p_pucInput + iIdx, 2);
		acTemp [2] = 0;
		ucHexToByte (acTemp, aucBitmap, &iLen);

		iSize = (127 < aucBitmap [0]) ? 128 : 64;

		if (p_iLen < iIdx + (iSize * 2 / DEFAULT_BITMAP_SZ))
		{
			ucRet = E_LEN_TOO_SHORT;
			p_pcErrMsg ? strcpy (p_pcErrMsg, "ISO message too short (bitmap)") : 0;
			goto CleanUp;
		}

		memcpy (acTemp, p_pucInput + iIdx, iSize * 2 / DEFAULT_BITMAP_SZ);
		acTemp [iSize * 2 / DEFAULT_BITMAP_SZ] = 0;

		ucHexToByte (acTemp, aucBitmap, &iLen);
		iIdx += iSize * 2 / DEFAULT_BITMAP_SZ;

		ucRet = gtlv_add (p_pstTLV, TLV_BIT_MAP_EXT, aucBitmap, iSize / DEFAULT_BITMAP_SZ, 0x00);
		
		if (0x00 != ucRet)
		{
			goto CleanUp;
		}

		iIdx += iSize * 2 / DEFAULT_BITMAP_SZ;
		break;
	}

	for (iLoop = 0, iTemp = 0; iLoop < iSize / DEFAULT_BITMAP_SZ; iLoop++)
	{
		iChar = aucBitmap [iLoop];

		for (iLoop2 = 0; iLoop2 < DEFAULT_BITMAP_SZ; iLoop2++)
		{
			aucBitmapFlag [iTemp++] = ((1 << (DEFAULT_BITMAP_SZ - iLoop2 - 1)) == (iChar & (1 << (DEFAULT_BITMAP_SZ - iLoop2 - 1))));
		}
	}

	iStopPos = p_iPos;

	if (0 == iStopPos) iStopPos = 196;

	// For all the '1' in the bitmap, extract the ISO field value.
	// call ulExtractValue to extract the value from the data stream.
	// Return FAILED if the format of the ISO message is wrong
	
	for (iLoop = 1; iLoop < iSize; iLoop++)
	{
		if (iLoop > iStopPos) goto CleanUp;

		if (aucBitmapFlag [iLoop]) 
		{
			if (0 != p_pstISO->astISODef [iLoop].iID)
			{
				if (p_iLen == iIdx)
				{
					ucRet = E_INSUF_DATA;
					p_pcErrMsg ? sprintf (p_pcErrMsg, "Insufficient data [%d]", p_iLen) : 0;
					goto CleanUp;
				}
				
				ucRet = iso8583_extract_field (p_pstISO, iLoop + 1, p_pucInput, p_iLen, &iIdx, aucTemp, &iLen, p_pcErrMsg);
				
				if (0x00 != ucRet)
				{
					goto CleanUp;
				}

				ucRet = gtlv_add (p_pstTLV, iLoop + 1, aucTemp, iLen, 0x00);

				if (0x00 != ucRet)
				{
					goto CleanUp;
				}
			}
			else 
			{
				ucRet = E_INVALID_ISO_FIELD;
				p_pcErrMsg ? sprintf (p_pcErrMsg, "Invalid ISO field %d", iLoop + 1) : 0;
				goto CleanUp;
			}
		}
	}

	// Check if unused byte(s) is found
	if (iIdx != p_iLen)
	{
		ucRet = E_UNUSED_DATA_FOUND;
		p_pcErrMsg ? sprintf (p_pcErrMsg, "Unused data found [%d], total [%d]", p_iLen - iIdx, p_iLen) : 0;
	}
	else
	{
		ucRet = 0x00;
	}

CleanUp:

	return ucRet;
}

unsigned char iso8583_build (ISO8583 *p_pstISO, unsigned char *p_pucOutput, int p_iSize, int *p_piLen, int p_iPos, GTLV_DATA *p_pstTLV, char *p_pcErrMsg)
{
	unsigned char	ucRet,
					aucTemp [MAX_TEMP_BUFFER_SZ],
					*pucBuf;

	int				iIdx							= 0,
					iLen,
					iLoop;

	// TPDU
	if (0 < p_pstISO->iHeader)
	{
		ucRet = gtlv_get (p_pstTLV, TLV_TPDU, &pucBuf, &iLen, 0x00);

		if (0x00 != ucRet || 0 == pucBuf || 0 >= iLen)
		{
			p_pcErrMsg ? sprintf (p_pcErrMsg, "Missing header") : 0;
			ucRet = E_MISSING_DATA;
			goto CleanUp;
		}

		switch (p_pstISO->iStandard)
		{
		case I_ISO_8583:
			if (iIdx + iLen > p_iSize) 
			{
				p_pcErrMsg ? sprintf (p_pcErrMsg, "Insufficient buffer length") : 0;
				ucRet = E_LEN_TOO_SHORT;
				goto CleanUp;
			}

			memcpy (p_pucOutput + iIdx, pucBuf, iLen);
			iIdx += iLen;
			break;

		case I_ISO_B24:
			if (iIdx + p_pstISO->iHeader > p_iSize) 
			{
				p_pcErrMsg ? sprintf (p_pcErrMsg, "Insufficient buffer length") : 0;
				ucRet = E_LEN_TOO_SHORT;
				goto CleanUp;
			}

			ucByteToHex (pucBuf, iLen, (char *) p_pucOutput + iIdx);
			iIdx += iLen;
			break;
		}
	}

	// MTI
	if (0 < p_pstISO->iMTILen)
	{
		ucRet = gtlv_get (p_pstTLV, TLV_MTI, &pucBuf, &iLen, 0x00);

		if (0x00 != ucRet || 0 == pucBuf || 0 >= iLen)
		{
			p_pcErrMsg ? sprintf (p_pcErrMsg, "Missing MTI") : 0;
			ucRet = E_MISSING_DATA;
			goto CleanUp;
		}

		switch (p_pstISO->iStandard)
		{
		case I_ISO_8583:
			if (iIdx + iLen > p_iSize) 
			{
				p_pcErrMsg ? sprintf (p_pcErrMsg, "Insufficient buffer length") : 0;
				ucRet = E_LEN_TOO_SHORT;
				goto CleanUp;
			}

			memcpy (p_pucOutput + iIdx, pucBuf, iLen);
			iIdx += iLen;
			break;

		case I_ISO_B24:
			if (iIdx + p_pstISO->iHeader > p_iSize) 
			{
				p_pcErrMsg ? sprintf (p_pcErrMsg, "Insufficient buffer length") : 0;
				ucRet = E_LEN_TOO_SHORT;
				goto CleanUp;
			}

			ucByteToHex (pucBuf, iLen, (char *) p_pucOutput + iIdx);
			iIdx += iLen;
			break;
		}
	}

	// Generate the bitmap
	iso8583_gen_bitmap (p_pstISO, aucTemp, &iLen, p_pstTLV);

	if (0 >= iLen)
	{
		p_pcErrMsg ? sprintf (p_pcErrMsg, "Missing bitmap") : 0;
		ucRet = E_MISSING_DATA;
		goto CleanUp;
	}
	else
	{
		switch (p_pstISO->iStandard)
		{
		case I_ISO_8583:
			memcpy (p_pucOutput + iIdx, aucTemp, iLen);
			iIdx += iLen;
			break;

		case I_ISO_B24:
			ucByteToHex (aucTemp, iLen, (char *) p_pucOutput + iIdx);
			iIdx += iLen * 2;
		}

		ucRet = gtlv_add (p_pstTLV, TLV_BIT_MAP_EXT, aucTemp, iLen, 0x00);

		if (0x00 != ucRet)
		{
			goto CleanUp;
		}
	}

	// Generate the data
	for (iLoop = 2; iLoop <= MAX_ISO_FIELDS; iLoop++) 
	{
		// Get the field value
		ucRet = gtlv_get (p_pstTLV, iLoop, &pucBuf, &iLen, 0x00);

		if (0x00 == ucRet && 0 != pucBuf && 0 < iLen)
		{
			iso8583_build_field (p_pstISO, iLoop, pucBuf, iLen, p_pucOutput, &iIdx, p_pcErrMsg);
		}
	}

	*p_piLen = iIdx;

	ucRet = 0x00;

CleanUp:

	return ucRet;
}

unsigned char iso8583_extract_field (ISO8583 *p_pstISO, int p_iID, unsigned char *p_pucInput, int p_iLen, int *p_piIdx, unsigned char *p_pucOutput, int *p_piLen, char *p_pcErrMsg)
{
	unsigned char	ucRet,
					*pucCurPtr						= p_pucInput + (*p_piIdx);

	char			acTemp [MAX_TEMP_BUFFER_SZ];

	int				iLen							= 0,
					iSize,
					iPos,
					iIdx;

	ISO_FIELD		*pstISODef						= &p_pstISO->astISODef [p_iID - 1];

	iSize = pstISODef->iSize;

	// Read the length of the field for non-fixed field
	if (!pstISODef->iFixed)
	{
		iLen = pstISODef->iLen;
		
		if (p_iLen - (*p_piIdx) < iLen)
		{
			ucRet = E_INSUF_DATA;
			p_pcErrMsg ? sprintf (p_pcErrMsg, "F%d - Insuf data (length) avail [%d] required [%d]", p_iID, p_iLen, (*p_piIdx) + iLen) : 0;
			goto CleanUp;
		}

		switch (p_pstISO->iStandard)
		{
		case I_ISO_8583:
			ucByteToHex (pucCurPtr, iLen, acTemp);
			break;
		
		case I_ISO_B24:
			memcpy (acTemp, pucCurPtr, iLen);
			acTemp [iLen] = 0;
		}

		iSize = atoi (acTemp);

		if (TYPE_B == pstISODef->iType) 
		{
			iSize = iSize * 8;
		}
		
		(*p_piIdx) += iLen;
	}

	// Extract the field value based on the field type
	switch (pstISODef->iType)
	{
	case TYPE_A:
	case TYPE_AN:
	case TYPE_ANS:
		if (p_iLen - (*p_piIdx) < iSize)
		{
			ucRet = E_INSUF_DATA;
			p_pcErrMsg ? sprintf (p_pcErrMsg, "F%d - Insuf data avail [%d] required [%d]", p_iID, p_iLen, (*p_piIdx) + iSize) : 0;
			goto CleanUp;
		}

		memcpy (acTemp, pucCurPtr, iSize);
		acTemp [iSize] = 0;

		ucHexToByte (acTemp, p_pucOutput, &iLen);

		*p_piLen = iSize / 2;
		(*p_piIdx) += iSize;
		break;

	case TYPE_N:
		if (p_iLen - (*p_piIdx) < iSize + 1)
		{
			ucRet = E_INSUF_DATA;
			p_pcErrMsg ? sprintf (p_pcErrMsg, "F%d - Insuf data avail [%d] required [%d]", p_iID, p_iLen, (*p_piIdx) + ((iSize + 1) / 2)) : 0;
			goto CleanUp;
		}

		memset (p_pucOutput, 0x00, iSize);

		switch (pstISODef->cPadPos)
		{
		case 'L':
			memcpy (p_pucOutput, pucCurPtr + iLen, iSize);
			break;

		case 'R':
			memcpy (p_pucOutput, pucCurPtr, iSize);
			break;
		}

		*p_piLen = iSize;

		(*p_piIdx) += iSize;
		break;

	case TYPE_B:
		if (p_iLen - (*p_piIdx) < iSize / 8)
		{
			ucRet = E_INSUF_DATA;
			p_pcErrMsg ? sprintf (p_pcErrMsg, "F%d - Insuf data avail [%d] required [%d]", p_iID, p_iLen, (*p_piIdx) + (iSize / 8)) : 0;
			goto CleanUp;
		}

		ucByteToHex (pucCurPtr + iLen, iSize / 8, acTemp);

		memcpy (p_pucOutput, acTemp, iSize / 4);
		*p_piLen = iSize / 4;

		(*p_piIdx) += (iSize / 8);
		break;

	case TYPE_Z:
		if (p_iLen - (*p_piIdx) < (iSize + 1) / 2)
		{
			ucRet = E_INSUF_DATA;
			p_pcErrMsg ? sprintf (p_pcErrMsg, "F%d - Insuf data avail [%d] required [%d]", p_iID, p_iLen, (*p_piIdx) + ((iSize + 1) / 2)) : 0;
			goto CleanUp;
		}

		ucByteToHex (pucCurPtr + iLen, (iSize + 1) / 2, acTemp);

		iLen = strlen (acTemp) - (iSize % 2);
		iPos = 0;
		iIdx = 0;

		if (0 != iSize % 2)
		{
			switch (pstISODef->cPadPos)
			{
			case 'L': iPos = 1; break;
			case 'R': iPos = 0; break;
			}
		}

		for (iIdx = 0; iIdx < iLen; iIdx++, iPos++)
		{
			*(p_pucOutput + iIdx) = (68 == acTemp [iPos]) ? 61 : acTemp [iPos];
		}

		*p_piLen = iIdx;

		if (0 != iSize % 2) iSize++;
		(*p_piIdx) += ((iSize + 1) / 2);
		break;
	}

	ucRet = 0x00;

CleanUp:

	return ucRet;
}

unsigned char iso8583_build_field (ISO8583 *p_pstISO, int p_iID, unsigned char *p_pucInput, int p_iLen, unsigned char *p_pucOutput, int *p_piIdx, char *p_pcErrMsg)
{
	unsigned char	ucRet							= 0x01,
					aucTemp [MAX_TEMP_BUFFER_SZ];

	char			acHex [MAX_FIELD_LEN + 1];

	int				iLen,
					iLoop;

	ISO_FIELD		stISODef;

	// Get the ISO field info record from the array
	if (0 == p_pstISO->astISODef [p_iID - 1].iID)
	{
		ucRet = E_MISSING_FIELD_DEF;
		p_pcErrMsg ? sprintf (p_pcErrMsg, "F%d - Missing field definition", p_iID) : 0;
		goto CleanUp;
	}

	memcpy (&stISODef, &p_pstISO->astISODef [p_iID - 1], sizeof (ISO_FIELD));
	
	iLen = p_iLen;

	// Padding ISO Field
	if (!stISODef.iFixed)
	{
		if (iLen > stISODef.iSize) iLen = stISODef.iSize;

		if (I_ISO_8583 == p_pstISO->iStandard)
		{
			if (TYPE_B != stISODef.iType)
			{
				iLen = iLen * 2;
			}

			sprintf (acHex, "%d", iLen);

			if (2 == stISODef.iLen)
			{
				if (10 > iLen)
				{
					acHex [3] = acHex [0];
					acHex [2] = acHex [1] = acHex [0] = '0';
				}
				else if (100 > iLen)
				{
					acHex [3] = acHex [1];
					acHex [2] = acHex [0];
					acHex [1] = acHex [0] = '0';
				}
				else if (1000 > iLen)
				{
					acHex [3] = acHex [2];
					acHex [2] = acHex [1];
					acHex [1] = acHex [0];
					acHex [0] = '0';
				}

				acHex [4] = 0;
			}
			else
			{
				if (10 > iLen)
				{
					acHex [1] = acHex [0];
					acHex [0] = '0';
				}

				acHex [2] = 0;
			}

			ucHexToByte (acHex, aucTemp, &stISODef.iLen);
			memcpy (p_pucOutput + *p_piIdx, aucTemp, stISODef.iLen);
		}
		else
		{
			sprintf (acHex, "%d", iLen);

			if (3 == stISODef.iLen)
			{
				if (10 > iLen)
				{
					acHex [2] = acHex [0];
					acHex [1] = acHex [0] = '0';
				}
				else if (100 > iLen)
				{
					acHex [2] = acHex [1];
					acHex [1] = acHex [0];
					acHex [0] = '0';
				}

				acHex [3] = 0;
			}
			else if (2 == stISODef.iLen)
			{
				if (10 > iLen)
				{
					acHex [1] = acHex [0];
					acHex [0] = '0';
				}

				acHex [2] = 0;
			}

			memcpy (p_pucOutput + *p_piIdx, acHex, stISODef.iLen);
		}

		(*p_piIdx) += stISODef.iLen;
	}

	// Based on the type of the field, form the data in appropriate field type
	switch (stISODef.iType)
	{
	case TYPE_A:
	case TYPE_AN:
	case TYPE_ANS:
		if (stISODef.iFixed)
		{
			memset (p_pucOutput + *p_piIdx, stISODef.cPadChar, stISODef.iSize);

			if (iLen > stISODef.iSize / 2) iLen = stISODef.iSize / 2;

			switch (stISODef.cPadPos)
			{
			case 'L':
				ucByteToHex (p_pucInput, iLen, (char *) p_pucOutput + *p_piIdx + (stISODef.iSize / 2) - iLen);
				break;

			case 'R':
				ucByteToHex (p_pucInput, iLen, (char *) p_pucOutput + *p_piIdx);
			}

			if (iLen < stISODef.iSize / 2) iLen = stISODef.iSize / 2;
			(*p_piIdx) += stISODef.iSize;
		}
		else
		{
			ucByteToHex (p_pucInput, iLen, (char *) p_pucOutput + *p_piIdx);
			(*p_piIdx) += iLen;
		}

		break;

	case TYPE_N:
		if (iLen >= stISODef.iSize)
		{
			memcpy (p_pucOutput + *p_piIdx, p_pucInput, stISODef.iSize);
		}
		else
		{
			memset (p_pucOutput + *p_piIdx, '0' - stISODef.cPadChar, stISODef.iSize);

			switch (stISODef.cPadPos)
			{
			case 'L':
				memcpy ((p_pucOutput + *p_piIdx) + stISODef.iSize - iLen, p_pucInput, iLen);
				break;

			case 'R':
				memcpy (p_pucOutput + *p_piIdx, p_pucInput, iLen);
			}
		}

		(*p_piIdx) += stISODef.iSize;
		break;

	case TYPE_B:
		if (stISODef.iFixed)
		{
			if (iLen > stISODef.iSize) iLen = stISODef.iSize;
		}

		if (!stISODef.iFixed || iLen >= stISODef.iSize)
		{
			memcpy (p_pucOutput + *p_piIdx, p_pucInput, iLen);
		}
		else
		{
			memset (p_pucOutput + *p_piIdx, stISODef.cPadChar, stISODef.iSize);

			switch (stISODef.cPadPos)
			{
			case 'L':
				memcpy (p_pucOutput + *p_piIdx + stISODef.iSize - iLen, p_pucInput, iLen);
				break;

			case 'R':
				memcpy (p_pucOutput + *p_piIdx, p_pucInput, iLen);
			}

			iLen = stISODef.iSize;
		}

		(*p_piIdx) += iLen;
		break;

	case TYPE_Z:
		if (iLen >= stISODef.iSize)
		{
			memcpy (p_pucOutput + *p_piIdx, p_pucInput, stISODef.iSize);

			for (iLoop = 0; iLoop < stISODef.iSize; iLoop++)
			{
				if ('=' == *(p_pucOutput + *p_piIdx + iLoop))
				{
					*(p_pucOutput + *p_piIdx) = 'D';
				}
			}
		}
		else
		{
			memset (p_pucOutput + *p_piIdx, stISODef.cPadChar, stISODef.iSize);

			switch (stISODef.cPadPos)
			{
			case 'L':
				memcpy (p_pucOutput + *p_piIdx + stISODef.iSize - iLen, p_pucInput, iLen);
				break;

			case 'R':
				memcpy (p_pucOutput + *p_piIdx, p_pucInput, iLen);
			}

			for (iLoop = 0; iLoop < stISODef.iSize; iLoop++)
			{
				if ('=' == *(p_pucOutput + *p_piIdx + iLoop))
				{
					*(p_pucOutput + *p_piIdx) = 'D';
				}
			}
		}

		(*p_piIdx) += stISODef.iSize;
	}

CleanUp:

	return ucRet;
}

unsigned char iso8583_get_xml_attr_value (char *p_pcBuf, int *p_piIdx, char *p_pcField, char *p_pcEndStr, char *p_pcValue, int *p_piLen)
{
	char		*pcTemp		= 0,
				*pcEnd		= 0;

	int			iLen,
				iIdx		= 0;

	if (!p_pcBuf || !p_pcField || !p_piIdx || *p_piIdx < 0 || !p_pcValue || !p_piLen || *p_piLen <= 0)
	{
		return 0x01;
	}

	iLen = strlen (p_pcField);

	pcTemp = strstr (p_pcBuf + *p_piIdx, p_pcField);

	if (0 == pcTemp) return 0x02;

	if (p_pcEndStr)
	{
		pcEnd = strstr (p_pcBuf + *p_piIdx, p_pcEndStr);
		
		if (0 != pcEnd && 0 <= pcTemp - pcEnd) 
		{
			*p_piIdx = pcEnd - p_pcBuf + 1;
			return 0x02;
		}
	}

	*p_piIdx = pcTemp - p_pcBuf;
	pcTemp = pcTemp + iLen + 2;

	while ('"' != *(pcTemp + iIdx) && 0 != *(pcTemp + iIdx)) 
	{
		if (iIdx == *p_piLen - 1) return 0x03;
		
		*(p_pcValue + iIdx) = *(pcTemp + iIdx);
		iIdx++;
	}

	*p_piLen = iIdx;
	*p_piIdx += iLen + iIdx + 2;
	*(p_pcValue + iIdx) = 0;

	return 0x00;
}

void iso8583_gen_bitmap (ISO8583 *p_pstISO, unsigned char *p_pucBitmap, int *p_piLen, GTLV_DATA *p_pstTLV)
{
	unsigned char			ucRet,
							*pucField;

	int						iLoop,
							iLen;

	#if defined (WIN32)

		unsigned INT64		ullLeft		= 0;
		unsigned INT64		ullRight	= 0;
		unsigned INT64		ullTemp		= 1;

	#else

		unsigned long long	ullLeft		= 0;
		unsigned long long	ullRight	= 0;
		unsigned long long	ullTemp		= 1;

	#endif

	// Get all Field IDs in the container and set the bitmap
	for (iLoop = 2; iLoop <= MAX_ISO_FIELDS; iLoop++) 
	{
		ucRet = gtlv_get (p_pstTLV, iLoop, &pucField, &iLen, 0x00);

		if (0x00 == ucRet && 0 != pucField && 0 < iLen)
		{
			if (64 < iLoop)
			{
				ullRight += (ullTemp << (128 - iLoop));
			}
			else
			{
				ullLeft += (ullTemp << (64 - iLoop));
			}
		}
	}

	if (0 < ullRight)
	{
		*p_piLen = DEFAULT_BITMAP_SZ * 2;

		for (iLoop = 0; iLoop < DEFAULT_BITMAP_SZ; iLoop++)
		{
			*(p_pucBitmap + DEFAULT_BITMAP_SZ + iLoop) = (int) (ullRight >> ((DEFAULT_BITMAP_SZ - iLoop - 1) * DEFAULT_BITMAP_SZ));
		}
	}
	else 
	{
		*p_piLen = DEFAULT_BITMAP_SZ;
	}

	for (iLoop = 0; iLoop < DEFAULT_BITMAP_SZ; iLoop++)
	{
		*(p_pucBitmap + iLoop) = (int) (ullLeft >> ((DEFAULT_BITMAP_SZ - iLoop - 1) * DEFAULT_BITMAP_SZ));
	}

	if (0 < ullRight)
	{
		*p_pucBitmap |= 1 << (DEFAULT_BITMAP_SZ - 1);
	}
}
