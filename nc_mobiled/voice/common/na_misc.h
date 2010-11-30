#ifndef NA_MISC_H
#define NA_MISC_H

#include <netinet/ip_icmp.h>
#include <arpa/inet.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "na_common.h"

#define	NA_ENC_NO_DIVIDER  90
#define	NA_ENC_NO_PAD_CHR  33		//!

unsigned char *na_int_to_byte (int val, unsigned char *ret);
unsigned char *na_uint_to_byte (unsigned int val, unsigned char *ret);
unsigned char *na_short_to_byte (short val, unsigned char *ret);
unsigned char *na_ushort_to_byte (unsigned short val, unsigned char *ret);
unsigned char *na_hex_to_byte (char *val, unsigned char *ret, unsigned int *rlen);

unsigned short na_byte_to_ushort (unsigned char *val, int len);
int na_byte_to_int (unsigned char *val, int len);
unsigned int na_byte_to_uint (unsigned char *val, int len);
unsigned long na_byte_to_ulong (unsigned char *val, int len);
char *na_byte_to_hex (unsigned char *val, unsigned int len, char *ret);

char *na_match_keyword (char *str, char *keyword);
char *na_find_filename (char *path);

int na_is_numeric (char *number);
unsigned long na_reverse_ulong (unsigned long d);

unsigned short na_crc (unsigned char *buf, unsigned int len);
unsigned short na_crc_ex (unsigned char *pbuf, unsigned int *plen, unsigned char *cbuf, unsigned int *clen, unsigned int *res, int final);

unsigned char na_lrc (unsigned char *buf, unsigned int len, unsigned char lrc);

unsigned long na_addr_aton (char *addr);
unsigned long na_addr_atoh (char *addr);
unsigned long na_adjt_addr (char *addr, char *mask, char *ret, long adjust);
char *na_addr_ntoa (unsigned long addr, char *ret);
char *na_addr_htoa (unsigned long addr, char *ret);

unsigned char *na_encode_no (unsigned long no, unsigned char *enc, unsigned int size);
unsigned long	na_decode_no (unsigned char *enc, unsigned int size);

#ifdef __cplusplus
extern "C" {
#endif

#endif

