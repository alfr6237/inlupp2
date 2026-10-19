#include "shelf.h"
#include <stdlib.h>

shelf_storage_t *create_shelf_storage(shelf_name_t shelf_name, size_t merch_id,
                                      size_t quantity) {

  shelf_storage_t *storage = calloc(1, sizeof(shelf_storage_t));
  storage->merch_id = merch_id;
  storage->quantity = quantity;
  storage->shelf_name = shelf_name;

  return storage;
}
void destotoy_shelf_storage(shelf_storage_t *storage) { free(storage); }

replenish_result_t replenish_merch(shelf_storage_t *storage, size_t merch_id,
                                   size_t quantitiy) {
  if (merch_id != storage->merch_id) {
    return (replenish_result_t){.is_ok = false, .error = MERCH_CONFLICT};
  }

  storage->quantity += quantitiy;

  return (replenish_result_t){.is_ok = true};
}
