#include "linked_list.h"
#include "common.h"
#include "linked_list_common.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

static node_t *node_create(elem_t value, node_t *next);
static node_t *node_destroy(node_t *node);
static bool get_link_at_index(ioopm_list_t *list, size_t index,
                              node_t ***out_link);

ioopm_list_t *ioopm_list_create(void)
{

  ioopm_list_t *list = calloc(1, sizeof(ioopm_list_t));
  list->head = NULL;
  list->last = NULL;
  return list;
}

void ioopm_list_destroy(ioopm_list_t *list)
{
  node_t *next = list->head;
  while (next != NULL)
  {
    next = node_destroy(next);
  }
  free(list);
}

void ioopm_list_append(ioopm_list_t *list, elem_t value)
{
  node_t *new_node = node_create(value, NULL);

  if (ioopm_list_is_empty(list))
  {
    list->head = new_node;
    list->last = new_node;
  }
  else
  {
    list->last->next = new_node;
    list->last = new_node;
  }
}

void ioopm_list_prepend(ioopm_list_t *list, elem_t value)
{
  node_t *new_node = node_create(value, list->head);
  if (ioopm_list_is_empty(list))
  {
    list->last = new_node;
  }
  list->head = new_node;
}

bool ioopm_list_head(ioopm_list_t *list, elem_t *value)
{
  if (ioopm_list_is_empty(list))
  {
    return false;
  }
  *value = list->head->value;
  return true;
}

bool ioopm_list_last(ioopm_list_t *list, elem_t *value)
{
  if (ioopm_list_is_empty(list))
  {
    return false;
  }

  *value = list->last->value;
  return true;
}

bool ioopm_list_insert(ioopm_list_t *list, size_t index, elem_t value)
{
  node_t **link;

  if (!get_link_at_index(list, index, &link))
  {
    return false;
  }

  node_t *new_node = node_create(value, *link);
  *link = new_node;

  return true;
}

bool ioopm_list_remove(ioopm_list_t *list, size_t index, elem_t *value)
{
  node_t **link;

  if (!get_link_at_index(list, index, &link))
  {
    return false;
  }

  node_t *old = *link;
  *value = old->value;
  *link = old->next;
  node_destroy(old);
  return true;
}

bool ioopm_list_get(ioopm_list_t *list, size_t index, elem_t *value)
{
  node_t **link;
  if (!get_link_at_index(list, index, &link))
  {
    return false;
  }
  *value = (*link)->value;
  return true;
}

int ioopm_list_size(ioopm_list_t *list)
{
  node_t *cursor = list->head;
  int size = 0;
  while (cursor != NULL)
  {
    cursor = cursor->next;
    size++;
  }
  return size;
}

bool ioopm_list_is_empty(ioopm_list_t *list) { return list->head == NULL; }

static bool get_link_at_index(ioopm_list_t *list, size_t index,
                              node_t ***out_link)
{

  node_t **cursor = &list->head;

  for (size_t i = 0; i < index; i++)
  {
    if (*cursor == NULL)
    {
      return false;
    }

    cursor = &(*cursor)->next;
  }

  *out_link = cursor;
  return true;
}

static node_t *node_create(elem_t value, node_t *next)
{
  node_t *node = malloc(sizeof(node_t));
  node->value = value;
  node->next = next;
  return node;
}

// destroys the node and returs its next pointer
static node_t *node_destroy(node_t *node)
{
  node_t *next = node->next;
  free(node);
  return next;
}
