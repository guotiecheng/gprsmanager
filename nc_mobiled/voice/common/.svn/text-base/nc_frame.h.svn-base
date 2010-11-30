#ifndef NC_FRAME_H
#define NC_FRAME_H

#ifdef __cplusplus
extern "C" {
#endif

#define NC_FRAME_HEADER_LEN       2
#define NC_FRAME_MIN_LEN          28
#define NC_FRAME_LEN              NC_FRAME_HEADER_LEN + NC_FRAME_MIN_LEN
#define NC_FRAME_MAX_APP_NAME     10


// ============================================================================================
// ** FRAME FORMAT                                                                           **
// ** 2B            | 10B     1B   | 10B     1B   | 1B        | 2B    | 1B  | 2B     | XB    **
// ** header length | dst_app NULL | src_app NULL | operation | flags | LRC | length | data  **
// ============================================================================================

typedef struct NC_FRAME_T
{
  char          *dst_app,
                *src_app;

  unsigned char ops;

  unsigned int  flags,
                dat_len,
                raw_len;

  unsigned char *raw_frm,
                *raw_end,
                *dat;
}
NC_FRAME;

int nc_frame_parse (NC_FRAME *ctx, unsigned char *buf, unsigned int len, unsigned int *used);
int nc_frame_build (NC_FRAME *ctx, unsigned char *buf, unsigned int size, unsigned int *used);

#ifdef __cplusplus
}
#endif

#endif

