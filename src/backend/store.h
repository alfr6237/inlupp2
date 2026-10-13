#pragma once
#include "../../vendor/hash_table.h"
#include "merch.h"
#include "result.h"
#include <stddef.h>

/// struct for location includeing quantity and merch instance

/// 4 hash tables

/// hash / eql fn for merch and hylla

typedef enum replenish_error {
  MERCH_CONFLICT,
  MERCH_NOT_FOUND
} replenish_error_t;

typedef RESULT_VOID(replenish_error_t) replenish_result_t;

typedef enum get_error { NOT_FOUND } get_error_t;

typedef struct shelf_storage {
  const char *merch_name;
  size_t quantity;
} shelf_storage_t;

typedef struct store ioopm_store_t;
typedef struct shelf ioopm_shelf_t;

typedef RESULT(shelf_storage_t, get_error_t) get_shelf_storage_result_t;

struct shelf {
  char c;
  size_t n;
};

struct store {
  ioopm_hash_table_t *merch_ids; // merch name to merch_id
  ioopm_hash_table_t *shelf_ids; // shelf to shelf_id

  ioopm_hash_table_t *shelfs; // shelf_id to shelf_storage
  ioopm_hash_table_t *merch;  // merch_id to merch

  size_t current_id;
};

ioopm_store_t *ioopm_store_create(void);
void ioopm_store_destroy(ioopm_store_t *store);

bool ioopm_merch_exists(ioopm_store_t *store, const char *merch_name);

bool ioopm_store_add_merch(ioopm_store_t *store, const char *name,
                           const char *desc, int price);

ioopm_merch_t *ioopm_store_get_merch(ioopm_store_t *store,
                                     const char *merch_name);

// returns false if there exist other type of merch on the specified shelf or
// quantity is 0
replenish_result_t ioopm_store_replenish_merch(ioopm_store_t *store,
                                               const char *merch_name,
                                               const ioopm_shelf_t shelf,
                                               size_t quantity);

bool ioopm_store_remove_merch(ioopm_store_t *store, const char *merch_name);

void ioopm_store_edit_merch_name(ioopm_store_t *store,
                                 const char *old_merch_name,
                                 const char *new_merch_name);

void ioopm_store_edit_merch_desc(ioopm_store_t *store, const char *merch_name,
                                 const char *new_desc);
void ioopm_store_edit_price(ioopm_store_t *store, const char *merch_name,
                            size_t new_price);

get_shelf_storage_result_t ioopm_get_shelf_storage(ioopm_store_t *store,
                                                   const ioopm_shelf_t shelf);
