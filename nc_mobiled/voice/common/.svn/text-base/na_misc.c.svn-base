/*******************************************************************************
* This file is licensed under the terms of your license agreement(s) with
* GHL covering this file. Redistribution, except as permitted by the terms of
* your license agreement(s) with GHL, is strictly prohibited.
********************************************************************************/

#include "na_misc.h"

unsigned char *na_int_to_byte (int val, unsigned char *ret)
{
  int size  = sizeof (int),
      i     = 0;

  if (0 > val || !ret)
    return 0;

  while (size)
    *(ret + --size) = (val >> ((8 * i++) & 0xFF));
  return ret;
}

unsigned char *na_uint_to_byte (unsigned int val, unsigned char *ret)
{
  return na_int_to_byte ((int) val, ret);
}

unsigned char	*na_short_to_byte (short val, unsigned char *ret)
{
  return na_ushort_to_byte ((unsigned short) val, ret);
}

unsigned char	*na_ushort_to_byte (unsigned short val, unsigned char *ret)
{
  int size  = sizeof (unsigned short),
      i     = 0;

  if (!ret)
    return 0;

  while (size)
    *(ret + --size) = (val >> ((8 * i++) & 0xFF));
  return ret;
}

unsigned char *na_hex_to_byte (char *val, unsigned char *ret, unsigned int *rlen)
{
  int           i,
                pos = 0,
                left,
                right;
  unsigned int  vlen;

  vlen = strlen (val);
  for (i = 0; i < vlen; i++, pos++) {
    left = val [i++];
    right = val [i];

    left = (58 > left) ? (left - 48) : (left - 55);
    right = (58 > right) ? (right - 48) : (right - 55);

    ret [pos] = (left << 4) | right;
  }

  *rlen = pos;

  return ret;
}

unsigned short na_byte_to_ushort (unsigned char *val, int len)
{
  int i;
  unsigned short ret = 0;

  if (val &&
      sizeof (unsigned short) >= len) {
    for (i = 0; i < len; i++) {
      ret += val [i];
      if (i < len - 1)
        ret = ret << 8;
    }
  }

  return ret;
}

int na_byte_to_int (unsigned char *val, int len)
{
  int i, ret;

  if (!val || sizeof (int) < len)
    return -1;

  for (i = 0, ret = 0, len--; i < len; i++) {
    ret += *(val + i);
    ret = ret << 8;
  }
  ret += *(val + len);

  return ret;
}

unsigned int na_byte_to_uint (unsigned char *val, int len)
{
  return (unsigned int) na_byte_to_int (val, len);
}

unsigned long na_byte_to_ulong (unsigned char *val, int len)
{
  int i;
  unsigned long ret;

  if (!val || sizeof (unsigned long) < len)
    return 0;

  ret = 0;
  for (i = 0; i < len; i++) {
    ret += *(val + i);

    if (i < len - 1)
      ret = ret << 8;
  }

  return ret;
}

char *na_byte_to_hex (unsigned char *val, unsigned int len, char *ret)
{
  unsigned int index;
  int l, r;
  char *ptr = ret;

  for (index = 0; index < len; index++) {
    l = (*(val + index) & 0xF0) >> 4;
    r = (*(val + index) & 0x0F);

    *(ptr++) = (10 > l) ? l + 48 : l + 55;
    *(ptr++) = (10 > r) ? r + 48 : r + 55;
  }
  *ptr = 0;

  return ret;
}

char *na_match_keyword (char *str, char *keyword)
{
  char  buf [256],
        *ptr = keyword,
        *tab,
        *ret;

  while (ptr) {
    // find for TAB
    if ((tab = strchr (ptr, 9))) {
      if (tab != ptr) {
        strncpy (buf, ptr, tab - ptr);
        buf [tab - ptr] = 0;
      }
      else {
        buf [0] = '\t';
        buf [1] = 0;
      }

      ptr = *(++tab) ? tab : 0;
    }
    else {
      strcpy (buf, ptr);
      ptr = 0;
    }

    if (buf [0] &&
        (ret = strstr (str, buf)))
      return ret;
  }

  return 0;
}

char *na_find_filename (char *path)
{
  char  *filename;

  if ((filename = strrchr (path, '/')))
    filename++;
  else
    filename = path;

  if (!*filename)
    filename = 0;

  return filename;
}

int na_is_numeric (char *number)
{
  char  *ptr  = number;

  while (*ptr) {
    if ('0' > *ptr || '9' < *ptr)
      return 0;
    ptr++;
  }

  return 1;
}

unsigned short na_crc_ex (unsigned char *pbuf, unsigned int *plen, unsigned char *cbuf, unsigned int *clen, unsigned int *res, int final)
{
  unsigned int    sum = 0,
                  len     = *clen;

  unsigned char   combine [2];

  unsigned short  rv      = 0,
                  *ptr;

  if (res)
    sum = *res;

  if (pbuf && plen && *plen && len) {
    combine [0] = *pbuf;
    combine [1] = *cbuf;

    ptr = (unsigned short *) combine;
    sum += *ptr;

    cbuf++;

    *plen = 0;
    len--;
  }

  // Our algorithm is simple, using a 32 bit accumulator (sum),
  // we add sequential 16 bit words to it, and at the end, fold
  // back all the carry bits from the top 16 bits into the lower
  // 16 bits.

  while (1 < len) {
    memcpy (combine, cbuf, 2);
    ptr = (unsigned short *) combine;

    sum += *ptr;

    len -= 2;
    cbuf += 2;
  }

  if (final) {
    // mop up an odd byte, if necessary
    if (1 == len) {
      sum += *cbuf;
      len = 0;
    }

    // add back carry outs from top 16 bits to low 16 bits
    sum = (sum >> 16) + (sum & 0xFFFF); // add hi 16 to low 16
    sum += (sum >> 16);                 // add carry
    rv = (unsigned short) ~sum;         // truncate to 16 bits

    if (res)
      *res = rv;
  }
  else {
    if (res)
      *res = sum;
  }

  *clen = len;

  return rv;
}

