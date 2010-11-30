/*******************************************************************************
* This file is licensed under the terms of your license agreement(s) with
* GHL covering this file. Redistribution, except as permitted by the terms of
* your license agreement(s) with GHL, is strictly prohibited.
********************************************************************************/

#include "na_array.h"



/* ==========================================================================================
 * Description: this function is the structure initializer, everytime before use structure
 *              must be initialize using this function. this function will initialize the
 *              synchonization library.
 *
 * Parameter  : [IN  ] ctx - structure context
 *              [IN  ] size - maximum size of the array
 *              [IN  ] order - the order can be either index or fifo
 *                             NA_ARRAY_INDEX - use index based array
 *                             NA_ARRAY_FIFO - use FIFO based array
 *              [IN  ] flags - behavior flag of the array, the flags can be either
 *                             NA_ARRAY_FLAG_CHK_DUP - check duplicate id before insert
 *              [IN  ] delete - delete data function pointer. It can use to deallocate
 *                              custom memory before the deletion of item.
 *
 * Return     : 0 - success
 *              else - failure
 * ==========================================================================================
 */
int na_array_init (NA_ARRAY *ctx, int size, int order, int flags, NA_ARRAY_DEL delete)
{
  int index;

  if (0 > size || (NA_ARRAY_INDEX != order && NA_ARRAY_FIFO != order))
    return -1;

  na_array_term (ctx);

  ctx->size = size;
  ctx->order = order;
  ctx->flags = flags;
  ctx->delete = delete;

  if (!(NA_ARRAY_FLAG_NO_LOCK & ctx->flags))
    na_sync_init (&ctx->lock);

  MEM_ALLOC ((ctx->items), (ctx->size * sizeof (NA_ARRAY_ITEM)), (NA_ARRAY_ITEM *));

  for (index = 0; index < size; index++)
    ctx->items [index].index = index;

  ctx->init = 1;

  return 0;
}

/* ==========================================================================================
 * Description: this function is the structure destructor, everytime after use structure
 *              must be destroy using this function. this function will destroy all the item
 *              and synchonization library. after that it will set the magic number to it.
 *              the magic number is use to indicate the structure has been properly initialized,
 *              so that any termination can be done correctly.
 *
 * Parameter  : [IN  ] ctx - structure context
 *
 * Return     : 0 - success
 *              else - failure
 * ==========================================================================================
 */
void na_array_term (NA_ARRAY *ctx)
{
  int index, count;

  if (!ctx ||
      (NA_MAGIC == ctx->magic && !ctx->init))
    return;

  if (NA_MAGIC == ctx->magic && ctx->init) {
    if (!(NA_ARRAY_FLAG_NO_LOCK & ctx->flags))
      na_sync_lock (&ctx->lock);

    for (index = 0, count = 0; index < ctx->size && count < ctx->count; index++) {
      if (NA_ARRAY_ITEM_FLAG_USED & ctx->items [index].flags) {
        count++;

        if (NA_ARRAY_ITEM_FLAG_DEL & ctx->items [index].flags) {
          if (ctx->delete)
            ctx->delete (ctx, &ctx->items [index]);
          else
            MEM_CLEAN (ctx->items [index].data);
        }

        ctx->items [index].data = 0;
        ctx->items [index].flags = 0;
      }
    }

    MEM_CLEAN (ctx->items);

    if (!(NA_ARRAY_FLAG_NO_LOCK & ctx->flags))
      na_sync_term (&ctx->lock);
  }

  memset (ctx, 0, sizeof (NA_ARRAY));
  ctx->magic = NA_MAGIC;
}

/* ==========================================================================================
 * Description: this function use to lock the array to provide thread safe resource.
 *
 * Parameter  : [IN  ] ctx - structure context
 *
 * Return     : 0 - success
 *              else - failure
 * ==========================================================================================
 */
int na_array_lock (NA_ARRAY *ctx)
{
  if (!(NA_ARRAY_FLAG_NO_LOCK & ctx->flags))
    return na_sync_lock (&ctx->lock);

  return 0;
}

/* ==========================================================================================
 * Description: this function use to unlock the array to provide thread safe resource.
 *
 * Parameter  : [IN  ] ctx - structure context
 *
 * Return     : 0 - success
 *              else - failure
 * ==========================================================================================
 */
