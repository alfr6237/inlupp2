#pragma once

#include "../../vendor/hash_table.h"
#include "result.h"
#include <stddef.h>

///@file common.h
///@brief common type declarations needed throughout the backend

/// @brief represents the entirety of a webstore.
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
} ioopm_store_t;

/// @brief representation of a merch
/// @note owns its name and desc strings and thus merch_destroy should be called
/// to clean up
typedef struct merch {
  char *name;
  char *desc;
  size_t price;
} ioopm_merch_t;

/// @brief explicit representation of a shelf as on character followed by a
/// number.
typedef struct shelf_name {
  char c;
  size_t n;
} ioopm_shelf_name_t;

/// @brief an internal representation of a shelf storage location where merch
/// can be stocked
typedef struct shelf_storage {
  ioopm_shelf_name_t shelf_name;
  size_t merch_id;
  size_t quantity;
} ioopm_shelf_storage_t;

/// @brief a public representation of shelf,
/// @note it is asumed that the merch stored on this shelf is know
/// from other context eg the store's locations hash_table.
typedef struct shelf {
  ioopm_shelf_name_t shelf_name;
  size_t quantity;
} ioopm_shelf_t;

/// @brief the possible errors that can occur due to invalid parameters when
/// replenishing merch
typedef enum replenish_error {
  MERCH_CONFLICT,
  MERCH_NOT_FOUND
} ioopm_replenish_error_t;

/// @brief the possible errors that can occur due to invalid parameters when
/// editing merch
typedef enum edit_error {
  NEW_NAME_EXIST,
  OLD_NAME_NOT_FOUND
} ioopm_edit_error_t;

/// @brief the possible errors that can occur due to invalid parameters when
/// adding or removing merch from a cart
///
/// @note MERCH_NOT_IN_CART is not a possible error when adding merch
typedef enum cart_error {
  CART_NOT_FOUND,
  QUANTITY_TOO_LARGE,
  MERCH_NOT_IN_STORE,
  MERCH_NOT_IN_CART, /// only applicable to removeing merch from a cart
} ioopm_cart_error_t;

/// result types so that errors can be returned and checked in a uniform way
typedef RESULT_VOID(ioopm_edit_error_t) ioopm_edit_result_t;
typedef RESULT_VOID(ioopm_cart_error_t) ioopm_cart_result_t;
typedef RESULT_VOID(ioopm_replenish_error_t) ioopm_replenish_result_t;
