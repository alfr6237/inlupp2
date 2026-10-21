#pragma once
#include "../../vendor/hash_table.h"
#include "merch.h"
#include "result.h"
#include "shelf.h"
#include <stddef.h>

typedef enum edit_error { NEW_NAME_EXIST, OLD_NAME_NOT_FOUND } edit_error_t;
typedef RESULT_VOID(edit_error_t) edit_result_t;

typedef enum serch_error { NOT_FOUND } search_error_t;

typedef struct store store_t;

typedef RESULT(shelf_storage_t, search_error_t) get_shelf_storage_result_t;

/// @description store represents the entire backend of the program and is
/// responsible for coordinating merches shelves and later carts when
/// implemented.
struct store {
  ioopm_hash_table_t *merch_ids; // merch name to merch_id
  ioopm_hash_table_t *shelf_ids; // shelf to shelf_id

  ioopm_hash_table_t *shelfs;    // shelf_id to shelf_storage
  ioopm_hash_table_t *merch;     // merch_id to merch
  ioopm_hash_table_t *locations; // merch_id to link list of shelf_id

  size_t current_id;
};

/// @brief creates a store
/// @return a pointer to a initialized store_t with no merch or shelves
store_t *store_create(void);

/// @brief destroys a store
/// @param store the store to be destroyed
void store_destroy(store_t *store);

/// @brief adds a merchandise to the store
/// @param store the store to operate on
/// @param name the name of the merchandise
/// @param desc a description of the merchandise
/// @param price the price of the merchandise
/// @return if the merch was successfully added
///
///
/// @note this does not add any stock of the merhcendise to any shelf
/// @note if the merchandise was not successfully added there already exists a
/// merchandise with the same name
bool store_add_merch(store_t *store, const char *name, const char *desc,
                     const size_t price);

/// @brief removes merch from the store
/// @param store the store to operate on
/// @param merch_name the name of the merch ot be removed
/// @return if the merch was removed or not
bool store_remove_merch(store_t *store, const char *merch_name);

/// @brief retrives a pointer to a merch_t in the store given the merchandises
/// name
/// @param store the store to operate on
/// @param merch_name the name of the merchandise
/// @return the merch coresponding to the merch_name or NULL if merch_name
/// is not recognized by the store
const merch_t *store_get_merch(store_t *store, const char *merch_name);

/// @brief adds merch to a shelf
/// @param store the store to operate on
/// @param merch_name the name of the merchandise
/// @param shelf_name name/code of the shelf to replenish on
/// @param quantity how many of the given merch to add to the given shelf
/// @return if successful the is_ok field is set to true and the error field
/// should be ignored, if is_ok is set to false the error field represents what
/// went wrong
replenish_result_t store_replenish_merch(store_t *store, const char *merch_name,
                                         const shelf_name_t shelf_name,
                                         const size_t quantity);

/// @brief edit a merchandise in the store
/// @param store the store to operate on
/// @param current_name the current name of the merchandise so we can find it
/// @param new_name the new name to give to the merchandise
/// @param new_desc the new description to give to the merchandise
/// @param price_desc the new price to give to the merchandise
/// @return if successful the is_ok field is set to true and the error field
/// should be ignored, if is_ok is set to false the error field represents what
/// went wrong
edit_result_t store_edit_merch(store_t *store, const char *current_name,
                               const char *new_name, const char *new_desc,
                               const size_t new_price);

/// @brief get the shelf name and how much merch is stored on that shelf for
/// every shelf that stocks this merch
/// @param store the store to operate on
/// @param merch_name the name of the merchandise
/// @return a linked list of shelf_t* that contains the shelf name and quantity
/// for every shelf that stocks this merch
///
/// @note the caller of this functions owns the returned list and is responsible
/// for calling ioopm_list_destroy('returned list'); to avoid memory leaks
ioopm_list_t *store_get_stock(const store_t *store, const char *merch_name);
