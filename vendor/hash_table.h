#pragma once
#include "common.h"
#include "hash_table_common.h"
#include <stdbool.h>
#include <stddef.h>

/**
 * @file hash_table.h
 * @author write both your names here
 * @date 24 september 2026
 * @brief Simple hash table data structure
 *
 *Basic functions for implementing a hash table data structure
 *
 * Including: 'create', 'destroy', 'insert', 'lookup' and 'remove'.
 * Hash/equality functions are provided by the user and keys/values
 * are stored as a union type 'elem_t' (defined in 'common.h')
 *
 **/

/// @brief Create a new hash table
/// @param hash_fn hash_function to hash the keys in the hash table
/// @param key_eql_fn fucntion to check for equality betwen keys
/// @return A new empty hash table
ioopm_hash_table_t *ioopm_hash_table_create(ioopm_hash_function *hash_fn,
                                            ioopm_eq_function *key_eq_fn);

/// @brief Delete a hash table and free its memory
/// @param ht a hash table to be deleted
void ioopm_hash_table_destroy(ioopm_hash_table_t *ht);

/// @brief add key => value entry in hash table ht
/// @param ht hash table operated upon
/// @param key key to insert
/// @param value value to insert
void ioopm_hash_table_insert(ioopm_hash_table_t *ht, elem_t key, elem_t value);

/// @brief lookup value for key in hash table ht
/// @param ht hash table operated upon
/// @param key key to lookup
/// @param out paramter for the value to be looked up
/// @return if the value was succesfully looked up
bool ioopm_hash_table_lookup(ioopm_hash_table_t *ht, elem_t key,
                             elem_t *result);

/// @brief remove any mapping from key to a value
/// @param ht hash table operated upon
/// @param key key to remove
/// @param out prameter for the value corresponding to the key if the returned
/// value is true
/// @return if the value was succesfully removed
bool ioopm_hash_table_remove(ioopm_hash_table_t *ht, elem_t key,
                             elem_t *result);

/// @breif checks if the key exists in the hash table
/// @param ht hash table operated upon
/// @param key to check existans of
/// @return if the key exists
bool ioopm_hash_table_has_key(ioopm_hash_table_t *ht, elem_t key);

/// @breif checks if the hash table is empty, ie has no entries
/// @param ht hash table operated upon
/// @return if the hash table was empty
bool ioopm_hash_table_is_empty(ioopm_hash_table_t *ht);

/// @breif returns the number of entries in the hash table
/// @param ht hash table operated upon
/// @return number of entries in the hash table
size_t ioopm_hash_table_size(ioopm_hash_table_t *ht);
