#pragma once
#include "result.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct shelf_name {
  char c;
  size_t n;
} shelf_name_t;

typedef struct shelf_storage {
  shelf_name_t shelf_name;
  size_t merch_id;
  size_t quantity;
} shelf_storage_t;

typedef struct shelf {
  shelf_name_t shelf_name;
  size_t quantity;
} shelf_t;

typedef enum replenish_error {
  MERCH_CONFLICT,
  MERCH_NOT_FOUND
} replenish_error_t;

typedef RESULT_VOID(replenish_error_t) replenish_result_t;

shelf_storage_t *create_shelf_storage(shelf_name_t shelf_name, size_t merch_id,
                                      size_t quantity);
void destotoy_shelf_storage(shelf_storage_t *storage);

replenish_result_t replenish_merch(shelf_storage_t *storage, size_t merch_id,
                                   size_t quantitiy);
