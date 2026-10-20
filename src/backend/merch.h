#pragma once
#include "../../vendor/linked_list.h"
#include <stddef.h>

typedef struct merch ioopm_merch_t;

struct merch {
  char *name;
  char *desc;
  size_t price;
};

ioopm_merch_t *ioopm_merch_create(const char *name, const char *desc,
                                  const size_t price);
void ioopm_merch_destroy(ioopm_merch_t *merch);

void ioopm_merch_edit_name(ioopm_merch_t *merch, const char *new_name);
void ioopm_merch_edit_desc(ioopm_merch_t *merch, const char *new_desc);
void ioopm_merch_edit_price(ioopm_merch_t *merch, const size_t new_price);
