#pragma once
#include "common.h"
#include "linked_list.h"
#include <stdbool.h>

/**
 * @file list_table_iterator.h
 * @author Elias Ekerby Alfred Frändberg
 * @date 28 september 2024
 * @brief Simple list iterator
 *
 * List iterators provide an easy way of iterating over the elements
 * of a list. The iterator has either current node in the list or it is
 * cosidered to have iterated over all elements which is indicated by the
 * 'ioopm_list_iterator_at_end' fucntion returning true. Modifying the list in
 * any way during iteration invalidates the iterator.
 *
 */

typedef struct list_iterator ioopm_list_iterator_t;

/// @brief Create a new iterator
/// @param l the list to iterate over
ioopm_list_iterator_t *ioopm_list_iterator_create(ioopm_list_t *l);

/// @brief Destroy the iterator and return its resources
/// @param iter the iterator
void ioopm_list_iterator_destroy(ioopm_list_iterator_t *iter);

/// @brief Checks if there are more elements to iterate over
/// @param iter the iterator
/// @return true if there is at least one more element
bool ioopm_list_iterator_at_end(ioopm_list_iterator_t *iter);

/// @brief Step the iterator forward one step
/// @param iter the iterator
void ioopm_list_iterator_advance(ioopm_list_iterator_t *iter);

/// @brief Return the current element from the underlying list
/// @param iter the iterator
/// @return the current element
elem_t ioopm_list_iterator_current(ioopm_list_iterator_t *iter);

/// NOTE: REMOVE IS OPTIONAL TO IMPLEMENT
/// @brief Remove the current element from the underlying list
/// @param iter the iterator
/// @return the removed element
int ioopm_list_iterator_remove(ioopm_list_iterator_t *iter);

/// NOTE: INSERT IS OPTIONAL TO IMPLEMENT
/// @brief Insert a new element into the underlying list making the current
/// element it's next
/// @param iter the iterator
/// @param element the element to be inserted
void ioopm_list_iterator_insert(ioopm_list_iterator_t *iter, elem_t element);
