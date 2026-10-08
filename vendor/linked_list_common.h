#pragma once
#include "common.h"
typedef struct list ioopm_list_t;
typedef struct node node_t;
struct list
{
  node_t *head;
  node_t *last;
};

struct node
{
  elem_t value;
  node_t *next;
};
