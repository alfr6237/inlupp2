#pragma once
#include "../../vendor/hash_table.h"
#include "common.h"

/// @file cart.h
/// @brief file that handles the removal of cart hashtables inside the store

/// @brief frees up allocated memory ocupied by cart hashtable in store
/// @param store store to be operated upon
void destroy_cart_table(store_t *store);

/// @brief removes and frees up allocated memory of specified cart in store
/// @param store store to be operated upon
/// @param cart  cart hashtable to free up
void remove_cart(store_t *store, ioopm_hash_table_t *cart);
