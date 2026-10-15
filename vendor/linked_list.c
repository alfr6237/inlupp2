#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include "linked_list.h"
#include "linked_list_iterator.h"
#include "linked_list_common.h"

// Allocates space for list
ioopm_list_t *ioopm_list_create(void)
{
    return calloc(1, sizeof(ioopm_list_t));
}

void ioopm_list_destroy(ioopm_list_t *list)
{
    //starting from first entry
    link_t *current_link = list->first;
    link_t *current_next;
    
    // when not empty, free every link 
    while (current_link != NULL)
    {
        current_next = current_link->next;
        free(current_link);
        current_link = current_next;    
    }
    
    //When all links free'd, free list
    free(list);
}

elem_t ioopm_list_head(ioopm_list_t *list)
{
    return list->first->element;
}

elem_t ioopm_list_last(ioopm_list_t *list)
{
    return list->last->element;
}

void ioopm_list_append(ioopm_list_t *list, elem_t value)
{
    link_t *link = calloc(1, sizeof(link_t));
    link->element = value;
    if (list->first == NULL) // when list is empty
    {
        list->last = link;
        list->first = link; // first and last points to same link (just one link)
        list->size++; // increase the "size" of list
    }
    else // List is not empty
    {
        list->last->next = link; //old last get new link as next
        list->last = link; //new link is last
        list->size++;
    }
}

void ioopm_list_prepend(ioopm_list_t *list, elem_t value)
{
    link_t *link = calloc(1, sizeof(link_t));
    link->element = value;
    if (list->first == NULL) // When list is empty
    {
        list->first = link; 
        list->last = link; // first and last points to same link (just on link)
        list->size++; //increase the "size" of list
    }
    else 
    {
        link_t *prev_first = list->first; // previous first 
        link->next = prev_first; // new link get old first as next
        list->first = link; // new link is first
        list->size++;
    }    
}

size_t ioopm_list_size(ioopm_list_t *list)
{
    return list->size;
}

bool ioopm_list_is_empty(ioopm_list_t *list)
{
    return (list->size == 0);
}

// presumes index is valid
static link_t *find_link(ioopm_list_t *list, const size_t index)
{   
    //starting at first link
    link_t *current_link = list->first;
    size_t counter = 0;

    while (counter != index) // current_link not at desired index
    {
        current_link = current_link->next; //get next link
        counter++;
    }
    return current_link;
}

//returns element at index
elem_t ioopm_list_get(ioopm_list_t *list, const size_t index)
{
    link_t *link = find_link(list, index); //find link at index
    elem_t value = link->element;

    return value;
}

// inserts element on index
void ioopm_list_insert(ioopm_list_t *list, const size_t index, elem_t value)
{
    size_t size = ioopm_list_size(list);
    
    if (index == 0)//if want insert first, prepend
    {
        ioopm_list_prepend(list, value);
    }
    else if (index == size)//if want insert last, append
    {
        ioopm_list_append(list, value);
    }
    else //if want insert "middle"
    {
        link_t *to_insert = calloc(1, sizeof(link_t));
        to_insert->element = value; //initialize link with desired element
        
        link_t *previous_link = find_link(list, index - 1);  //find prev_link    
        link_t *next_link = previous_link->next; //get new next_link
        
        to_insert->next = next_link; //new link get next_link as next
        previous_link->next = to_insert; //prev_link get new link as next

        list->size++; //increase "size" of list
    }
}

elem_t ioopm_list_remove(ioopm_list_t *list, const size_t index)
{
    size_t size = ioopm_list_size(list);
    elem_t result; // will be value of element in desired link

    if (index == 0) // remove first link
    {
        link_t *new_first = list->first->next; // get new first
        result = list->first->element; // save value of current first
        
        free(list->first); // free current first 
        list->first = new_first; // list first-pointer to new first

        list->size--; // decrease "size" of list 
        return result;
    }
    else if (index == size-1) // remove last link
    {
        link_t *second_last = find_link(list, index - 1); // find link before last
        second_last->next = NULL; // new last link points to NULL

        result = list->last->element; // save value of current last 
        free(list->last); // free current last
        
        list->last = second_last; // list last-pointer to new last
        list->size--; // decrease "size" of list
        return result;
    }
    else // remove link at index 
    {
        link_t *previous = find_link(list, index-1); //find prev_link
        link_t *to_remove = previous->next; //get link to_remove
        link_t *to_remove_next = to_remove->next; //get next_link
        
        result = to_remove->element; //get elem of link_to_remove
        free(to_remove); //free to_remove

        previous->next = to_remove_next; //prev_link get next_link as next

        list->size--; //reduce "size" of list
        return result;
    }
}
