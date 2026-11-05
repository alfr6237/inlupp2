#pragma once
#include "common.h"
#include "result.h"
#include <stdbool.h>
#include <stddef.h>

shelf_storage_t *create_shelf_storage(shelf_name_t shelf_name, size_t merch_id,
                                      size_t quantity);
void destotoy_shelf_storage(shelf_storage_t *storage);

replenish_result_t replenish_merch(shelf_storage_t *storage, size_t merch_id,
                                   size_t quantitiy);
