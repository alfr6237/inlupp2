#pragma once
#include "../../vendor/hash_table.h"
#include "common.h"

/// @file cart.h
/// @brief file that handles the removal of cart hashtables inside the store

/// @brief frees up allocated memory ocupied by cart hashtable in store
/// @param store store to be operated upon
void ioopm_destroy_cart_table(ioopm_store_t *store);
