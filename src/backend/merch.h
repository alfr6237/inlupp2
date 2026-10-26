#pragma once
#include "../../vendor/linked_list.h"
#include <stddef.h>
#include <stdio.h>

typedef struct merch merch_t;

struct merch {
  char *name;
  char *desc;
  size_t price;
};

merch_t *merch_create(const char *name, const char *desc, const size_t price);
void merch_destroy(merch_t *merch);
