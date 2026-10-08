#include <stdbool.h>
#include <stdlib.h>

#include "common.h"
#include "linked_list_common.h"
#include "linked_list_iterator.h"

struct list_iterator
{
  ioopm_list_t *list;
  node_t *current_node;
};

ioopm_list_iterator_t *ioopm_list_iterator_create(ioopm_list_t *l)
{
  ioopm_list_iterator_t *it = malloc(sizeof(ioopm_list_iterator_t));
  it->list = l;
  it->current_node = l->head;
  return it;
}

void ioopm_list_iterator_destroy(ioopm_list_iterator_t *iter) { free(iter); }

bool ioopm_list_iterator_at_end(ioopm_list_iterator_t *iter)
{
  return iter->current_node == NULL;
}

void ioopm_list_iterator_advance(ioopm_list_iterator_t *iter)
{
  iter->current_node = iter->current_node->next;
}

elem_t ioopm_list_iterator_current(ioopm_list_iterator_t *iter)
{
  return iter->current_node->value;
}
