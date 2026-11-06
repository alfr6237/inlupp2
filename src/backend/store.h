#pragma once
#include "../../vendor/hash_table.h"
#include "common.h"
#include "merch.h"
#include "../../vendor/linked_list.h"
#include <stddef.h>

///@file store.h
///@brieg An in memeory representation of a webstore that can be hooked up
/// to a fronted to drive it.

/// @brief creates a store
/// @return a pointer to a initialized store_t with no merch or shelves
store_t *store_create(void);

/// @brief destroys a store
/// @param store the store to be destroyed
void store_destroy(store_t *store);

/// @brief adds a merchandise to the store
/// @param store the store to operate on, must not be NULL
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
/// @param store the store to operate on, must not be NULL
/// @param merch_name the name of the merch ot be removed
/// @return if the merch was removed or not
bool store_remove_merch(store_t *store, const char *merch_name);

/// @brief returns a list of all the merch added to the store
/// @param store the store to operate on, must not be NULL
/// @returns list of the merch in the store in alphabetical order
ioopm_list_t *store_merch_list(const store_t *store);

/// @brief retrives a pointer to a merch_t in the store given the merchandises
/// name
/// @param store the store to operate on, must not be NULL
/// @param merch_name the name of the merchandise
/// @return the merch coresponding to the merch_name or NULL if merch_name
/// is not recognized by the store
const merch_t *store_get_merch(store_t *store, const char *merch_name);

/// @brief adds merch to a shelf
/// @param store the store to operate on, must not be NULL
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
/// @param store the store to operate on, must not be NULL
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
/// @param store the store to operate on, must not be NULL
/// @param merch_name the name of the merchandise
/// @return a linked list of shelf_t* that contains the shelf name and quantity
/// for every shelf that stocks this merch
///
/// @note the caller of this functions owns the returned list and is responsible
/// for calling ioopm_list_destroy('returned list'); to avoid memory leaks
ioopm_list_t *store_get_stock(const store_t *store, const char *merch_name);

/// @brief add a new cart to the store
/// @param store the store to operate on, must not be NULL
/// @return the id that refers to the new cart
size_t store_add_cart(store_t *store);

/// @brief removes an existing cart from the store
/// @param store the store to operate on, must not be NULL
/// @return true if the cart existed in the store, else false
bool store_cart_remove(store_t *store, size_t cart_id);

/// @brief add merch to a cart
/// @param store the store to operate on, must not be NULL
/// @param cart_id the cart to add the merch to
/// @param name of the merch to add to cart
/// @param quantity the quantity of merch to add to cart
/// @return if successful the is_ok field is set to true and the error field
/// should be ignored, if is_ok is set to false the error field represents what
/// went wrong
cart_result_t store_cart_add_merch(store_t *store, size_t cart_id,
                                   const char *merch_name, size_t quantity);

/// @brief removes merch from a cart
/// @param store the store to operate on, must not be NULL
/// @param cart_id the cart to remove the merch from
/// @param name of the merch to remove from cart
/// @param quantity the quantity of merch to remove from cart
/// @return if successful the is_ok field is set to true and the error field
/// should be ignored, if is_ok is set to false the error field represents what
/// went wrong
cart_result_t store_cart_remove_merch(store_t *store, size_t cart_id,
                                      const char *merch_name, size_t quantity);

/// @brief calculats the cost of all merch in the given cart
/// @param store the store to operate on, must not be NULL
/// @param cart_id the cart to calculate the cost of
/// @param out_cost out parameter to store the cost in
/// @return returns false if cart is not in store else true
/// return false if cart nto found
bool store_cart_cost(store_t *store, size_t cart_id, size_t *out_cost);

/// @brief checkout a cart from the store,
/// this removes stock of the merch carried by cart from shelves in the store
///
/// @param store the store to operate on, must not be NULL
/// @param cart_id the cart to checkout
/// @return returns false if cart is not in store else true
bool store_cart_checkout(store_t *store, size_t cart_id);

/// @brief calculates the stock of a merch in the store
///
///
/// @param store the store to operate on, must not be NULL
/// @param merch name the merch to caluclate the stock fo
/// @return the stock quantity
size_t store_count_stock(store_t *store, const char *merch_name);
