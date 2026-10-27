#include "linked_list.h"
#include "linked_list_common.h"
// #include "linked_list_iterator.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

// Allocates space for list
ioopm_list_t *ioopm_list_create(list_remove_fn_t *remove_fn) {
  ioopm_list_t *list = calloc(1, sizeof(ioopm_list_t));
  list->remove_fn = remove_fn;
  return list;
}

void ioopm_list_destroy(ioopm_list_t *list) {
  // starting from first entry
  link_t *current_link = list->first;
  link_t *current_next;

  // when not empty, free every link
  while (current_link != NULL) {
    current_next = current_link->next;
    if (list->remove_fn) {
      list->remove_fn(current_link->element);
    }
    free(current_link);
    current_link = current_next;
  }

  // When all links free'd, free list
  free(list);
}

elem_t ioopm_list_head(const ioopm_list_t *list) {
  return list->first->element;
}

elem_t ioopm_list_last(const ioopm_list_t *list) { return list->last->element; }

void ioopm_list_append(ioopm_list_t *list, elem_t value) {
  link_t *link = calloc(1, sizeof(link_t));
  link->element = value;
  if (list->first == NULL) // when list is empty
  {
    list->last = link;
    list->first = link; // first and last points to same link (just one link)
    list->size++;       // increase the "size" of list
  } else                // List is not empty
  {
    list->last->next = link; // old last get new link as next
    list->last = link;       // new link is last
    list->size++;
  }
}

void ioopm_list_prepend(ioopm_list_t *list, elem_t value) {
  link_t *link = calloc(1, sizeof(link_t));
  link->element = value;
  if (list->first == NULL) // When list is empty
  {
    list->first = link;
    list->last = link; // first and last points to same link (just on link)
    list->size++;      // increase the "size" of list
  } else {
    link_t *prev_first = list->first; // previous first
    link->next = prev_first;          // new link get old first as next
    list->first = link;               // new link is first
    list->size++;
  }
}

size_t ioopm_list_size(const ioopm_list_t *list) { return list->size; }

bool ioopm_list_is_empty(const ioopm_list_t *list) { return (list->size == 0); }

// presumes index is valid
static link_t *find_link(const ioopm_list_t *list, const size_t index) {
  // starting at first link
  link_t *current_link = list->first;
  size_t counter = 0;

  while (counter != index) // current_link not at desired index
  {
    current_link = current_link->next; // get next link
    counter++;
  }
  return current_link;
}

// returns element at index
elem_t ioopm_list_get(const ioopm_list_t *list, const size_t index) {
  link_t *link = find_link(list, index); // find link at index
  elem_t value = link->element;

  return value;
}

// inserts element on index
void ioopm_list_insert(ioopm_list_t *list, const size_t index, elem_t value) {
  size_t size = ioopm_list_size(list);

  if (index == 0) // if want insert first, prepend
  {
    ioopm_list_prepend(list, value);
  } else if (index == size) // if want insert last, append
  {
    ioopm_list_append(list, value);
  } else // if want insert "middle"
  {
    link_t *to_insert = calloc(1, sizeof(link_t));
    to_insert->element = value; // initialize link with desired element

    link_t *previous_link = find_link(list, index - 1); // find prev_link
    link_t *next_link = previous_link->next;            // get new next_link

    to_insert->next = next_link;     // new link get next_link as next
    previous_link->next = to_insert; // prev_link get new link as next

    list->size++; // increase "size" of list
  }
}

elem_t ioopm_list_remove(ioopm_list_t *list, const size_t index) {
  size_t size = ioopm_list_size(list);
  elem_t result; // will be value of element in desired link

  if (index == 0) // remove first link
  {
    link_t *new_first = list->first->next; // get new first
    result = list->first->element;         // save value of current first

    free(list->first);       // free current first
    list->first = new_first; // list first-pointer to new first

    list->size--; // decrease "size" of list
    return result;
  } else if (index == size - 1) // remove last link
  {
    link_t *second_last = find_link(list, index - 1); // find link before last
    second_last->next = NULL; // new last link points to NULL

    result = list->last->element; // save value of current last
    if (list->remove_fn) {
      list->remove_fn(list->last->element);
    }
    free(list->last); // free current last

    list->last = second_last; // list last-pointer to new last
    list->size--;             // decrease "size" of list
    return result;
  } else // remove link at index
  {
    link_t *previous = find_link(list, index - 1); // find prev_link
    link_t *to_remove = previous->next;            // get link to_remove
    link_t *to_remove_next = to_remove->next;      // get next_link

    result = to_remove->element; // get elem of link_to_remove
    if (list->remove_fn) {
      list->remove_fn(to_remove->element);
    }
    free(to_remove); // free to_remove

    previous->next = to_remove_next; // prev_link get next_link as next

    list->size--; // reduce "size" of list
    return result;
  }
}

static void swap_link (link_t **prev_next_a, link_t **prev_next_b)
{
  assert(prev_next_a != prev_next_b);

  //extracts pointers relevant for first link
  link_t *link_a = *prev_next_a;
  link_t *link_a_next = link_a->next;

  //extracts pointer for second link
  link_t *link_b = *prev_next_b;
  link_t *link_b_next = link_b->next;

  if(link_b->next == link_a)
  {
    link_a->next = link_b;
    link_b->next = link_a_next;
    *prev_next_b = link_a;
  }
  else if(link_a->next == link_b)
  {
    link_b->next = link_a;
    link_a->next = link_b_next;
    *prev_next_a = link_b;
  }
  else
  {
    link_a->next = link_b_next;
    link_b->next = link_a_next;
    *prev_next_a = link_b;
    *prev_next_b = link_a;
  }
  
  // //make previous pointer to a now point to b, a:s next pointer is now b:s next
  // *prev_next_a = link_b;
  // if(link_b->next == link_a)
  // {
  //   link_a->next = link_b;
  // }
  // else
  // {
  //   link_a->next = link_b->next;
  // }
  // //make prev pointer of b now point to a, next pointer of b is now old next pointer of a
  // *prev_next_b = link_a;
  // if(link_a->next == link_b)
  // {
  //   link_b->next = link_a;
  // }
  // else
  // {
  //   link_b->next = link_a_next;
  // }
  
}

void ioopm_list_sort(ioopm_list_t *list, less_than_fn_t *less_than_fn) 
{
  link_t **current = &list->first;
  while(*current != NULL)
  {
    puts("\nouter loop start");
    link_t **swap_cand = &(*current)->next;
    while(*swap_cand != NULL)
    {
      puts("\ninner loop start");
      if (less_than_fn((*current)->element, (*swap_cand)->element))
      {
        puts("\nswap initiated");
        swap_link(current, swap_cand);
      }

      printf("\neql: %s\n", swap_cand == &(*swap_cand)->next ? "true" : "false");
      swap_cand = &(*swap_cand)->next;
    }
    current = &(*current)->next;
  }
}
