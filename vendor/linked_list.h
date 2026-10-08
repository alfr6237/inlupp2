#pragma once
#include "common.h"
#include <stddef.h>

#include "linked_list_common.h"
#include <stdbool.h>

/**
 * @file lnked_list.h
 * @author Elias Ekerby Alfred Frändberg
 * @date 24 september 2024
 * @brief Simple linked list implementation
 *
 * A linked list provides a flexible way to store ordered elements
 * where the order is detemined by each element knowing which element is the
 * next. To add an element use either 'ioopm_list_prepend', 'ioopm_list_append'
 * or 'ioomp_list_insert' depending on where you want the element to go. To
 * remove and element simply call 'ioomp_list_remove'.
 */

/// @brief Creates a new empty list
/// @return an empty linked list
ioopm_list_t *ioopm_list_create(void);

/// @brief Tear down the linked list and return all its memory (but not the
/// memory of the elements)
/// @param list the list to be destroyed
void ioopm_list_destroy(ioopm_list_t *list);

/// @brief Insert at the end of a linked list in O(1) time
/// @param list the linked list that will be appended
/// @param value the value to be appended
void ioopm_list_append(ioopm_list_t *list, elem_t value);

/// @brief Insert at the front of a linked list in O(1) time
/// @param list the linked list that will be prepended to
/// @param value the value to be prepended
void ioopm_list_prepend(ioopm_list_t *list, elem_t value);

/// @brief Return the first element of a linked list in O(1) time
/// @pre the list is non-empty
/// @param list the linked list to take the head of
/// @param out value for the head value
/// @return if the head value exists
bool ioopm_list_head(ioopm_list_t *list, elem_t *value);

/// @brief Return the last element of a linked list in O(1) time
/// @pre the list is non-empty
/// @param list the linked list to take the last element of
/// @param out param for the last value in the list
/// @return if the last value exists
bool ioopm_list_last(ioopm_list_t *list, elem_t *value);

/// @brief Insert an element into a linked list in O(n) time.
/// The valid values of index are [0,n] for a list of n elements,
/// where 0 means before the first element and n means after
/// the last element.
/// @pre 0 <= index <= length(list)
/// @param list the linked list that will be extended
/// @param index the position in the list
/// @param value the value to be inserted
/// @return if the value was inserted succesfully
bool ioopm_list_insert(ioopm_list_t *list, size_t index, elem_t value);

/// @brief Remove an element from a linked list in O(n) time.
/// The valid values of index are [0,n-1] for a list of n elements,
/// where 0 means the first element and n-1 means the last element.
/// @pre 0 <= index < length(list)
/// @param list the linked list
/// @param index the position in the list
/// @param out parameter for the value removed
/// @return if the value was removed, if false value parameter should not be
/// used
bool ioopm_list_remove(ioopm_list_t *list, size_t index, elem_t *value);

/// @brief Retrieve an element from a linked list in O(n) time.
/// The valid values of index are [0,n-1] for a list of n elements,
/// where 0 means the first element and n-1 means the last element.
/// @pre 0 <= index < length(list)
/// @param list the linked list that will be extended
/// @param index the position in the list
/// @param out param for the value to get
/// @return if the value was succesfully retrieved
bool ioopm_list_get(ioopm_list_t *list, size_t index, elem_t *value);

/// @brief Lookup the number of elements in the linked list in O(1) time
/// @param list the linked list
/// @return the number of elements in the list
int ioopm_list_size(ioopm_list_t *list);

/// @brief Test whether a list is empty or not
/// @param list the linked list
/// @return true if the number of elements int the list is 0, else false
bool ioopm_list_is_empty(ioopm_list_t *list);