int na_array_unlock (NA_ARRAY *ctx)
{
  if (!(NA_ARRAY_FLAG_NO_LOCK & ctx->flags))
    return na_sync_unlock (&ctx->lock);

  return 0;
}

/* ==========================================================================================
 * Description: this function use to calculate the number of item
 *
 * Parameter  : [IN  ] ctx - structure context
 *
 * Return     : number of item
 * ==========================================================================================
 */
int na_array_count (NA_ARRAY *ctx)
{
  if (NA_MAGIC == ctx->magic && ctx->init)
    return ctx->count;

  return 0;
}

/* ==========================================================================================
 * Description: this function use to calculate the array size
 *
 * Parameter  : [IN  ] ctx - structure context
 *
 * Return     : size of array
 * ==========================================================================================
 */
int na_array_size (NA_ARRAY *ctx)
{
  if (NA_MAGIC == ctx->magic && ctx->init)
    return ctx->size;

  return 0;
}

/* ==========================================================================================
 * Description: this function use to add item into the array.
 *
 * Parameter  : [IN  ] ctx - structure context
 *              [IN  ] data - data to insert
 *              [IN  ] id - id for the item to ease searching (optional)
 *              [IN  ] allow_delete - the data can be delete by this array or not
 *                                    0: dont delete the data during deletion or termination
 *                                    1: delete the data during deletion or termination
 *              [OUT ] item - pointer of the inserted item (optional)
 *
 * Return     : 0 - success
 *              else - failure
 * ==========================================================================================
 */
int na_array_add (NA_ARRAY *ctx, void *data, char *id, int allow_delete, NA_ARRAY_ITEM **item)
{
  int           rv,
                index,
                count;

  NA_ARRAY_ITEM *ret = 0;

  if (NA_MAGIC != ctx->magic ||
      !ctx->init)
    return -1;

  if (!(NA_ARRAY_FLAG_NO_LOCK & ctx->flags)) {
    if (na_sync_lock (&ctx->lock))
      return -1;
  }

  if (ctx->count == ctx->size) {
    rv = -2;
    goto cleanup;
  }

  switch (ctx->order) {
  case NA_ARRAY_INDEX:
    // check duplicated id
    if (id && NA_ARRAY_FLAG_CHK_DUP & ctx->flags) {
      for (index = 0, count = 0; index < ctx->size && count < ctx->count; index++) {
        if (NA_ARRAY_ITEM_FLAG_USED & ctx->items [index].flags) {
          count++;
          if (!strcmp (ctx->items [index].id, id)) {
            rv = -3;
            goto cleanup;
          }
        }
      }
    }

    for (index = 0; index < ctx->size; index++) {
      if (!(NA_ARRAY_ITEM_FLAG_USED & ctx->items [index].flags))
        break;
    }
    break;

  case NA_ARRAY_FIFO:
    // calculate next index
    if (ctx->head != ctx->tail || 0 < ctx->count)
      index = (ctx->tail + 1) % ctx->size;
    else
      index = ctx->tail;

    if (NA_ARRAY_ITEM_FLAG_USED & ctx->items [index].flags)
      index = ctx->size;
    break;

  default:
    index = ctx->size;
  }

  if (index != ctx->size) {
    ret = &ctx->items [index];

    memset (ret->id, 0, sizeof (ret->id));
    if (id)
      strncpy (ret->id, id, sizeof (ret->id) - 1);

    ret->data = data;
    ret->flags = NA_ARRAY_ITEM_FLAG_USED;

    if (allow_delete)
      ret->flags |= NA_ARRAY_ITEM_FLAG_DEL;

    if (NA_ARRAY_FIFO == ctx->order)
      ctx->tail = index;
    ctx->count++;
  }

  if (item)
    *item = ret;
  rv = 0;

cleanup:
  if (!(NA_ARRAY_FLAG_NO_LOCK & ctx->flags))
    na_sync_unlock (&ctx->lock);

  return rv;
}

/* ==========================================================================================
 * Description: this function use to delete the first item from the array.
 *
 * Parameter  : [IN  ] ctx - structure context
 *              [OUT ] data - pointer of data to delete, if the pointer is provided then the
 *                            data will be return and caller is responsible to delete it
 *
 * Return     : 0 - success
 *              else - failure
 * ==========================================================================================
 */