unsigned short na_crc (unsigned char *buf, unsigned int len)
{
  return na_crc_ex (0, 0, buf, &len, 0, 1);
}

unsigned char na_lrc (unsigned char *buf, unsigned int len, unsigned char lrc)
{
  unsigned int i;

  for (i = 0; i < len; i++)
    lrc ^= buf [i];

  return lrc;
}

unsigned long na_reverse_ulong (unsigned long d)
{
  int           i;

  unsigned long num     = d,
                reverse;

  // reverse the address byte-order
  reverse = 0;
  for (i = 0; i < sizeof (unsigned long); i++) {
    reverse <<= 8;
    reverse |= (num >> (i * 8) & 0xFF);
  }

  return reverse;
}

unsigned long na_addr_aton (char *addr)
{
  unsigned long   rv  = 0;

  struct in_addr  s_addr;

  if (addr &&
      inet_aton (addr, &s_addr))
    memcpy (&rv, &s_addr, sizeof (unsigned long));

  return rv;
}

unsigned long na_addr_atoh (char *addr)
{
  // for host use need to reverse the address byte-order
  return na_reverse_ulong (na_addr_aton (addr));
}

char *na_addr_htoa (unsigned long addr, char *ret)
{
  if (ret)
    sprintf (ret, "%lu.%lu.%lu.%lu", (addr >> 24) & 0xFF, (addr >> 16) & 0xFF,
                                     (addr >> 8) & 0xFF, (addr & 0xFF));
  return ret;
}

char *na_addr_ntoa (unsigned long addr, char *ret)
{
  if (ret)
    sprintf (ret, "%lu.%lu.%lu.%lu", (addr & 0xFF), (addr >> 8) & 0xFF,
                                     (addr >> 16) & 0xFF, (addr >> 24) & 0xFF);
  return ret;
}

unsigned long na_adjt_addr (char *addr, char *mask, char *ret, long adjust)
{
  int             x;

  unsigned long   n_addr,
                  n_mask,
                  n_wild,
                  n_min   = 0,
                  n_max   = 0;

  if (!(n_addr = na_addr_atoh (addr)) ||
      !(n_mask = na_addr_atoh (mask)))
    return -1;

  if (0 < adjust) {
    n_wild = n_mask ^ 0xFFFFFFFF;

    // get the maximum ip address range
    for (x = 0; x < 4; x++) {
      if (3 == x)
        n_max |= (((n_wild >> ((3 - x) * 8)) | (n_addr >> ((3 - x) * 8))) & 0xFF) - 1;
      else {
        n_max |= ((n_wild >> ((3 - x) * 8)) | (n_addr >> ((3 - x) * 8))) & 0xFF;
        n_max <<= 8;
      }
    }

    if (n_max < (n_addr + adjust))
     return -2;
  }
  else {
    // get the minimum ip address range
    for (x = 0; x < 4; x++) {
      if (3 == x)
        n_min |= (((n_mask >> ((3 - x) * 8)) & (n_addr >> ((3 - x) * 8))) & 0xFF) + 1;
      else {
        n_min |= ((n_mask >> ((3 - x) * 8)) & (n_addr >> ((3 - x) * 8))) & 0xFF;
        n_min <<=  8;
      }
    }

    if (n_min > (n_addr + adjust))
     return -2;
  }

  n_addr += adjust;

  na_addr_htoa (n_addr, ret);

  return 0;
}

unsigned char *na_encode_no (unsigned long no, unsigned char *enc, unsigned int size)
{
  unsigned long	result = no;  // division result
  int           remainder = 0,
                i = size - 1;

  if (0 <= i) {
    // clear buffer
    memset (enc, 0, size);

    // divide by KID_DIVIDER and get remainder
    do {
      remainder = (int) (result % NA_ENC_NO_DIVIDER);
      result /= NA_ENC_NO_DIVIDER;

      enc [i--] = remainder + 35;
    }
    while (0 <= i &&
           NA_ENC_NO_DIVIDER <= result);
    if (0 <= i)
      enc [i--] = (int) result + 35;

    // pad with KID_PAD_CHR
    while (0 <= i)
      enc [i--] = NA_ENC_NO_PAD_CHR;
  }

  return enc;
}

unsigned long	na_decode_no (unsigned char *enc, unsigned int size)
{
  int i;
  unsigned int  len;
  unsigned long res,
                mul;

  if (!enc ||
      !size)
    return 0;

  // find out how many chars are actually being used
  len = 0;
  while (NA_ENC_NO_PAD_CHR == enc [len] && len < size)
    len++;

  // calculate the correct multiplier for the first digit
  mul = 1;
  for (i = 0; i < (size - len - 1); i++)
    mul *= NA_ENC_NO_DIVIDER;

  res = 0;
  for (i = len; i < size; i++) {
    res = res + (mul * (enc [i] - 35));
    mul /= NA_ENC_NO_DIVIDER;
  }

  return res;
}

