#pragma once
#include "common.h"
#include "result.h"
#include <stdbool.h>
#include <stddef.h>

/// @file shelf.h
/// @brief interanl utility concerning shelfs

/// @brief create a shelf storage location
/// @param shelf_name name of the shelf
/// @param merch_id merch id to store on the shelf
/// @parma quantitiy how many of merch_id to store on the shelf
/// @return a new self storage location with the specified paramters
shelf_storage_t *create_shelf_storage(shelf_name_t shelf_name, size_t merch_id,
                                      size_t quantity);

/// @brief create a shelf storage location
/// @param storage storage location to replenish merch on, must not be NULL
/// @param merch_id merch id to store on the shelf
/// @param quantitiy how many of merch_id to store on the shelf
/// @return if successful the is_ok field is set to true and the error field
/// should be ignored, if is_ok is set to false the error field represents what
/// went wrong
replenish_result_t replenish_merch(shelf_storage_t *storage, size_t merch_id,
                                   size_t quantitiy);
