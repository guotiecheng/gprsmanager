#ifndef NA_ARRAY_H
#define NA_ARRAY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "na_common.h"

#define NA_ARRAY_ID_SIZE          64

#define NA_ARRAY_INDEX            1
#define NA_ARRAY_FIFO             2

#define NA_ARRAY_ITEM_FLAG_USED   1
#define NA_ARRAY_ITEM_FLAG_DEL    2

#define NA_ARRAY_FLAG_CHK_DUP     1
#define NA_ARRAY_FLAG_NO_LOCK     2

// ========================
// ** EXTERNAL FUNCTIONS **
// ========================

typedef void (*NA_ARRAY_DEL) (void *_ctx, void *_data);


// ==========================
// ** ARRAY DATA STRUCTURE **
// ==========================

typedef struct NA_ARRAY_ITEM_T
{
  int               index;

  unsigned int      flags;

  // flag definitions
  // 0 ... 0 0 0 0 0 0 0 0
  // |     | | | | | | | |___________ used flag - 0: free, 1: occupied
  // |     | | | | | | |_____________ delete flag - 0: dont delete, 1: can delete
  // | ... |_|_|_|_|_|_______________ reserved

  char  id [NA_ARRAY_ID_SIZE + 1];

  void  *data;
}
NA_ARRAY_ITEM;

typedef struct NA_ARRAY_T
{
  int               magic,
                    init,
                    size,
                    count,
                    head, tail,
                    order;

  unsigned int      flags;

  // flag definitions
  // 0 ... 0 0 0 0 0 0 0 0
  // |     | | | | | | | |___________ duplicate flag - 0: not allow duplicate id, 1: allow duplicate id
  // | ... |_|_|_|_|_|_|_____________ reserved

  NA_SYNC           lock;
  NA_ARRAY_DEL      delete;

  NA_ARRAY_ITEM     *items;
}
NA_ARRAY;


// =====================
// ** PUBLIC FUNCTION **
// =====================

int na_array_init (NA_ARRAY *ctx, int size, int order, int flags, NA_ARRAY_DEL delete);
void na_array_term (NA_ARRAY *ctx);

int na_array_lock (NA_ARRAY *ctx);
int na_array_unlock (NA_ARRAY *ctx);

int na_array_count (NA_ARRAY *ctx);
int na_array_size (NA_ARRAY *ctx);

int na_array_add (NA_ARRAY *ctx, void *data, char *id, int allow_delete, NA_ARRAY_ITEM **item);
int na_array_del (NA_ARRAY *ctx, void **data);
int na_array_del_by_index (NA_ARRAY *ctx, int index, void **data);
int na_array_del_by_id (NA_ARRAY *ctx, char *id, void **data);
void na_array_del_all (NA_ARRAY *ctx);

int na_array_get_by_index (NA_ARRAY *ctx, int index, NA_ARRAY_ITEM **item);
int na_array_get_by_id (NA_ARRAY *ctx, char *id, NA_ARRAY_ITEM **item);

NA_ARRAY_ITEM *na_array_get_first (NA_ARRAY *ctx);
NA_ARRAY_ITEM *na_array_get_next (NA_ARRAY *ctx, NA_ARRAY_ITEM *item);

#ifdef __cplusplus
}
#endif

#endif
