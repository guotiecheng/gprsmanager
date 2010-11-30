/*******************************************************************************
* This file is licensed under the terms of your license agreement(s) with
* GHL covering this file. Redistribution, except as permitted by the terms of
* your license agreement(s) with GHL, is strictly prohibited.
********************************************************************************/

#include "nc_frame.h"
#include "libna.h"

/* ==========================================================================================
 * Description: parse system frame into structure. the raw buffer must not be move/clear before
 *              use, otherwise the data of the structure is not correctly pointed.
 *
 * Parameter  : [IN  ] ctx - structure context
 *              [IN  ] buf - received buffer
 *              [IN  ] len - received length
 *              [OUT ] used - parsed length
 *
 * Return     : 0 - success
 *              else - failure
 * ==========================================================================================
 */
int nc_frame_parse (NC_FRAME *ctx, unsigned char *buf, unsigned int len, unsigned int *used)
{
  int rv;
  unsigned int  hlen;
  unsigned char lrc_r,  // received lrc
                lrc_c,  // calculated lrc
                *lrc_ptr,
                *ptr;

  if (NC_FRAME_HEADER_LEN > len ||
      len < (hlen = na_byte_to_uint (buf, NC_FRAME_HEADER_LEN)) ||
      NC_FRAME_MIN_LEN > hlen) {
    if (used)
      *used = 0;

    return -1;
  }

  ptr = buf + NC_FRAME_HEADER_LEN;

  // destination app name
  ctx->dst_app = (char *) ptr;
  ptr += (NC_FRAME_MAX_APP_NAME + 1);

  // source app name
  ctx->src_app = (char *) ptr;
  ptr += (NC_FRAME_MAX_APP_NAME + 1);

  // operation
  ctx->ops = *(ptr++);

  // flags
  ctx->flags = na_byte_to_uint (ptr, 2);
  ptr += 2;

  // lrc
  lrc_ptr = ptr++;
  lrc_r = *lrc_ptr;
  *lrc_ptr = 0;

  // length
  ctx->dat_len = na_byte_to_uint (ptr, 2);
  ptr += 2;

  // value
  ctx->dat = ptr;

  // raw from, end pointer
  ctx->raw_len = NC_FRAME_HEADER_LEN + hlen;
  ctx->raw_frm = buf;
  ctx->raw_end = ctx->raw_frm + ctx->raw_len;

  if (used)
    *used = ctx->raw_end - ctx->raw_frm;

  // calculate lrc
  lrc_c = na_lrc (ctx->raw_frm, NC_FRAME_LEN, 0);
  if (ctx->dat &&
      ctx->dat_len) {
    lrc_c = na_lrc (ctx->dat, ctx->dat_len, lrc_c);
  }

  if (lrc_r != lrc_c)
    rv = -2;
  else
    rv = 0;

  *lrc_ptr = lrc_r;

  return rv;
}

/* ==========================================================================================
 * Description: build system frame header, the caller should append the user data after system
 *              frame header.
 *
 * Parameter  : [IN  ] hbuf - header buffer
 *              [IN  ] hsize - header maximum buffer size
 *              [OUT ] hlen - builded header length
 *              [IN  ] app_name - application name
 *              [IN  ] ops - operation
 *              [IN  ] flags - flags
 *              [IN  ] len - user defined data length
 *
 * Return     : 0 - success
 *              else - failure
 * ==========================================================================================
 */
int nc_frame_build (NC_FRAME *ctx, unsigned char *buf, unsigned int size, unsigned int *used)
{
  unsigned char lrc_c,
                *lrc_ptr,
                *ptr  = buf;

  memset (ptr, 0, size);

  // header length
  na_ushort_to_byte (NC_FRAME_MIN_LEN + ctx->dat_len, ptr);
  ptr += NC_FRAME_HEADER_LEN;

  // destination app name
  if (ctx->dst_app)
    strncpy ((char *) ptr, ctx->dst_app, NC_FRAME_MAX_APP_NAME);
  ptr += (NC_FRAME_MAX_APP_NAME + 1);

  // source app name
  if (ctx->src_app)
    strncpy ((char *) ptr, ctx->src_app, NC_FRAME_MAX_APP_NAME);
  ptr += (NC_FRAME_MAX_APP_NAME + 1);

  // operation
  *(ptr++) = ctx->ops;

  // flags
  na_ushort_to_byte ((unsigned short) ctx->flags, ptr);
  ptr += 2;

  // lrc
  lrc_ptr = ptr++;

  // length
  na_ushort_to_byte ((unsigned short) ctx->dat_len, ptr);
  ptr += 2;

  // calculate lrc
  lrc_c = na_lrc (buf, NC_FRAME_LEN, 0);
  if (ctx->dat &&
      ctx->dat_len) {
    lrc_c = na_lrc (ctx->dat, ctx->dat_len, lrc_c);
  }

  *lrc_ptr = lrc_c;

  *used = NC_FRAME_LEN;

  return 0;
}

