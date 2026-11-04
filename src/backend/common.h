#pragma once

#include "../../vendor/hash_table.h"
#include "result.h"
#include <stddef.h>

typedef struct store {
  ioopm_hash_table_t *merch_ids; // merch name to merch_id
  ioopm_hash_table_t *shelf_ids; // shelf to shelf_id

  ioopm_hash_table_t *shelfs;    // shelf_id to shelf_storage
  ioopm_hash_table_t *merch;     // merch_id to merch
  ioopm_hash_table_t *locations; // merch_id to link list of shelf_id
  ioopm_hash_table_t *
      reserved_stock; // merch_id to how much of this merch is reserved by carts
  ioopm_hash_table_t *carts; // cart_id to cart

  size_t current_id;      // id for shelves and merchandise
  size_t current_cart_id; // id for carts
} store_t;

typedef struct merch {
  char *name;
  char *desc;
  size_t price;
} merch_t;

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

typedef enum edit_error { NEW_NAME_EXIST, OLD_NAME_NOT_FOUND } edit_error_t;

typedef enum serch_error { NOT_FOUND } search_error_t;

typedef enum cart_error {
  CART_NOT_FOUND,
  QUANTITY_TOO_LARGE,
  MERCH_NOT_IN_STORE,
  MERCH_NOT_IN_CART,
} cart_error_t;

typedef RESULT(shelf_storage_t, search_error_t) get_shelf_storage_result_t;
typedef RESULT_VOID(edit_error_t) edit_result_t;
typedef RESULT_VOID(cart_error_t) cart_result_t;
typedef RESULT_VOID(replenish_error_t) replenish_result_t;
