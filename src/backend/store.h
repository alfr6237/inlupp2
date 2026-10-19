#pragma once
#include "../../vendor/hash_table.h"
#include "merch.h"
#include "result.h"
#include "shelf.h"
#include <stddef.h>

/// struct for location includeing quantity and merch instance

/// 4 hash tables

/// hash / eql fn for merch and hylla

// typedef enum replenish_error {
//   MERCH_CONFLICT,
//   MERCH_NOT_FOUND
// } replenish_error_t;

typedef enum edit_error { NEW_NAME_EXIST, OLD_NAME_NOT_FOUND } edit_error_t;
typedef RESULT_VOID(edit_error_t) edit_result_t;

// typedef RESULT_VOID(replenish_error_t) replenish_result_t;
typedef enum serch_error { NOT_FOUND } search_error_t;

typedef struct store ioopm_store_t;
// typedef struct shelf_name shelf_name_t;

// struct shelf_name {
//   char c;
//   size_t n;
// };
//
// typedef struct shelf_storage {
//   shelf_name_t shelf_name;
//   size_t merch_id;
//   size_t quantity;
// } shelf_storage_t;
//
// typedef struct shelf {
//   shelf_name_t shelf_name;
//   size_t quantity;
// } shelf_t;
//
// typedef struct shelf_stock {
//   const char *merch_name;
//   size_t quantity;
// } shelf_stock_t;

typedef RESULT(shelf_storage_t, search_error_t) get_shelf_storage_result_t;

struct store {
  ioopm_hash_table_t *merch_ids; // merch name to merch_id
  ioopm_hash_table_t *shelf_ids; // shelf to shelf_id

  ioopm_hash_table_t *shelfs;    // shelf_id to shelf_storage
  ioopm_hash_table_t *merch;     // merch_id to merch
  ioopm_hash_table_t *locations; // merch_id to link list of shelf_id

  size_t current_id;
};

ioopm_store_t *ioopm_store_create(void);
void ioopm_store_destroy(ioopm_store_t *store);

bool ioopm_store_add_merch(ioopm_store_t *store, const char *name,
                           const char *desc, int price);

const ioopm_merch_t *ioopm_store_get_merch(ioopm_store_t *store,
                                           const char *merch_name);

// returns false if there exist other type of merch on the specified shelf or
// quantity is 0
replenish_result_t ioopm_store_replenish_merch(ioopm_store_t *store,
                                               const char *merch_name,
                                               const shelf_name_t shelf,
                                               size_t quantity);

bool ioopm_store_remove_merch(ioopm_store_t *store, const char *merch_name);

edit_result_t ioopm_store_edit_merch(ioopm_store_t *store, const char *old_name,
                                     const char *new_name, const char *new_desc,
                                     const size_t new_price);

ioopm_list_t *ioopm_store_get_stock(const ioopm_store_t *store,
                                    const char *merch_name);

// shelf_t ioopm_store_get_shelf_stock(const ioopm_store_t *store,
//                                     const shelf_name_t shelf_name);
