#ifndef _PDU__H_
#define _PDU__H_


#if 0
#define         GSM_7BIT        0
#define         GSM_8BIT        4
#define         GSM_UCS2        8
#endif

extern unsigned  char pk_rcv_total;
extern unsigned  char pk_rcv_num;

int gsmSendSMS(SM_PARAM* pSrc, COM_PORT *sms_comport);
int gsmRecvSMS(SM_PARAM *pDst, int i, COM_PORT *sms_comport);
int gsmDeleteSMS(int index, COM_PORT *sms_comport);


#if 0
const unsigned char singlebytecharacter[127] = {
64, 163, 36, 165, 232, 233, 249, 236, 242, 199, 10, 216, 248, 13, 197 ,229, 0, 95, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 198, 230, 223, 201, 32, 33, 34, 35, 164, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 161, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90,196, 214, 209, 220, 167, 191, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 228, 124, 125,126
};
#endif

#endif
