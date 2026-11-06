#include "shelf.h"
#include <stdio.h>
#include <stdlib.h>

ioopm_shelf_storage_t *ioopm_create_shelf_storage(ioopm_shelf_name_t shelf_name,
                                                  size_t merch_id,
                                                  size_t quantity) {

  ioopm_shelf_storage_t *storage = calloc(1, sizeof(ioopm_shelf_storage_t));
  storage->merch_id = merch_id;
  storage->quantity = quantity;
  storage->shelf_name = shelf_name;

  return storage;
}
void ioopm_destotoy_shelf_storage(ioopm_shelf_storage_t *storage) {
  free(storage);
}

ioopm_replenish_result_t ioopm_replenish_merch(ioopm_shelf_storage_t *storage,
                                               size_t merch_id,
                                               size_t quantity) {
  if (merch_id != storage->merch_id) {
    return (ioopm_replenish_result_t){.is_ok = false, .error = MERCH_CONFLICT};
  }

  storage->quantity += quantity;

  return (ioopm_replenish_result_t){.is_ok = true};
}
