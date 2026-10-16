#pragma once
#include "common.h"
#include <stddef.h>

typedef struct link link_t;
typedef struct list ioopm_list_t;
typedef void list_remove_fn_t(elem_t elem);

struct list {
  link_t *first;
  link_t *last;
  size_t size;
  list_remove_fn_t *remove_fn;
};

struct link {
  elem_t element;
  link_t *next;
};

struct list_iterator {
  ioopm_list_t *list;
  link_t *current_link;
  size_t current_index;
};