int na_array_del (NA_ARRAY *ctx, void **data)
{
  int           rv,
                index,
                count;

  NA_ARRAY_ITEM *item   = 0;

  if (NA_MAGIC != ctx->magic ||
      !ctx->init)
    return -1;

  if (!(NA_ARRAY_FLAG_NO_LOCK & ctx->flags)) {
    if (na_sync_lock (&ctx->lock))
      return -1;
  }

  if (!ctx->count) {
    rv = -2;
    goto cleanup;
  }

  switch (ctx->order) {
  case NA_ARRAY_INDEX:
    for (index = 0, count = 0; index < ctx->size; index++) {
      if (NA_ARRAY_ITEM_FLAG_USED & ctx->items [index].flags) {
        item = &ctx->items [index];
        break;
      }
    }
    break;

  case NA_ARRAY_FIFO:
    if (NA_ARRAY_ITEM_FLAG_USED & ctx->items [ctx->head].flags) {
      index = ctx->head;
      item = &ctx->items [index];
    }
  }

  if (!item) {
    rv = -3;
    goto cleanup;
  }

  if (data)
    *data = item->data;
  else {
    if (NA_ARRAY_ITEM_FLAG_DEL & item->flags) {
      if (ctx->delete)
        ctx->delete (ctx, item);
      else
        MEM_CLEAN (item->data);
    }
  }

  memset (item, 0, sizeof (NA_ARRAY_ITEM));
  item->index = index;

  if (ctx->count)
    ctx->count--;
  rv = 0;

cleanup:
  if (!(NA_ARRAY_FLAG_NO_LOCK & ctx->flags))
    na_sync_unlock (&ctx->lock);

  return rv;
}

/* ==========================================================================================
 * Description: this function use to delete the item from the array according to index.
 *
 * Parameter  : [IN  ] ctx - structure context
 *              [IN  ] index - index of the data
 *              [OUT ] data - pointer of data to delete, if the pointer is provided then the
 *                            data will be return and caller is responsible to delete it
 *
 * Return     : 0 - success
 *              else - failure
 * ==========================================================================================
 */
int na_array_del_by_index (NA_ARRAY *ctx, int index, void **data)
{
  int           rv;

  NA_ARRAY_ITEM *item;

  if (NA_MAGIC != ctx->magic ||
      !ctx->init ||
      NA_ARRAY_INDEX != ctx->order ||
      0 > index || ctx->size <= index)
    return -1;

  if (!(NA_ARRAY_FLAG_NO_LOCK & ctx->flags)) {
    if (na_sync_lock (&ctx->lock))
      return -1;
  }

  item = &ctx->items [index];

  if (!(NA_ARRAY_ITEM_FLAG_USED & item->flags)) {
    rv = -2;
    goto cleanup;
  }

  if (data)
    *data = item->data;
  else {
    if (NA_ARRAY_ITEM_FLAG_DEL & item->flags) {
      if (ctx->delete)
        ctx->delete (ctx, item);
      else
        MEM_CLEAN (item->data);
    }
  }

  memset (item, 0, sizeof (NA_ARRAY_ITEM));
  item->index = index;

  if (ctx->count)
    ctx->count--;
  rv = 0;

cleanup:
  if (!(NA_ARRAY_FLAG_NO_LOCK & ctx->flags))
    na_sync_unlock (&ctx->lock);

  return rv;
}

/* ==========================================================================================
 * Description: this function use to delete the item from the array according to the id.
 *
 * Parameter  : [IN  ] ctx - structure context
 *              [IN  ] id - id of the data
 *              [OUT ] data - pointer of data to delete, if the pointer is provided then the
 *                            data will be return and caller is responsible to delete it
 *
 * Return     : 0 - success
 *              else - failure
 * ==========================================================================================
 */
