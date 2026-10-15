#include "linked_list_common.h"
#include "linked_list_iterator.h"
#include "linked_list.h"
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

ioopm_list_iterator_t *ioopm_list_iterator_create(ioopm_list_t *list)
{
    ioopm_list_iterator_t *iter = calloc(1, sizeof(ioopm_list_iterator_t));
    iter->list = list;
    iter->current_link = list->first;

    return iter;
} 

void ioopm_list_iterator_destroy(ioopm_list_iterator_t *iter)
{
    free(iter);
}

// return true if at end 
bool ioopm_list_iterator_at_end(ioopm_list_iterator_t *iter)
{
    return (iter->current_link == NULL);
}

void ioopm_list_iterator_advance(ioopm_list_iterator_t *iter)
{   
    iter->current_link = iter->current_link->next; // current_link points to next link instead
    iter->current_index++; // index of iter
}

elem_t ioopm_list_iterator_current(ioopm_list_iterator_t *iter)
{
    return iter->current_link->element;
}


elem_t ioopm_list_iterator_remove(ioopm_list_iterator_t *iter)
{
    //get next link
    link_t *new_current_link = iter->current_link->next;

    //remove old iter current from list
    elem_t removed_elem = ioopm_list_remove(iter->list, iter->current_index);

    //iter current is now next link
    iter->current_link = new_current_link;

    return removed_elem; 
}

void ioopm_list_iterator_insert(ioopm_list_iterator_t *iter, elem_t element)
{ 
    // insert element at current index of iter
    ioopm_list_insert(iter->list, iter->current_index, element);
    // e.g. list = (1,2,3) -> insert(list, 1, 7) -> list = (1,7,2,3)

    //inserted elem infront of current, current idx increment
    iter->current_index++;
}
