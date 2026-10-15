#pragma once
#include <stddef.h>
#include "common.h"

typedef struct link link_t;
typedef struct list ioopm_list_t;

struct list 
{
    link_t *first;
    link_t *last;   
    size_t size;
};

struct link
{
    elem_t element;
    link_t *next; 
};

struct list_iterator
{
    ioopm_list_t *list;
    link_t *current_link;
    size_t current_index;
};