int na_array_del_by_id (NA_ARRAY *ctx, char *id, void **data)
{
  int           rv,
                index, count;

  NA_ARRAY_ITEM *ptr,
                *item = 0;

  if (NA_MAGIC != ctx->magic ||
      !ctx->init ||
      NA_ARRAY_INDEX != ctx->order ||
      !id)
    return -1;

  if (!(NA_ARRAY_FLAG_NO_LOCK & ctx->flags)) {
    if (na_sync_lock (&ctx->lock))
      return -1;
  }

  for (index = 0, count = 0; index < ctx->size && count < ctx->count; index++) {
    ptr = &ctx->items [index];

    if (NA_ARRAY_ITEM_FLAG_USED & ptr->flags) {
      count++;
      if (!strcmp (ptr->id, id)) {
        item = ptr;
        break;
      }
    }
  }

  if (!item) {
    rv = -2;
    goto cleanup;
  }

  if (data)
    *data = item->data;
  else {
    if (NA_ARRAY_ITEM_FLAG_DEL & item->flags) {
      if (ctx->delete)
        ctx->delete (ctx, item);
      else
        MEM_CLEAN (item->data);
    }
  }

  memset (item, 0, sizeof (NA_ARRAY_ITEM));
  item->index = index;

  if (ctx->count)
    ctx->count--;
  rv = 0;

cleanup:
  if (!(NA_ARRAY_FLAG_NO_LOCK & ctx->flags))
    na_sync_unlock (&ctx->lock);

  return rv;
}

/* ==========================================================================================
 * Description: this function use to delete the all item from the array.
 *
 * Parameter  : [IN  ] ctx - structure context
 *
 * Return     : void
 * ==========================================================================================
 */
void na_array_del_all (NA_ARRAY *ctx)
{
  int           index, count;

  NA_ARRAY_ITEM *item;

  if (NA_MAGIC != ctx->magic ||
      !ctx->init)
    return;

  if (!(NA_ARRAY_FLAG_NO_LOCK & ctx->flags)) {
    if (na_sync_lock (&ctx->lock))
      return;
  }

  for (index = 0, count = 0; index < ctx->size && count < ctx->count; index++) {
    item = &ctx->items [index];

    if (NA_ARRAY_ITEM_FLAG_USED & item->flags) {
      count++;

      if (NA_ARRAY_ITEM_FLAG_DEL & item->flags) {
        if (ctx->delete)
          ctx->delete (ctx, item);
        else
          MEM_CLEAN (item->data);
      }
    }
  }

  memset (ctx->items, 0, ctx->size * sizeof (NA_ARRAY_ITEM));
  for (index = 0; index < ctx->size; index++)
    ctx->items [index].index = index;

  ctx->count = 0;
  ctx->head = 0;
  ctx->tail = 0;

  if (!(NA_ARRAY_FLAG_NO_LOCK & ctx->flags))
    na_sync_unlock (&ctx->lock);
}

/* ==========================================================================================
 * Description: this function use to retrieve item from the array according to index.
 *
 * Parameter  : [IN  ] ctx - structure context
 *              [IN  ] index - index of the data
 *              [OUT ] item - pointer of matched item (optional)
 *
 * Return     : 0 - success
 *              else - failure
 * ==========================================================================================
 */
int na_array_get_by_index (NA_ARRAY *ctx, int index, NA_ARRAY_ITEM **item)
{
  int rv;

  if (NA_MAGIC != ctx->magic ||
      !ctx->init ||
      0 > index || ctx->size <= index)
    return -1;

  if (!(NA_ARRAY_FLAG_NO_LOCK & ctx->flags)) {
    if (na_sync_lock (&ctx->lock))
      return -1;
  }

  if (!(NA_ARRAY_ITEM_FLAG_USED & ctx->items [index].flags)) {
    rv = -2;
    goto cleanup;
  }

  if (item)
    *item = &ctx->items [index];
  rv = 0;

cleanup:
  if (!(NA_ARRAY_FLAG_NO_LOCK & ctx->flags))
    na_sync_unlock (&ctx->lock);

  return rv;
}

/* ==========================================================================================
 * Description: this function use to retrieve item from the array according to id.
 *
 * Parameter  : [IN  ] ctx - structure context
 *              [IN  ] id - id of the data
 *              [OUT ] item - pointer of matched item (optional)
 *
 * Return     : 0 - success
 *              else - failure
 * ==========================================================================================
 */
