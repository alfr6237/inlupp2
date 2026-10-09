#pragma once
#include "../../vendor/linked_list.h"
#include <stddef.h>

typedef struct merch ioopm_merch_t;

struct merch {
  char *name;
  char *desc;

  size_t price;

  ioopm_list_t *locations; // list of shelves
};

ioopm_merch_t *ioopm_merch_create(const char *name, const char *desc,
                                  size_t price);
void ioopm_merch_destroy(ioopm_merch_t *merch);