int na_array_get_by_id (NA_ARRAY *ctx, char *id, NA_ARRAY_ITEM **item)
{
  int           rv,
                index, count;

  NA_ARRAY_ITEM *ret;

  if (NA_MAGIC != ctx->magic ||
      !ctx->init ||
      NA_ARRAY_INDEX != ctx->order ||
      !id)
    return -1;

  if (!(NA_ARRAY_FLAG_NO_LOCK & ctx->flags)) {
    if (na_sync_lock (&ctx->lock))
      return -1;
  }

  for (index = 0, count = 0; index < ctx->size && count < ctx->count; index++) {
    ret = &ctx->items [index];

    if (NA_ARRAY_ITEM_FLAG_USED & ret->flags) {
      count++;
      if (!strcmp (ret->id, id)) {
        if (item)
          *item = ret;

        rv = 0;
        goto cleanup;
      }
    }
  }

  rv = -2;

cleanup:
  if (!(NA_ARRAY_FLAG_NO_LOCK & ctx->flags))
    na_sync_unlock (&ctx->lock);

  return rv;
}

/* ==========================================================================================
 * Description: this function use to retrieve the first item from the array.
 *
 * Parameter  : [IN  ] ctx - structure context
 *
 * Return     : 0 - not found
 *              else - pointer of the first item
 * ==========================================================================================
 */
NA_ARRAY_ITEM *na_array_get_first (NA_ARRAY *ctx)
{
  int           index;

  NA_ARRAY_ITEM *item = 0;

  if (NA_MAGIC != ctx->magic ||
      !ctx->init)
    return item;

  if (!(NA_ARRAY_FLAG_NO_LOCK & ctx->flags)) {
    if (na_sync_lock (&ctx->lock))
      return item;
  }

  if (0 >= ctx->count)
    goto cleanup;

  switch (ctx->order) {
  case NA_ARRAY_INDEX:
    for (index = 0; index < ctx->size; index++) {
      if (NA_ARRAY_ITEM_FLAG_USED & ctx->items [index].flags)
        break;
    }
    break;

  case NA_ARRAY_FIFO:
    if (NA_ARRAY_ITEM_FLAG_USED & ctx->items [ctx->head].flags)
      index = ctx->head;
    else
      index = ctx->size;
    break;

  default:
    index = ctx->size;
  }

  if (index != ctx->size)
    item = &ctx->items [index];

cleanup:
  if (!(NA_ARRAY_FLAG_NO_LOCK & ctx->flags))
    na_sync_unlock (&ctx->lock);

  return item;
}

/* ==========================================================================================
 * Description: this function use to retrieve the next item from the array.
 *
 * Parameter  : [IN  ] ctx - structure context
 *              [IN  ] item - the current item, if NULL is provided then it will return the
 *                            first item if available
 *
 * Return     : 0 - not found
 *              else - pointer of the next item
 * ==========================================================================================
 */
NA_ARRAY_ITEM *na_array_get_next (NA_ARRAY *ctx, NA_ARRAY_ITEM *item)
{
  int           index;

  if (NA_MAGIC != ctx->magic ||
      !ctx->init) {
    item = 0;
    return item;
  }

  if (!(NA_ARRAY_FLAG_NO_LOCK & ctx->flags)) {
    if (na_sync_lock (&ctx->lock)) {
      item = 0;
      return item;
    }
  }

  if (0 >= ctx->count) {
    item = 0;
    goto cleanup;
  }

  switch (ctx->order) {
  case NA_ARRAY_INDEX:
    if (!item) {
      for (index = 0; index < ctx->size; index++) {
        if (NA_ARRAY_ITEM_FLAG_USED & ctx->items [index].flags) {
          item = &ctx->items [index];
          break;
        }
      }
    }
    else {
      for (index = item->index + 1; index < ctx->size; index++) {
        if (NA_ARRAY_ITEM_FLAG_USED & ctx->items [index].flags) {
          item = &ctx->items [index];
          break;
        }
      }
      if (index == ctx->size)
        item = 0;
    }
    break;

  case NA_ARRAY_FIFO:
    if (!item &&
        NA_ARRAY_ITEM_FLAG_USED & ctx->items [ctx->head].flags) {
      item = &ctx->items [ctx->head];
    }
    else {
      index = item->index;
      item = 0;

      if (index != ctx->tail) {
        index = (index + 1) % ctx->size;
        if (NA_ARRAY_ITEM_FLAG_USED & ctx->items [index].flags)
          item = &ctx->items [index];
      }
    }
    break;

  default:
    item = 0;
  }

cleanup:
  if (!(NA_ARRAY_FLAG_NO_LOCK & ctx->flags))
    na_sync_unlock (&ctx->lock);

  return item;
}

